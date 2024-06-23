#include "onvif_base_service.h"
#include "onvif_base_service_local.h"
#include "generated/onvifsoap.nsmap"
#include "onvif_credentials.h"
#include "wsseapi.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "clogger.h"
#include "plugin/logging.h"
#include "httpda.h"

struct _OnvifBaseService {
    OnvifDevice * device;
    char * endpoint;
    P_MUTEX_TYPE service_lock;
    P_MUTEX_TYPE prop_lock;
    struct http_da_info *  da_info;
};

OnvifBaseService * OnvifBaseService__create(OnvifDevice * device, const char * endpoint){
    OnvifBaseService * self = malloc(sizeof(OnvifBaseService));
    OnvifBaseService__init(self,device, endpoint);
    return self;
}

void OnvifBaseService__init(OnvifBaseService * self,OnvifDevice * device, const  char * endpoint){
    self->device = device;

    self->endpoint = malloc(strlen(endpoint)+1);
    strcpy(self->endpoint,endpoint);

    self->da_info = NULL;
    
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
        if(self->da_info){
            //It might be more efficient to clean up da_info manually
            struct soap fakesoap;
            soap_init1(&fakesoap, SOAP_XML_CANONICAL | SOAP_C_UTFSTRING);
            soap_register_plugin(&fakesoap, http_da);
            http_da_release(&fakesoap, self->da_info);
            free(self->da_info);
            soap_destroy(&fakesoap);
            soap_end(&fakesoap);
            soap_done(&fakesoap);
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
        char * endpoint = OnvifBaseService__get_endpoint(self);
        C_TRACE("[%s] No time adjustment required. Camera in sync with client.",endpoint);
        free(endpoint);
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

int OnvifBaseService__http_challenge(OnvifBaseService * self, SoapDef * soap, char * url){
    if(!soap->authrealm){
        return 0;
    }
    
    C_DEBUG("[%s] WWW-Authorization challenge '%s'",url,soap->authrealm);

    char * user = OnvifCredentials__get_username(OnvifDevice__get_credentials(self->device));
    char * pass = OnvifCredentials__get_password(OnvifDevice__get_credentials(self->device));
    if(!self->da_info){
        self->da_info = malloc(sizeof(struct http_da_info));
        memset (self->da_info, 0, sizeof(struct http_da_info));
    }
    http_da_save(soap, self->da_info, soap->authrealm, user, pass);
    free(user);
    free(pass);
    //Reapply wsse data after challenge started
    int wsseret = OnvifBaseService__set_wsse_data(self,soap);
    if(!wsseret){
        OnvifBaseService__soap_destroy(self, soap);
        return 0;
    }

    return 1;
}

SoapDef * OnvifBaseService__soap_new(OnvifBaseService * self){
    struct soap * soap = soap_new1(SOAP_XML_CANONICAL | SOAP_C_UTFSTRING); //SOAP_XML_STRICT may cause crash
    soap->connect_timeout = 2; // 2 sec
    soap->recv_timeout = soap->send_timeout = 10;//10 sec
    soap_register_plugin(soap, soap_wsse);
    soap_register_plugin(soap, http_da);
    soap_set_namespaces(soap, onvifsoap_namespaces);

    soap_ssl_client_context(soap,
        SOAP_SSL_SKIP_HOST_CHECK,
        NULL,          /* no keyfile */
        NULL,          /* no keyfile password */
        NULL,  /* self-signed certificate cacert.pem */
        NULL,          /* no capath to trusted certificates */
        NULL           /* no random data to seed randomness */
    );
    soap->fsslverify = OnvifBaseService__ssl_verify_callback;

    if(self->da_info)
        http_da_restore(soap, self->da_info);
        
    int wsseret = OnvifBaseService__set_wsse_data(self,soap);

    if(!wsseret){
        OnvifBaseService__soap_destroy(self, soap);
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

void OnvifBaseService__soap_destroy(OnvifBaseService * self, SoapDef * soap){
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

//TODO Placeholder until user-input is implemented
int OnvifBaseService__ssl_verify_callback(int ok, X509_STORE_CTX *store)
{
    if (!ok){
        int err = X509_STORE_CTX_get_error(store);
        switch (err){
            case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
            case X509_V_ERR_UNABLE_TO_GET_CRL:
            case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
                X509_STORE_CTX_set_error(store, X509_V_OK);
                ok = 1;
                break;
            default:
                C_ERROR("SSL ERROR : %d",err);
                break;
        }
    }
    return ok;
}