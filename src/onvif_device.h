#ifndef ONVIF_DEVICE_H_ 
#define ONVIF_DEVICE_H_

#include "client.h"


struct chunk {
    char * buffer;
    size_t size;
};

struct OnvifProfile {
    char * name;
    char * token;
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

struct OnvifHttp {
    struct soap *soap;
    void * info;
};

typedef struct {
    char * protocol;
    char * ip;
    char * port;
    char * hostname;
    int authorized;
    OnvifSoapClient* device_soap;
    OnvifSoapClient* media_soap;
    void * image_handle;
    void * priv_ptr;
    int sizeSrofiles;
    struct OnvifProfile * profiles;
    struct OnvifHttp * snapshot;
} OnvifDevice;


#ifdef __cplusplus
    #define SHARD_EXTERN extern "C"
#else
    #define SHARD_EXTERN extern
#endif

#ifdef _WIN32
    #ifdef BUILD_SHARD
        #define SHARD_EXPORT SHARD_EXTERN __declspec(dllexport)
    #else
        #define SHARD_EXPORT SHARD_EXTERN __declspec(dllimport)
    #endif
#else
    #ifdef BUILD_SHARD
        #define SHARD_EXPORT SHARD_EXTERN  __attribute__((visibility("default")))
    #else
        #define SHARD_EXPORT SHARD_EXTERN
    #endif
#endif



SHARD_EXPORT OnvifDevice * OnvifDevice__create(char * device_url);
SHARD_EXPORT void OnvifDevice__destroy(OnvifDevice* device); 
SHARD_EXPORT void OnvifDevice_authenticate(OnvifDevice* self);
SHARD_EXPORT char * OnvifDevice__device_getHostname(OnvifDevice* self);  // equivalent to "point->x()"
SHARD_EXPORT OnvifCapabilities* OnvifDevice__device_getCapabilities(OnvifDevice* self);
SHARD_EXPORT OnvifDeviceInformation * OnvifDevice__device_getDeviceInformation(OnvifDevice *self);
SHARD_EXPORT void OnvifDevice_get_profiles(OnvifDevice* self);
SHARD_EXPORT char * OnvifDevice__media_getSnapshotUri(OnvifDevice *self, int profile_index);
SHARD_EXPORT struct chunk * OnvifDevice__media_getSnapshot(OnvifDevice *self, int profile_index);
SHARD_EXPORT char * OnvifDevice__media_getStreamUri(OnvifDevice* self, int profile_index);
SHARD_EXPORT void OnvifDevice_set_credentials(OnvifDevice* self, char * user, char* pass);
#endif