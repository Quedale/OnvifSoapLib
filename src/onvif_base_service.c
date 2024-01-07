#include "onvif_base_service.h"
#include "generated/DeviceBinding.nsmap"
#include "onvif_credentials.h"
#include "wsse2api.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "clogger.h"

#define FAULT_UNAUTHORIZED "[\"http://www.onvif.org/ver10/error\":NotAuthorized]"

struct _OnvifBaseService {
    char * endpoint;
    OnvifCredentials * credentials;
    P_MUTEX_TYPE service_lock;
    P_MUTEX_TYPE prop_lock;
    void (*error_cb)(OnvifErrorTypes type, void * user_data);
    void * error_data;
};

OnvifBaseService * OnvifBaseService__create(const char * endpoint, OnvifCredentials * credentials, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    OnvifBaseService * self = malloc(sizeof(OnvifBaseService));
    OnvifBaseService__init(self,endpoint,credentials, error_cb, error_data);
    return self;
}

void OnvifBaseService__init(OnvifBaseService * self,const char * endpoint, OnvifCredentials * credentials, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    self->error_cb = error_cb;
    self->error_data = error_data;
    self->credentials = credentials;

    self->endpoint = malloc(strlen(endpoint)+1);
    strcpy(self->endpoint,endpoint);

    P_MUTEX_SETUP(self->service_lock);

    P_MUTEX_SETUP(self->prop_lock);
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

OnvifCredentials * OnvifBaseService__get_credentials(OnvifBaseService * self){
    return self->credentials;
}

int OnvifBaseService__set_wsse_data(OnvifBaseService * self, SoapDef * soap){
    int ret = 1;
    char * user = OnvifCredentials__get_username(self->credentials);
    char * pass = OnvifCredentials__get_password(self->credentials);
    soap_wsse_delete_Security(soap);
    if(user && pass){
        if (soap_wsse_add_Timestamp(soap, "Time", 10)
            || soap_wsse_add_UsernameTokenDigest(soap, "Auth",user,pass)){
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
    soap->connect_timeout = soap->recv_timeout = soap->send_timeout = 10; // 10 sec
    soap_register_plugin(soap, soap_wsse);

    int wsseret = OnvifBaseService__set_wsse_data(self,soap);

    if(!wsseret){
        OnvifBaseService__soap_destroy(soap);
        soap = NULL;
    }

    return (SoapDef *) soap;
}

void OnvifBaseService__soap_destroy(SoapDef * soap){
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
        OnvifBaseService__set_error_code(self,ONVIF_CONNECTION_ERROR);
    } else if (strcmp(soap_fault_subcode(soap),FAULT_UNAUTHORIZED)) {
        C_WARN("Warning : NOT AUTHORIZED\n");
        OnvifBaseService__set_error_code(self,ONVIF_NOT_AUTHORIZED);
    } else { //Mostly SOAP_FAULT
        C_ERROR("SOAP ERROR %i [%s]\n",error_code, soap_fault_subcode(soap));
        soap_print_fault(soap, stderr);
        OnvifBaseService__set_error_code(self,ONVIF_SOAP_ERROR);
    }
}