#include "onvif_media1_audio_output_config_options_local.h"
#include "../onvif_media_audio_output_config_options_local.h"

G_DEFINE_TYPE (OnvifMedia1AudioOutputConfigOptions, OnvifMedia1AudioOutputConfigOptions_, ONVIF_TYPE_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS)

static void
OnvifMedia1AudioOutputConfigOptions__construct(SoapObject * obj, gpointer ptr){
    OnvifMedia1AudioOutputConfigOptions * self = ONVIF_MEDIA1_AUDIO_OUTPUT_CONFIG_OPTIONS(obj);
    struct _trt__GetAudioOutputConfigurationOptionsResponse * resp = ptr;

    if(!resp){
        return;
    }

    OnvifMediaAudioOutputConfigOptions__set_option(ONVIF_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS(self),resp->Options);
}

static void
OnvifMedia1AudioOutputConfigOptions__class_init (OnvifMedia1AudioOutputConfigOptionsClass * klass){
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMedia1AudioOutputConfigOptions__construct;
}

static void
OnvifMedia1AudioOutputConfigOptions__init (OnvifMedia1AudioOutputConfigOptions * self){

}

OnvifMedia1AudioOutputConfigOptions* 
OnvifMedia1AudioOutputConfigOptions__new(struct _trt__GetAudioOutputConfigurationOptionsResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA1_AUDIO_OUTPUT_CONFIG_OPTIONS, "data", resp, NULL);
}
