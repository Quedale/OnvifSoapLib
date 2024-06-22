#include "onvif_device.h"
#include "onvif_credentials.h"
#include "ip_match.h"
#include "clogger.h"
#include "url_parser.h"
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
/* We are on Windows */
# define strtok_r strtok_s
#endif

typedef struct _OnvifDevice {
    ParsedURL * purl;
    time_t * datetime;
    double time_offset;
    P_MUTEX_TYPE auth_lock;
    int authenticated;

    OnvifErrorTypes last_error;
    P_MUTEX_TYPE prop_lock;
    
    OnvifDeviceService * device_service;
    P_MUTEX_TYPE media_lock;
    OnvifMediaService * media_service;

    OnvifCredentials * credentials;
} OnvifDevice;

void OnvifDevice__set_last_error(OnvifErrorTypes error, void * error_data);

SoapFault OnvifDevice__createMediaService(OnvifDevice* self){
    SoapFault ret_fault = SOAP_FAULT_NONE;
    if(self->media_service){
        return ret_fault;
    }

    P_MUTEX_LOCK(self->media_lock);

    //Check again in case it was created during lock
    if(self->media_service){
        goto exit;
    }

    ONVIF_DEVICE_TRACE("[%s] OnvifDevice__createMediaService",self);

    OnvifMedia * media;
    OnvifCapabilities* capabilities = OnvifDeviceService__getCapabilities(self->device_service);
    SoapFault * caps_fault = SoapObject__get_fault(SOAP_OBJECT(capabilities));
    SoapFault service_fault;
    switch(*caps_fault){
        case SOAP_FAULT_NONE:
            media = OnvifCapabilities__get_media(capabilities);
            self->media_service = OnvifMediaService__create(self, OnvifMedia__get_address(media),OnvifDevice__set_last_error,self);
            service_fault = OnvifMediaService__get_fault(self->media_service);
            if(service_fault != SOAP_FAULT_NONE){
                ret_fault = service_fault;
                OnvifMediaService__destroy(self->media_service);
                self->media_service = NULL;
            }
            break;
        case SOAP_FAULT_ACTION_NOT_SUPPORTED:
        case SOAP_FAULT_CONNECTION_ERROR:
        case SOAP_FAULT_NOT_VALID:
        case SOAP_FAULT_UNAUTHORIZED:
        case SOAP_FAULT_UNEXPECTED:
        default:
            ONVIF_DEVICE_ERROR("[%s] Failed to retrieve capabilities capabilities...\n", self);
            ret_fault = *caps_fault;
            break;
    }

    g_object_unref(capabilities);

exit:
    P_MUTEX_UNLOCK(self->media_lock);
    return ret_fault;
}

time_t * OnvifDevice__getSystemDateTime(OnvifDevice * self){
    P_MUTEX_LOCK(self->prop_lock);
    return self->datetime;
    P_MUTEX_UNLOCK(self->prop_lock);
}

double OnvifDevice__getTimeOffset(OnvifDevice * self){
    return self->time_offset;
}

SoapFault OnvifDevice__authenticate(OnvifDevice* self){
    ONVIF_DEVICE_INFO("[%s] OnvifDevice__authenticate",self);
    SoapFault ret_fault;

    P_MUTEX_LOCK(self->auth_lock);
    if(self->authenticated){
        ret_fault = SOAP_FAULT_NONE;
        goto exit;
    }

    if(!self->datetime){
        OnvifDateTime * onviftime = OnvifDeviceService__getSystemDateAndTime(self->device_service);
        time_t datetime;
        ret_fault = *SoapObject__get_fault(SOAP_OBJECT(onviftime));
        switch(ret_fault){
            case SOAP_FAULT_NONE:
                datetime = *OnvifDateTime__get_datetime(onviftime);
                if(datetime > 0){
                    P_MUTEX_LOCK(self->prop_lock);
                    self->time_offset = difftime(datetime,time(NULL));
                    self->datetime = malloc(sizeof(time_t));
                    memcpy(self->datetime, &datetime, sizeof(datetime));

                    char str_now[20];
                    strftime(str_now, 20, "%Y-%m-%d %H:%M:%S", localtime(self->datetime));
                    ONVIF_DEVICE_INFO("[%s] Camera SystemDateAndTime : '%s'\n",self, str_now);

                    P_MUTEX_UNLOCK(self->prop_lock);
                } else {
                    ONVIF_DEVICE_ERROR("[%s] Camera SystemDateAndTime not set\n",self);
                }
                g_object_unref(onviftime);
                break;
            case SOAP_FAULT_ACTION_NOT_SUPPORTED:
            case SOAP_FAULT_CONNECTION_ERROR:
            case SOAP_FAULT_NOT_VALID:
            case SOAP_FAULT_UNAUTHORIZED:
            case SOAP_FAULT_UNEXPECTED:
            default:
                ONVIF_DEVICE_ERROR("[%s] OnvifDevice__authenticate - Failed to retrieve SystemDateTime [%d]\n",self, ret_fault);
                g_object_unref(onviftime);
                goto exit;
        }
    }

    ret_fault = OnvifDevice__createMediaService(self);
    switch(ret_fault){
        case SOAP_FAULT_NONE:
            ONVIF_MEDIA_DEBUG("[%s] Successfully created Media soap",self);
            break;
        case SOAP_FAULT_ACTION_NOT_SUPPORTED:
        case SOAP_FAULT_CONNECTION_ERROR:
        case SOAP_FAULT_NOT_VALID:
        case SOAP_FAULT_UNAUTHORIZED:
        case SOAP_FAULT_UNEXPECTED:
        default:
            ONVIF_DEVICE_ERROR("[%s] OnvifDevice__authenticate - Failed to create media service [%d]\n",self, ret_fault);
            goto exit;
    }

    char * uri = NULL;
    OnvifMediaUri * media_uri = OnvifMediaService__getStreamUri(self->media_service,0);
    ret_fault = *SoapObject__get_fault(SOAP_OBJECT(media_uri));
    switch(ret_fault){
        case SOAP_FAULT_NONE:
            uri = OnvifMediaUri__get_uri(media_uri);
            if(!uri){
                ONVIF_MEDIA_ERROR("[%s] No StreamURI provided...",self);
                OnvifMediaService__destroy(self->media_service);
                self->media_service = NULL;
            } else {
                ONVIF_MEDIA_ERROR("[%s] StreamURI : %s",self, uri);
                P_MUTEX_LOCK(self->prop_lock);
                self->authenticated = 1;
                P_MUTEX_UNLOCK(self->prop_lock);
            }
            break;
        case SOAP_FAULT_ACTION_NOT_SUPPORTED:
        case SOAP_FAULT_CONNECTION_ERROR:
        case SOAP_FAULT_NOT_VALID:
        case SOAP_FAULT_UNAUTHORIZED:
        case SOAP_FAULT_UNEXPECTED:
        default:
            ONVIF_MEDIA_ERROR("[%s] Failed to retrieve StreamURI...",self);
            OnvifMediaService__destroy(self->media_service);
            self->media_service = NULL;
            break;
    }
    
    g_object_unref(media_uri);

exit:
    P_MUTEX_UNLOCK(self->auth_lock);
    return ret_fault;
}

int OnvifDevice__is_authenticated(OnvifDevice* self){
    int ret;
    P_MUTEX_LOCK(self->auth_lock);
    ret = self->authenticated;
    P_MUTEX_UNLOCK(self->auth_lock);
    return ret;
}

void OnvifDevice__set_credentials(OnvifDevice* self,const char * user,const char* pass){
    P_MUTEX_LOCK(self->prop_lock);
    OnvifCredentials__set_username(self->credentials,user);
    OnvifCredentials__set_password(self->credentials,pass);
    P_MUTEX_UNLOCK(self->prop_lock);
}

OnvifCredentials * OnvifDevice__get_credentials(OnvifDevice * self){
    return self->credentials;
}

int OnvifDevice__is_valid(OnvifDevice* self){
    //According to Core specification, the device entry point it set. (5.1.1)
    if(!ParsedURL__is_valid(self->purl) || !self->purl->service || strcmp(self->purl->service,"onvif/device_service")){
        return 0;
    }

    //TODO Test endpoint
    return 1;
}

OnvifDeviceService * OnvifDevice__get_device_service(OnvifDevice* self){
    return self->device_service;
}

OnvifMediaService * OnvifDevice__get_media_service(OnvifDevice* self){
    return self->media_service;
}

void OnvifDevice__init(OnvifDevice* self, char * device_url) {
    P_MUTEX_SETUP(self->prop_lock);
    P_MUTEX_SETUP(self->auth_lock);
    P_MUTEX_SETUP(self->media_lock);

    self->authenticated = 0;
    self->time_offset = 0;
    self->last_error = ONVIF_ERROR_NOT_SET;
    self->credentials = OnvifCredentials__create();
    self->device_service = OnvifDeviceService__create(self, device_url,OnvifDevice__set_last_error,self);
    self->media_service = NULL;
    self->purl = ParsedURL__create(device_url);
    self->datetime = NULL;

    C_INFO("Created Device:\n");
    C_INFO("\tprotocol -- %s\n",self->purl->protocol);
    C_INFO("\thost : %s\n",self->purl->hostorip);
    C_INFO("\tport -- %s\n",self->purl->port);
    C_INFO("\tendpoint : %s\n",self->purl->service);
}

OnvifDevice * OnvifDevice__create(char * device_url) {
    C_DEBUG("OnvifDevice__create\n");
    OnvifDevice * result = malloc(sizeof(OnvifDevice));
    OnvifDevice__init(result,device_url);
    return result;
}

void OnvifDevice__destroy(OnvifDevice* device) {
    if (device) {
        ONVIF_DEVICE_DEBUG("[%s] OnvifDevice__destroy",device);

        OnvifCredentials__destroy(device->credentials);
        OnvifDeviceService__destroy(device->device_service);
        OnvifMediaService__destroy(device->media_service);
        P_MUTEX_CLEANUP(device->media_lock);
        P_MUTEX_CLEANUP(device->prop_lock);
        P_MUTEX_CLEANUP(device->auth_lock);
        ParsedURL__destroy(device->purl);
        free(device->datetime);
        free(device);
    }
}

char * OnvifDevice__get_host(OnvifDevice* self){
    char * ret = NULL;
    P_MUTEX_LOCK(self->prop_lock);
    if(self->purl->hostorip){
        ret = malloc(strlen(self->purl->hostorip)+1);
        strcpy(ret,self->purl->hostorip);
    }
    P_MUTEX_UNLOCK(self->prop_lock);
    return ret;
}

char * OnvifDevice__get_port(OnvifDevice* self){
    char * ret = NULL;
    P_MUTEX_LOCK(self->prop_lock);
    if(self->purl->port){
        ret = malloc(strlen(self->purl->port)+1);
        strcpy(ret,self->purl->port);
    }
    P_MUTEX_UNLOCK(self->prop_lock);
    return ret;
}

void OnvifDevice__set_last_error(OnvifErrorTypes error, void * error_data){
    OnvifDevice * self = (OnvifDevice *) error_data;
    P_MUTEX_LOCK(self->prop_lock);
    self->last_error = error;
    P_MUTEX_UNLOCK(self->prop_lock);
}

OnvifErrorTypes OnvifDevice__get_last_error(OnvifDevice * self){
    OnvifErrorTypes ret;
    P_MUTEX_LOCK(self->prop_lock);
    ret = self->last_error;
    P_MUTEX_UNLOCK(self->prop_lock);
    return ret;
}