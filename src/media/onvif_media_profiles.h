#ifndef ONVIF_MEDIA_MEDIA_PROFILES_H_ 
#define ONVIF_MEDIA_MEDIA_PROFILES_H_

#include "../shard_export.h"
#include "../SoapObject.h"
#include "onvif_media_profile.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_PROFILES OnvifMediaProfiles__get_type()
G_DECLARE_DERIVABLE_TYPE (OnvifMediaProfiles, OnvifMediaProfiles_, ONVIF, MEDIA_PROFILES, SoapObject)

struct _OnvifMediaProfilesClass {
  SoapObjectClass parent_class;
};

SHARD_EXPORT int OnvifMediaProfiles__get_size(OnvifMediaProfiles* self);
SHARD_EXPORT OnvifMediaProfile * OnvifMediaProfiles__get_profile(OnvifMediaProfiles * self,int index);
void OnvifMediaProfiles__insert(OnvifMediaProfiles * self,OnvifMediaProfile * profile);

G_END_DECLS

#endif