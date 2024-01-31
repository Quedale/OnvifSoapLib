#ifndef ONVIF_DEVICE_H_ 
#define ONVIF_DEVICE_H_

typedef struct _OnvifDevice OnvifDevice;

#include "shard_export.h"
#include "onvif_device_service.h"
#include "onvif_media_service.h"


SHARD_EXPORT OnvifDevice * OnvifDevice__create(char * device_url);
SHARD_EXPORT void OnvifDevice__destroy(OnvifDevice* device); 
SHARD_EXPORT char * OnvifDevice__get_host(OnvifDevice* self);
// SHARD_EXPORT void OnvifDevice__lookup_ip(OnvifDevice* self);
SHARD_EXPORT char * OnvifDevice__get_port(OnvifDevice* self);
SHARD_EXPORT OnvifErrorTypes OnvifDevice__get_last_error(OnvifDevice * self);
SHARD_EXPORT void OnvifDevice__authenticate(OnvifDevice* self);
SHARD_EXPORT void OnvifDevice__set_credentials(OnvifDevice* self,const char * user,const char* pass);
SHARD_EXPORT OnvifCredentials * OnvifDevice__get_credentials(OnvifDevice * self);
SHARD_EXPORT int OnvifDevice__is_valid(OnvifDevice* self);
SHARD_EXPORT OnvifDeviceService * OnvifDevice__get_device_service(OnvifDevice* self);
SHARD_EXPORT OnvifMediaService * OnvifDevice__get_media_service(OnvifDevice* self);
SHARD_EXPORT time_t * OnvifDevice__getSystemDateTime(OnvifDevice * self);
SHARD_EXPORT double OnvifDevice__getTimeOffset(OnvifDevice * self);

#endif