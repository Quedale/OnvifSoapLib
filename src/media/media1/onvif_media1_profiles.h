#ifndef ONVIF_MEDIA1_PROFILES_H_ 
#define ONVIF_MEDIA1_PROFILES_H_

#include "../onvif_media_profiles.h"
#include "onvif_media1_profile.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA1_PROFILES OnvifMedia1Profiles__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia1Profiles, OnvifMedia1Profiles_, ONVIF, MEDIA1_PROFILES, OnvifMediaProfiles)

struct _OnvifMedia1Profiles {
    SoapObject parent_instance;
};


struct _OnvifMedia1ProfilesClass {
    SoapObjectClass parent_class;
};

G_END_DECLS

#endif