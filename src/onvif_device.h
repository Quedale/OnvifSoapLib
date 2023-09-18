#ifndef ONVIF_DEVICE_H_ 
#define ONVIF_DEVICE_H_

#include "client.h"
#include "shard_export.h"
#include "onvif_device_profile.h"
#include "onvif_device_info.h"
#include "onvif_device_interface.h"

typedef enum {
    ONVIF_NOT_SET = -1,
    ONVIF_ERROR_NONE = 0,
    ONVIF_CONNECTION_ERROR = 1,
    ONVIF_SOAP_ERROR = 2,
    ONVIF_NOT_AUTHORIZED = 3
} OnvifErrorTypes;

typedef struct _OnvifScope OnvifScope;
typedef struct _OnvifScopes OnvifScopes;
typedef struct _OnvifSnapshot OnvifSnapshot;
typedef struct _OnvifMedia OnvifMedia;
typedef struct _OnvifCapabilities OnvifCapabilities;
typedef struct _OnvifDevice OnvifDevice;


SHARD_EXPORT OnvifDevice * OnvifDevice__create(const char * device_url);
SHARD_EXPORT void OnvifDevice__destroy(OnvifDevice* device); 
SHARD_EXPORT char * OnvifDevice__get_ip(OnvifDevice* self);
SHARD_EXPORT char * OnvifDevice__get_port(OnvifDevice* self);
SHARD_EXPORT OnvifProfiles * OnvifDevice__get_profiles(OnvifDevice * self);
SHARD_EXPORT OnvifErrorTypes OnvifDevice__get_last_error(OnvifDevice * self);
SHARD_EXPORT char * OnvifDevice__get_device_endpoint(OnvifDevice * self);
SHARD_EXPORT char * OnvifDevice__get_media_endpoint(OnvifDevice * self);

SHARD_EXPORT void OnvifDevice_authenticate(OnvifDevice* self);
SHARD_EXPORT OnvifScopes * OnvifDevice__device_getScopes(OnvifDevice* self);
SHARD_EXPORT char * OnvifScopes__extract_scope(OnvifScopes * scopes, char * key);
SHARD_EXPORT void OnvifScopes__destroy(OnvifScopes * scopes);
SHARD_EXPORT char * OnvifDevice__device_getHostname(OnvifDevice* self);  // equivalent to "point->x()"
SHARD_EXPORT OnvifCapabilities* OnvifDevice__device_getCapabilities(OnvifDevice* self);
SHARD_EXPORT OnvifDeviceInformation * OnvifDevice__device_getDeviceInformation(OnvifDevice *self);
SHARD_EXPORT void OnvifDevice__media_getProfiles(OnvifDevice* self);
SHARD_EXPORT OnvifInterfaces * OnvifDevice__device_getNetworkInterfaces(OnvifDevice* self);
SHARD_EXPORT char * OnvifDevice__media_getSnapshotUri(OnvifDevice *self, int profile_index);
SHARD_EXPORT OnvifSnapshot * OnvifDevice__media_getSnapshot(OnvifDevice *self, int profile_index);
SHARD_EXPORT char * OnvifDevice__media_getStreamUri(OnvifDevice* self, int profile_index);
SHARD_EXPORT void OnvifDevice_set_credentials(OnvifDevice* self,const char * user,const char* pass);
SHARD_EXPORT int OnvifDevice__is_valid(OnvifDevice* self);

SHARD_EXPORT double OnvifSnapshot__get_size(OnvifSnapshot * self);
SHARD_EXPORT char * OnvifSnapshot__get_buffer(OnvifSnapshot * self);

#endif