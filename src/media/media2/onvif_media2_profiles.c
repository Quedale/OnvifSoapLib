#include "onvif_media2_profiles_local.h"

G_DEFINE_TYPE(OnvifMedia2Profiles, OnvifMedia2Profiles_, ONVIF_TYPE_MEDIA_PROFILES)

static void
OnvifMedia2Profiles__construct(SoapObject * obj, gpointer ptr){
    OnvifMedia2Profiles * self = ONVIF_MEDIA2_PROFILES(obj);
    struct _ns1__GetProfilesResponse * resp = ptr;

    if(!resp){
        return;
    }

    for(int i = 0; i < resp->__sizeProfiles; i++){
        OnvifMedia2Profile * profile = OnvifMedia2Profile__new(&(resp->Profiles[i]),i);
        OnvifMediaProfiles__insert(ONVIF_MEDIA_PROFILES(self),ONVIF_MEDIA_PROFILE(profile));
    }

    //struct ns1__ConfigurationSet*        Configurations 
}


static void
OnvifMedia2Profiles__class_init (OnvifMedia2ProfilesClass * klass){
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMedia2Profiles__construct;
}

static void
OnvifMedia2Profiles__init (OnvifMedia2Profiles * self){

}

OnvifMedia2Profiles* OnvifMedia2Profiles__new(struct _ns1__GetProfilesResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA2_PROFILES, "data", resp, NULL);
}