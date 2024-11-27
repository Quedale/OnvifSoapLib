#ifndef ONVIF_MEDIA1_PROFILE_H_ 
#define ONVIF_MEDIA1_PROFILE_H_

#include "../onvif_media_profile.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA1_PROFILE OnvifMedia1Profile__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia1Profile, OnvifMedia1Profile_, ONVIF, MEDIA1_PROFILE, OnvifMediaProfile)

struct _OnvifMedia1Profile {
    OnvifMediaProfile parent_instance;
};


struct _OnvifMedia1ProfileClass {
    OnvifMediaProfileClass parent_class;
};

G_END_DECLS

#endif