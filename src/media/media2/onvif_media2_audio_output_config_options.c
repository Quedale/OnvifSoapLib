#include "onvif_media2_audio_output_config_options_local.h"
#include "../onvif_media_audio_output_config_options_local.h"

G_DEFINE_TYPE (OnvifMedia2AudioOutputConfigOptions, OnvifMedia2AudioOutputConfigOptions_, ONVIF_TYPE_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS)

static void
OnvifMedia2AudioOutputConfigOptions__construct(SoapObject * obj, gpointer ptr){
    OnvifMedia2AudioOutputConfigOptions * self = ONVIF_MEDIA2_AUDIO_OUTPUT_CONFIG_OPTIONS(obj);
    struct _ns1__GetAudioOutputConfigurationOptionsResponse * resp = ptr;

    if(!resp){
        return;
    }

    OnvifMediaAudioOutputConfigOptions__set_option(ONVIF_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS(self),resp->Options);
}

static void
OnvifMedia2AudioOutputConfigOptions__class_init (OnvifMedia2AudioOutputConfigOptionsClass * klass){
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMedia2AudioOutputConfigOptions__construct;
}

static void
OnvifMedia2AudioOutputConfigOptions__init (OnvifMedia2AudioOutputConfigOptions * self){

}

OnvifMedia2AudioOutputConfigOptions* 
OnvifMedia2AudioOutputConfigOptions__new(struct _ns1__GetAudioOutputConfigurationOptionsResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA2_AUDIO_OUTPUT_CONFIG_OPTIONS, "data", resp, NULL);
}
