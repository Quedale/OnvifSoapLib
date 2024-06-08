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

void OnvifDevice__createMediaService(OnvifDevice* self){
    if(self->media_service){
        return;
    }

    P_MUTEX_LOCK(self->media_lock);

    //Check again in case it was created during lock
    if(self->media_service){
        goto exit;
    }

    ONVIF_DEVICE_TRACE("[%s] OnvifDevice__createMediaService",self);

    OnvifCapabilities* capabilities = OnvifDeviceService__getCapabilities(self->device_service);
    if(capabilities){
        OnvifMedia * media = OnvifCapabilities__get_media(capabilities);
        self->media_service = OnvifMediaService__create(self, OnvifMedia__get_address(media),OnvifDevice__set_last_error,self);
        OnvifCapabilities__destroy(capabilities);
    } else {
        C_WARN("No capabilities...\n");
    }

exit:
    P_MUTEX_UNLOCK(self->media_lock);
}

time_t * OnvifDevice__getSystemDateTime(OnvifDevice * self){
    P_MUTEX_LOCK(self->prop_lock);
    return self->datetime;
    P_MUTEX_UNLOCK(self->prop_lock);
}

double OnvifDevice__getTimeOffset(OnvifDevice * self){
    return self->time_offset;
}

void OnvifDevice__authenticate(OnvifDevice* self){
    ONVIF_DEVICE_INFO("[%s] OnvifDevice__authenticate",self);

    P_MUTEX_LOCK(self->auth_lock);
    if(self->authenticated){
        goto exit;
    }

    if(!self->datetime){
        time_t t = OnvifDeviceService__getSystemDateAndTime(self->device_service);
        if(t > 0){
            P_MUTEX_LOCK(self->prop_lock);
            self->time_offset = difftime(t,time(NULL));
            self->datetime = malloc(sizeof(time_t));
            memcpy(self->datetime, &t, sizeof(t));
            P_MUTEX_UNLOCK(self->prop_lock);
        }
    }

    if(self->datetime){
        char str_now[20];
        strftime(str_now, 20, "%Y-%m-%d %H:%M:%S", localtime(self->datetime));
        ONVIF_DEVICE_INFO("[%s] Camera SystemDateAndTime : '%s'\n",self, str_now);
    }

    OnvifDevice__createMediaService(self);
    if(!self->media_service){
        ONVIF_DEVICE_ERROR("[%s] OnvifDevice__authenticate - Failed to create media service\n",self);
        goto exit;
    }

    ONVIF_MEDIA_DEBUG("[%s] Successfully created Media soap",self);

    char * stream_uri = OnvifMediaService__getStreamUri(self->media_service,0);
    if(!stream_uri){
        ONVIF_MEDIA_ERROR("[%s] No stream uri returned...",self);
        OnvifMediaService__destroy(self->media_service);
        self->media_service = NULL;
    } else {
        ONVIF_MEDIA_ERROR("[%s] StreamURI : %s",self, stream_uri);
        P_MUTEX_LOCK(self->prop_lock);
        self->authenticated = 1;
        P_MUTEX_UNLOCK(self->prop_lock);
    }

    free(stream_uri);

exit:
    P_MUTEX_UNLOCK(self->auth_lock);
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