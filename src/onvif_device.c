#include "onvif_device.h"
#include "onvif_credentials.h"
#include "generated/soapH.h"
#include "generated/DeviceBinding.nsmap"
#include "wsseapi.h"
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <netdb.h>
#endif
#include "wsddapi.h"
#include "ip_match.h"
#include "onvif_device_service.h"
#include "onvif_media_service.h"

typedef struct _OnvifDevice {
    char * protocol;
    char * ip;
    char * endpoint;
    char * port;
    char * hostname;
    OnvifErrorTypes last_error;
    pthread_mutex_t  * prop_lock;
    
    OnvifDeviceService * device_service;
    pthread_mutex_t  * media_lock;
    OnvifMediaService * media_service;

    OnvifCredentials * credentials;
} OnvifDevice;

void OnvifDevice__set_last_error(OnvifErrorTypes error, void * error_data);

void OnvifDevice__createMediaService(OnvifDevice* self){
    if(self->media_service){
        return;
    }

    pthread_mutex_lock(self->media_lock);

    //Check again in case it was created during lock
    if(self->media_service){
        goto exit;
    }

    printf("OnvifDevice__createMediaService\n");
    OnvifCapabilities* capabilities = OnvifDeviceService__getCapabilities(self->device_service);
    if(capabilities){
        OnvifMedia * media = OnvifCapabilities__get_media(capabilities);
        self->media_service = OnvifMediaService__create(OnvifMedia__get_address(media),self->credentials,OnvifDevice__set_last_error,self);
        OnvifCapabilities__destroy(capabilities);
    } else {
        printf("No capabilities...\n");
    }

exit:
    pthread_mutex_unlock(self->media_lock);
}

// Place holder to allow client compile
void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{ printf("wsdd_event_Hello\n"); }

void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{ printf("wsdd_event_Bye\n"); }

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *ProbeMatches)
{
    printf("wsdd_event_Probe\n");
    return SOAP_WSDD_ADHOC;
}

void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *ProbeMatches)
{  printf("wsdd_event_ProbeMatches\n"); }

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
    printf("wsdd_event_Resolve\n");
    return SOAP_WSDD_ADHOC;
}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char * SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{ printf("wsdd_event_ResolveMatches\n"); }

void OnvifDevice_authenticate(OnvifDevice* self){
    char * endpoint = OnvifDeviceService__get_endpoint(self->device_service);
    printf("OnvifDevice_authenticate [%s]\n", endpoint);
    free(endpoint);

    OnvifDevice__createMediaService(self);
    if(!self->media_service){
        return;
    }

    endpoint = OnvifMediaService__get_endpoint(self->media_service);
    printf("Created Media soap [%s]\n",endpoint);
    free(endpoint);

    char * stream_uri = OnvifMediaService__getStreamUri(self->media_service,0);
    if(!stream_uri){
        return;
    }
    printf("StreamURI : %s\n",stream_uri);
}

void OnvifDevice_set_credentials(OnvifDevice* self,const char * user,const char* pass){
    pthread_mutex_lock(self->prop_lock);
    OnvifCredentials__set_username(self->credentials,user);
    OnvifCredentials__set_password(self->credentials,pass);
    pthread_mutex_unlock(self->prop_lock);
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
    self->prop_lock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(self->prop_lock, NULL);

    self->media_lock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(self->media_lock, NULL);

    self->last_error = ONVIF_NOT_SET;
    self->credentials = OnvifCredentials__create(NULL,NULL);
    self->device_service = OnvifDeviceService__create(device_url,self->credentials,OnvifDevice__set_last_error,self);
    self->media_service = NULL;

    char * data = malloc(strlen(device_url)+1);
    memcpy(data,device_url,strlen(device_url)+1);

    if(strstr(data,"://")){
        self->protocol = strtok_r ((char *)data, "://", &data);
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
            self->port = strtok_r ((char *)hostport, "/", &hostport);
        } else { //If no port is set
            hostorip = strtok_r (hostport, "/", &hostport);
        }
    } else {
        hostorip = NULL;
    }

    if(is_valid_ip(hostorip)){
        self->ip = hostorip;
        self->hostname = NULL;
    } else {
        self->ip = NULL;
        self->hostname = hostorip;
    }

    if(!self->port && self->protocol && !strcmp(self->protocol,"http")){
        self->port = "80";
    } else if(!self->port && self->protocol && !strcmp(self->protocol,"https")){
        self->port = "443";
    }

    printf("Created Device:\n");
    printf("\tprotocol -- %s\n",self->protocol);
    printf("\tip : %s\n",self->ip);
    printf("\thostname : %s\n",self->hostname);
    printf("\tport -- %s\n",self->port);
    printf("\tendpoint : %s\n",self->endpoint);
}

OnvifDevice * OnvifDevice__create(const char * device_url) {
    printf("OnvifDevice__create\n");
    OnvifDevice * result = malloc(sizeof(OnvifDevice));
    OnvifDevice__init(result,device_url);
    return result;
}

void OnvifDevice__destroy(OnvifDevice* device) {
    printf("OnvifDevice__destroy\n");
    if (device) {
        OnvifCredentials__destroy(device->credentials);
        OnvifDeviceService__destroy(device->device_service);
        OnvifMediaService__destroy(device->media_service);
        pthread_mutex_destroy(device->media_lock);
        pthread_mutex_destroy(device->prop_lock);
        free(device->media_lock);
        free(device->prop_lock);
        free(device);
    }
}

char * OnvifDevice__get_ip(OnvifDevice* self){
    char * ret;
    pthread_mutex_lock(self->prop_lock);
    ret = malloc(strlen(self->ip)+1);
    strcpy(ret,self->ip);
    pthread_mutex_unlock(self->prop_lock);
    return ret;
}

char * OnvifDevice__get_port(OnvifDevice* self){
    char * ret;
    pthread_mutex_lock(self->prop_lock);
    ret = malloc(strlen(self->port)+1);
    strcpy(ret,self->port);
    pthread_mutex_unlock(self->prop_lock);
    return ret;
}

void OnvifDevice__set_last_error(OnvifErrorTypes error, void * error_data){
    OnvifDevice * self = (OnvifDevice *) error_data;
    pthread_mutex_lock(self->prop_lock);
    self->last_error = error;
    pthread_mutex_unlock(self->prop_lock);
}

OnvifErrorTypes OnvifDevice__get_last_error(OnvifDevice * self){
    OnvifErrorTypes ret;
    pthread_mutex_lock(self->prop_lock);
    ret = self->last_error;
    pthread_mutex_unlock(self->prop_lock);
    return ret;
}