#ifndef ONVIF_MEDIA2_PROFILE_H_ 
#define ONVIF_MEDIA2_PROFILE_H_

#include "../onvif_media_profile.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA2_PROFILE OnvifMedia2Profile__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia2Profile, OnvifMedia2Profile_, ONVIF, MEDIA2_PROFILE, OnvifMediaProfile)

struct _OnvifMedia2Profile {
    OnvifMediaProfile parent_instance;
};


struct _OnvifMedia2ProfileClass {
    OnvifMediaProfileClass parent_class;
};

G_END_DECLS

#endif
