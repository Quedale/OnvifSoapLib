#include "onvif_base_service.h"
#include "generated/DeviceBinding.nsmap"
#include "onvif_credentials.h"
#include "wsseapi.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "clogger.h"
#include "plugin/logging.h"

#define FAULT_UNAUTHORIZED "[\"http://www.onvif.org/ver10/error\":NotAuthorized]"

struct _OnvifBaseService {
    OnvifDevice * device;
    char * endpoint;
    P_MUTEX_TYPE service_lock;
    P_MUTEX_TYPE prop_lock;
    void (*error_cb)(OnvifErrorTypes type, void * user_data);
    void * error_data;
};

OnvifBaseService * OnvifBaseService__create(OnvifDevice * device, const char * endpoint, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    OnvifBaseService * self = malloc(sizeof(OnvifBaseService));
    OnvifBaseService__init(self,device, endpoint, error_cb, error_data);
    return self;
}

void OnvifBaseService__init(OnvifBaseService * self,OnvifDevice * device, const  char * endpoint, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    self->error_cb = error_cb;
    self->error_data = error_data;
    self->device = device;

    self->endpoint = malloc(strlen(endpoint)+1);
    strcpy(self->endpoint,endpoint);

    P_MUTEX_SETUP(self->service_lock);

    P_MUTEX_SETUP(self->prop_lock);
}

void OnvifBaseService__set_endpoint(OnvifBaseService * self, char * endpoint){
    P_MUTEX_LOCK(self->prop_lock);
    self->endpoint = realloc(self->endpoint, strlen(endpoint)+1);
    strcpy(self->endpoint, endpoint);
    P_MUTEX_UNLOCK(self->prop_lock);
}

void OnvifBaseService__destroy(OnvifBaseService * self){
    if (self) {
        if(self->endpoint){
            free(self->endpoint);
        }
        P_MUTEX_CLEANUP(self->prop_lock);
        P_MUTEX_CLEANUP(self->service_lock);
        free(self);
    }
}

void OnvifBaseService__lock(OnvifBaseService * self){
    P_MUTEX_LOCK(self->service_lock);
}

void OnvifBaseService__unlock(OnvifBaseService * self){
    P_MUTEX_UNLOCK(self->service_lock);
}

char * OnvifBaseService__get_endpoint(OnvifBaseService * self){
    char * ret = NULL;
    P_MUTEX_LOCK(self->prop_lock);
    ret = malloc(strlen(self->endpoint)+1);
    strcpy(ret,self->endpoint);
    P_MUTEX_UNLOCK(self->prop_lock);
    return ret;
}

OnvifDevice * OnvifBaseService__get_device(OnvifBaseService * self){
    return self->device;
}

time_t OnvifBaseService__get_offset_time(OnvifBaseService * self){
    double offset = OnvifDevice__getTimeOffset(self->device);

    time_t now = time( NULL);
    if(offset == 0){
        return now;
    }

    struct tm now_tm = *localtime( &now);
    now_tm.tm_sec += offset;

    time_t ntime = mktime(&now_tm);

    char buff1[20];
    strftime(buff1, 20, "%Y-%m-%d %H:%M:%S", gmtime(&now));

    char buff2[20];
    strftime(buff2, 20, "%Y-%m-%d %H:%M:%S", gmtime(&ntime));

    char * endpoint = OnvifBaseService__get_endpoint(self);
    C_TRACE("[%s] Time adjustment [%s] to [%s] offset %.0lf", endpoint, buff1, buff2, offset);
    free(endpoint);
    return ntime;
}

int OnvifBaseService__set_wsse_data(OnvifBaseService * self, SoapDef * soap){
    int ret = 1;
    char * user = OnvifCredentials__get_username(OnvifDevice__get_credentials(self->device));
    char * pass = OnvifCredentials__get_password(OnvifDevice__get_credentials(self->device));
    soap_wsse_delete_Security(soap);
    if(user && pass){
        if (soap_wsse_add_Timestamp(soap, "Time", 10)
            || soap_wsse_add_UsernameTokenDigest_at(soap, "Auth",user,pass,OnvifBaseService__get_offset_time(self))){ //TODO Set camera time offset
            //TODO Error handling
            C_ERROR("Unable to set wsse creds...\n");
            ret = 0;
        }
    }

    //TODO support encryption
    free(user);
    free(pass);
    return ret;
}

SoapDef * OnvifBaseService__soap_new(OnvifBaseService * self){
    struct soap * soap = soap_new1(SOAP_XML_CANONICAL | SOAP_C_UTFSTRING); //SOAP_XML_STRICT may cause crash
    soap->connect_timeout = 2; // 2 sec
    soap->recv_timeout = soap->send_timeout = 10;//10 sec
    soap_register_plugin(soap, soap_wsse);

    int wsseret = OnvifBaseService__set_wsse_data(self,soap);

    if(!wsseret){
        OnvifBaseService__soap_destroy(soap);
        soap = NULL;
    }


    char *debug_flag = NULL;

    if (( debug_flag =getenv( "ONVIF_DEBUG" )) != NULL )
        C_INFO("ONVIF_DEBUG variable set. '%s'",debug_flag) ;

    if(debug_flag){
        soap_register_plugin(soap, logging);
        soap_set_logging_outbound(soap,stdout);
        soap_set_logging_inbound(soap,stdout);
    }


    return (SoapDef *) soap;
}

void OnvifBaseService__soap_destroy(SoapDef * soap){
    char *debug_flag = NULL;

    if (( debug_flag =getenv( "ONVIF_DEBUG" )) != NULL )
        C_INFO("ONVIF_DEBUG variable set. '%s'",debug_flag) ;

    if(debug_flag){
        soap_set_logging_outbound(soap,NULL);
        soap_set_logging_inbound(soap,NULL);
    }

    soap_wsse_delete_Security(soap);
    soap_wsse_verify_done(soap);
    soap_destroy(soap);
    soap_end(soap);
    soap_done(soap);
    soap_free(soap);
}

void OnvifBaseService__set_error_code(OnvifBaseService * self, OnvifErrorTypes code){
    if(self->error_cb) self->error_cb(code,self->error_data);
}

void OnvifBaseService__handle_soap_error(OnvifBaseService * self, struct soap * soap, int error_code){
    if (error_code == SOAP_UDP_ERROR || 
        error_code == SOAP_TCP_ERROR || 
        error_code == SOAP_HTTP_ERROR || 
        error_code == SOAP_SSL_ERROR || 
        error_code == SOAP_ZLIB_ERROR){
        C_ERROR("CONNECTION ERROR\n");
        OnvifBaseService__set_error_code(self,ONVIF_ERROR_CONNECTION);
    } else if(error_code == SOAP_NO_TAG){
        C_ERROR("ERROR : Server didn't return a soap message.");
        OnvifBaseService__set_error_code(self,ONVIF_ERROR_NOT_VALID);
    } else if(error_code == 400){
        C_ERROR("ERROR : Server returned 400 bad request");
        OnvifBaseService__set_error_code(self,ONVIF_ERROR_NOT_VALID);
    } else {
        const char * fault_code = soap_fault_subcode(soap);
        if (fault_code && strcmp(fault_code,FAULT_UNAUTHORIZED)) {
            if(soap->error == 400 || //Bad request
                soap->error == 403 || //Forbidden (soap not authorized returns a 200)
                soap->error == 404){ //Not found
                C_ERROR("ERROR : NOT VALID ONVIF HTTP Error code [%d]\n",soap->error);
                OnvifBaseService__set_error_code(self,ONVIF_ERROR_NOT_VALID);
            } else {
                C_WARN("Warning : NOT AUTHORIZED HTTP Error code [%d]\n",soap->error);
                OnvifBaseService__set_error_code(self,ONVIF_ERROR_NOT_AUTHORIZED);
            }
        } else { //Mostly SOAP_FAULT
            C_ERROR("SOAP ERROR %i [%s]\n",error_code, fault_code);
            soap_print_fault(soap, stderr);
            OnvifBaseService__set_error_code(self,ONVIF_ERROR_SOAP);
        }
    }
}