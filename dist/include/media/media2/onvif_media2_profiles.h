#ifndef ONVIF_MEDIA2_PROFILES_H_ 
#define ONVIF_MEDIA2_PROFILES_H_

#include "../onvif_media_profiles.h" //TODFO REmove this for OnvifMediaProfiles
#include "onvif_media2_profile.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA2_PROFILES OnvifMedia2Profiles__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia2Profiles, OnvifMedia2Profiles_, ONVIF, MEDIA2_PROFILES, OnvifMediaProfiles)

typedef struct _OnvifMedia2Profile OnvifMedia2Profile;

struct _OnvifMedia2Profiles {
    OnvifMediaProfiles parent_instance;
};


struct _OnvifMedia2ProfilesClass {
    OnvifMediaProfilesClass parent_class;
};

G_END_DECLS

#endif
