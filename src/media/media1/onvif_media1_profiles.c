#include "onvif_media1_profiles_local.h"
#include "onvif_media1_profile_local.h"


G_DEFINE_TYPE(OnvifMedia1Profiles, OnvifMedia1Profiles_, ONVIF_TYPE_MEDIA_PROFILES)

static void
OnvifMedia1Profiles__construct(SoapObject * obj, gpointer ptr){
    OnvifMedia1Profiles * self = ONVIF_MEDIA1_PROFILES(obj);
    struct _trt__GetProfilesResponse * resp = ptr;

    if(!resp){
        return;
    }

    for(int i = 0; i < resp->__sizeProfiles; i++){
        OnvifMedia1Profile * profile = OnvifMedia1Profile__new(&(resp->Profiles[i]),i);
        OnvifMediaProfiles__insert(ONVIF_MEDIA_PROFILES(self),ONVIF_MEDIA_PROFILE(profile));
    }
}

static void
OnvifMedia1Profiles__class_init (OnvifMedia1ProfilesClass * klass){
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMedia1Profiles__construct;
}

static void
OnvifMedia1Profiles__init (OnvifMedia1Profiles * self) {

}

OnvifMedia1Profiles* 
OnvifMedia1Profiles__new(struct _trt__GetProfilesResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA1_PROFILES, "data", resp, NULL);
}
