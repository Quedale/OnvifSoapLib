#ifndef ONVIF_DEV_PROFILE_H_ 
#define ONVIF_DEV_PROFILE_H_

#include "shard_export.h"

typedef struct _OnvifProfile OnvifProfile;
typedef struct _OnvifProfiles OnvifProfiles;

void OnvifProfile__destroy(OnvifProfile* self);
SHARD_EXPORT char * OnvifProfile__get_name(OnvifProfile* self);
SHARD_EXPORT char * OnvifProfile__get_token(OnvifProfile* self);

void OnvifProfiles__destroy(OnvifProfiles* self);
void OnvifProfiles__insert(OnvifProfiles * self,OnvifProfile * profile);
SHARD_EXPORT int OnvifProfiles__get_size(OnvifProfiles* self);
SHARD_EXPORT OnvifProfile * OnvifProfiles__get_profile(OnvifProfiles * self,int index);
SHARD_EXPORT OnvifProfiles * OnvifProfiles__copy(OnvifProfiles * self);

#endif