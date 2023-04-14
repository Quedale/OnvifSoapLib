#ifndef ONVIF_DEVICE_H_ 
#define ONVIF_DEVICE_H_

#include "client.h"

typedef struct {
    char * token; ///< Required attribute.
    int enabled; ///< Required element.

    //Info ///< Optional element.
    int has_info;
    char * name;
    char * mac;
    int mtu;

    //IPv4 ///< Optional element.
    //__sizeManual
    int ipv4_enabled;
    int ipv4_dhcp; // DHCP
    int ipv4_manual_count;
    char ** ipv4_manual; // Manual
    char * ipv4_link_local; // LinkLocal
    char * ipv4_from_dhcp; // FromDHCP

    //TODO Support IPv6

} OnvifInterface;

typedef struct {
    int count;
    OnvifInterface ** interfaces;
} OnvifInterfaces;

typedef struct {
    char * scope;
    int editable;
} OnvifScope;

typedef struct {
    int count;
    OnvifScope ** scopes;
} OnvifScopes;

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

typedef struct {
    char * protocol;
    char * ip;
    char * port;
    char * hostname;
    int authorized;
    OnvifSoapClient* device_soap;
    pthread_mutex_t * device_lock;
    OnvifSoapClient* media_soap;
    pthread_mutex_t * media_lock;
    void * priv_ptr;
    int sizeSrofiles;
    struct OnvifProfile * profiles;
    struct http_da_info *  snapshot_da_info;
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


SHARD_EXPORT void OnvifInterfaces__destroy(OnvifInterfaces * self);
SHARD_EXPORT OnvifDevice * OnvifDevice__create(const char * device_url);
SHARD_EXPORT void OnvifDevice__destroy(OnvifDevice* device); 
SHARD_EXPORT void OnvifDevice_authenticate(OnvifDevice* self);
SHARD_EXPORT OnvifScopes * OnvifDevice__device_getScopes(OnvifDevice* self);
SHARD_EXPORT char * OnvifScopes__extract_scope(OnvifScopes * scopes, char * key);
SHARD_EXPORT void OnvifScopes__destroy(OnvifScopes * scopes);
SHARD_EXPORT char * OnvifDevice__device_getHostname(OnvifDevice* self);  // equivalent to "point->x()"
SHARD_EXPORT OnvifCapabilities* OnvifDevice__device_getCapabilities(OnvifDevice* self);
SHARD_EXPORT void OnvifDeviceInformation__destroy(OnvifDeviceInformation *self);
SHARD_EXPORT OnvifDeviceInformation * OnvifDevice__device_getDeviceInformation(OnvifDevice *self);
SHARD_EXPORT void OnvifDevice_get_profiles(OnvifDevice* self);
SHARD_EXPORT OnvifInterfaces * OnvifDevice__device_getNetworkInterfaces(OnvifDevice* self);
SHARD_EXPORT char * OnvifDevice__media_getSnapshotUri(OnvifDevice *self, int profile_index);
SHARD_EXPORT struct chunk * OnvifDevice__media_getSnapshot(OnvifDevice *self, int profile_index);
SHARD_EXPORT char * OnvifDevice__media_getStreamUri(OnvifDevice* self, int profile_index);
SHARD_EXPORT void OnvifDevice_set_credentials(OnvifDevice* self,const char * user,const char* pass);
#endif