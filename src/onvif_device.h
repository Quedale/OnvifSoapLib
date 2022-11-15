#ifndef ONVIF_DEVICE_H_ 
#define ONVIF_DEVICE_H_

#include "client.h"


struct chunk {
    char * buffer;
    size_t size;
};

typedef struct {
    char *xaddr;
    //TODO StreamingCapabilities
    //TODO Extension
} OnvifMedia;

typedef struct {
    OnvifMedia *media;
} OnvifCapabilities;

typedef struct {
    char * manufacturer;
    char * model;
    char * firmwareVersion;
    char * serialNumber;
    char * hardwareId;
} OnvifDeviceInformation;

struct _OnvifCred;
typedef struct _OnvifCred  *OnvifCred;

typedef struct {
    char * protocol;
    char * ip;
    char * port;
    char * hostname;
    int * authorized;
    OnvifSoapClient* device_soap;
    OnvifSoapClient* media_soap;
    OnvifCred* cred;
    // OnvifCapabilities * capabilities;
} OnvifDevice;



__attribute__ ((visibility("default"))) 
extern OnvifDevice OnvifDevice__create(char * device_url);
__attribute__ ((visibility("default")))  
extern void OnvifDevice__destroy(OnvifDevice* device); 
__attribute__ ((visibility("default")))  
extern void OnvifDevice_authenticate(OnvifDevice* self);
__attribute__ ((visibility("default"))) 
extern char * OnvifDevice__device_getHostname(OnvifDevice* self);  // equivalent to "point->x()"
__attribute__ ((visibility("default")))  
extern OnvifDevice * OnvifDevice__copy(OnvifDevice * dev);
__attribute__ ((visibility("default"))) 
extern OnvifCapabilities* OnvifDevice__device_getCapabilities(OnvifDevice* self);
__attribute__ ((visibility("default"))) 
extern OnvifDeviceInformation * OnvifDevice__device_getDeviceInformation(OnvifDevice *self);
__attribute__ ((visibility("default"))) 
extern char * OnvifDevice__media_getSnapshotUri(OnvifDevice *self);
__attribute__ ((visibility("default"))) 
extern struct chunk * OnvifDevice__media_getSnapshot(OnvifDevice *self);
__attribute__ ((visibility("default"))) 
extern char * OnvifDevice__media_getStreamUri(OnvifDevice* self);

#endif