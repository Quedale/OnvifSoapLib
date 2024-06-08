#ifndef ONVIF_DEV_PROFILE_H_ 
#define ONVIF_DEV_PROFILE_H_

#include "shard_export.h"

typedef struct _OnvifProfile OnvifProfile;
typedef struct _OnvifProfiles OnvifProfiles;

SHARD_EXPORT void OnvifProfile__destroy(OnvifProfile* self);
SHARD_EXPORT char * OnvifProfile__get_name(OnvifProfile* self);
SHARD_EXPORT char * OnvifProfile__get_token(OnvifProfile* self);
SHARD_EXPORT int OnvifProfile__get_index(OnvifProfile* self);

SHARD_EXPORT void OnvifProfiles__destroy(OnvifProfiles* self);
void OnvifProfiles__insert(OnvifProfiles * self,OnvifProfile * profile);
SHARD_EXPORT int OnvifProfiles__get_size(OnvifProfiles* self);
SHARD_EXPORT OnvifProfile * OnvifProfiles__get_profile(OnvifProfiles * self,int index);
SHARD_EXPORT OnvifProfiles * OnvifProfiles__copy(OnvifProfiles * self);
SHARD_EXPORT OnvifProfile * OnvifProfile__copy(OnvifProfile * self);
SHARD_EXPORT int OnvifProfile__equals(OnvifProfile * prof1, OnvifProfile * prof2);

#endif