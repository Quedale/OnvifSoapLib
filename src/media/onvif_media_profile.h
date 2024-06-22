#ifndef ONVIF_DEV_PROFILE_H_ 
#define ONVIF_DEV_PROFILE_H_

#include "shard_export.h"
#include "SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_PROFILES OnvifMediaProfiles__get_type()
G_DECLARE_FINAL_TYPE (OnvifMediaProfiles, OnvifMediaProfiles_, ONVIF, MEDIA_PROFILES, SoapObject)

typedef struct _OnvifProfile OnvifProfile;

struct _OnvifMediaProfiles
{
    SoapObject parent_instance;
};


struct _OnvifMediaProfilesClass
{
    SoapObjectClass parent_class;
};

SHARD_EXPORT int OnvifMediaProfiles__get_size(OnvifMediaProfiles* self);
SHARD_EXPORT OnvifProfile * OnvifMediaProfiles__get_profile(OnvifMediaProfiles * self,int index);

SHARD_EXPORT char * OnvifProfile__get_name(OnvifProfile* self);
SHARD_EXPORT char * OnvifProfile__get_token(OnvifProfile* self);
SHARD_EXPORT int OnvifProfile__get_index(OnvifProfile* self);
SHARD_EXPORT void OnvifProfile__destroy(OnvifProfile* self);
SHARD_EXPORT OnvifProfile * OnvifProfile__copy(OnvifProfile * self);
SHARD_EXPORT int OnvifProfile__equals(OnvifProfile * prof1, OnvifProfile * prof2);

G_END_DECLS

#endif