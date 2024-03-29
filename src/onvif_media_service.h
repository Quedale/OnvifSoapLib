#ifndef ONVIF_MEDIA_SERVICE_H_ 
#define ONVIF_MEDIA_SERVICE_H_

typedef struct _OnvifMediaService OnvifMediaService;

#include "shard_export.h"
#include "onvif_base_service.h"
#include "onvif_credentials.h"
#include "onvif_media_profile.h"
#include "onvif_media_snapshot.h"
#include "onvif_device.h"

//Generic Service functions
SHARD_EXPORT OnvifMediaService * OnvifMediaService__create(OnvifDevice * device, const char * endpoint, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data);
SHARD_EXPORT void OnvifMediaService__init(OnvifMediaService * self, OnvifDevice * device, const char * endpoint, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data);
SHARD_EXPORT void OnvifMediaService__destroy(OnvifMediaService * self);
SHARD_EXPORT OnvifBaseService * OnvifMediaService__get_parent(OnvifMediaService * self);

//No wait functions
SHARD_EXPORT char * OnvifMediaService__get_endpoint(OnvifMediaService * self);
SHARD_EXPORT void OnvifMediaService__get_profile_token(OnvifMediaService *self, int index, char * ret);

//Lazy cache function
SHARD_EXPORT OnvifProfiles * OnvifMediaService__get_profiles(OnvifMediaService * self);

//Remote Sevice function
SHARD_EXPORT OnvifProfiles * OnvifMediaService__getProfiles(OnvifMediaService* self);
SHARD_EXPORT char * OnvifMediaService__getSnapshotUri(OnvifMediaService *self, int profile_index);
SHARD_EXPORT OnvifSnapshot * OnvifMediaService__getSnapshot(OnvifMediaService *self, int profile_index);
SHARD_EXPORT char * OnvifMediaService__getStreamUri(OnvifMediaService* self, int profile_index);

#endif