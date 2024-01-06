#include "onvif_device.h"
#include "onvif_credentials.h"
#include "onvif_device_service.h"
#include "onvif_media_service.h"
#include "wsddapi.h"
#include "ip_match.h"
#include "clogger.h"

#if defined(_WIN32) || defined(_WIN64)
/* We are on Windows */
# define strtok_r strtok_s
#endif

typedef struct _OnvifDevice {
    char * protocol;
    char * ip;
    char * endpoint;
    char * port;
    char * hostname;
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

    C_DEBUG("OnvifDevice__createMediaService\n");
    OnvifCapabilities* capabilities = OnvifDeviceService__getCapabilities(self->device_service);
    if(capabilities){
        OnvifMedia * media = OnvifCapabilities__get_media(capabilities);
        self->media_service = OnvifMediaService__create(OnvifMedia__get_address(media),self->credentials,OnvifDevice__set_last_error,self);
        OnvifCapabilities__destroy(capabilities);
    } else {
        C_WARN("No capabilities...\n");
    }

exit:
    P_MUTEX_UNLOCK(self->media_lock);
}

void OnvifDevice__authenticate(OnvifDevice* self){
    char * endpoint = OnvifDeviceService__get_endpoint(self->device_service);
    C_INFO("OnvifDevice__authenticate [%s]\n", endpoint);
    free(endpoint);

    OnvifDevice__createMediaService(self);
    if(!self->media_service){
        return;
    }

    endpoint = OnvifMediaService__get_endpoint(self->media_service);
    C_DEBUG("Created Media soap [%s]\n",endpoint);
    free(endpoint);

    char * stream_uri = OnvifMediaService__getStreamUri(self->media_service,0);
    if(!stream_uri){
        return;
    }
    C_DEBUG("StreamURI : %s\n",stream_uri);
    free(stream_uri);
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
    if(!self->protocol){
        return 0;
    }
    if(!self->hostname && !self->ip){
        return 0;
    }
    //According to Core specification, the device entry point it set. (5.1.1)
    if(!self->endpoint || strcmp(self->endpoint,"onvif/device_service")){
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

void OnvifDevice__init(OnvifDevice* self, const char * device_url) {
    P_MUTEX_SETUP(self->prop_lock);

    P_MUTEX_SETUP(self->media_lock);

    self->last_error = ONVIF_NOT_SET;
    self->credentials = OnvifCredentials__create();
    self->device_service = OnvifDeviceService__create(device_url,self->credentials,OnvifDevice__set_last_error,self);
    self->media_service = NULL;

    char data_arr[strlen(device_url)+1];
    char * data = data_arr;
    memcpy(data,device_url,strlen(device_url)+1);

    if(strstr(data,"://")){
        char * tmpprot = strtok_r ((char *)data, "://", &data);
        self->protocol = malloc(strlen(tmpprot)+1);
        strcpy(self->protocol,tmpprot);
    } else {
        self->protocol = NULL;
    }

    self->port = NULL;

    char * hostorip = NULL;
    char *hostport = strtok_r ((char *)data, "/", &data);
    self->endpoint = strtok_r ((char *)data, "\n", &data);

    //TODO Support IPv6
    if(hostport){
        if(strstr(hostport,":")){ //If the port is set
            hostorip = strtok_r (hostport, ":", &hostport);
            char * tmpport = strtok_r ((char *)hostport, "/", &hostport);
            self->port = malloc(strlen(tmpport)+1);
            strcpy(self->port,tmpport);
        } else { //If no port is set
            hostorip = strtok_r (hostport, "/", &hostport);
        }
    } else {
        hostorip = NULL;
    }

    if(is_valid_ip(hostorip)){
        self->ip = malloc(strlen(hostorip) +1);
        strcpy(self->ip,hostorip);
        self->hostname = NULL;
    } else {
        self->ip = NULL;
        self->hostname = malloc(strlen(hostorip) +1);
        strcpy(self->hostname,hostorip);

        // To retrieve host information
        struct hostent *host_entry = gethostbyname(self->hostname);
        if(host_entry){
            C_DEBUG("Resolving hostname...");
            char * tmpip = inet_ntoa(*((struct in_addr*)
                        host_entry->h_addr_list[0]));
            self->ip = malloc(strlen(tmpip)+1);
            strcpy(self->ip,tmpip);
        }
    }

    if(!self->port && self->protocol && !strcmp(self->protocol,"http")){
        self->port = malloc(strlen("80")+1);
        strcpy(self->port,"80");
    } else if(!self->port && self->protocol && !strcmp(self->protocol,"https")){
        self->port = malloc(strlen("443")+1);
        strcpy(self->port,"443");
    }

    C_INFO("Created Device:\n");
    C_INFO("\tprotocol -- %s\n",self->protocol);
    C_INFO("\tip : %s\n",self->ip);
    C_INFO("\thostname : %s\n",self->hostname);
    C_INFO("\tport -- %s\n",self->port);
    C_INFO("\tendpoint : %s\n",self->endpoint);
}

OnvifDevice * OnvifDevice__create(const char * device_url) {
    C_DEBUG("OnvifDevice__create\n");
    OnvifDevice * result = malloc(sizeof(OnvifDevice));
    OnvifDevice__init(result,device_url);
    return result;
}

void OnvifDevice__destroy(OnvifDevice* device) {
    C_DEBUG("OnvifDevice__destroy\n");
    if (device) {
        OnvifCredentials__destroy(device->credentials);
        OnvifDeviceService__destroy(device->device_service);
        OnvifMediaService__destroy(device->media_service);
        P_MUTEX_CLEANUP(device->media_lock);
        P_MUTEX_CLEANUP(device->prop_lock);
        free(device->hostname);
        free(device->ip);
        free(device->protocol);
        free(device->port);
        free(device);
    }
}

char * OnvifDevice__get_ip(OnvifDevice* self){
    char * ret;
    P_MUTEX_LOCK(self->prop_lock);
    if(self->ip != NULL){
        ret = malloc(strlen(self->ip)+1);
        strcpy(ret,self->ip);
    } else {
        //Something went wrong here?
        ret = malloc(strlen("N/A")+1);
        strcpy(ret,"N/A");
    }
    P_MUTEX_UNLOCK(self->prop_lock);
    return ret;
}

char * OnvifDevice__get_port(OnvifDevice* self){
    char * ret;
    P_MUTEX_LOCK(self->prop_lock);
    ret = malloc(strlen(self->port)+1);
    strcpy(ret,self->port);
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