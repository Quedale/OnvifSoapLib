#include "onvif_media1_audio_output_configs_local.h"
#include "../onvif_media_audio_output_configs_local.h"
#include "../onvif_media_audio_output_config_local.h"

G_DEFINE_TYPE (OnvifMedia1AudioOutputConfigs, OnvifMedia1AudioOutputConfigs_, ONVIF_TYPE_MEDIA_AUDIO_OUTPUT_CONFIGS)

static void
OnvifMedia1AudioOutputConfigs__construct(SoapObject * obj, gpointer ptr){
    OnvifMedia1AudioOutputConfigs * self = ONVIF_MEDIA1_AUDIO_OUTPUT_CONFIGS(obj);
    struct _trt__GetAudioOutputConfigurationsResponse * resp = ptr;

    if(!resp){
        return;
    }

    for(int i = 0; i < resp->__sizeConfigurations; i++){
        struct tt__AudioOutputConfiguration config = resp->Configurations[i];
        OnvifMediaAudioOutputConfigs__insert(ONVIF_MEDIA_AUDIO_OUTPUT_CONFIGS(self),OnvifMediaAudioOutputConfig__new(&config));
    }
}

static void
OnvifMedia1AudioOutputConfigs__class_init (OnvifMedia1AudioOutputConfigsClass * klass){
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMedia1AudioOutputConfigs__construct;
}

static void
OnvifMedia1AudioOutputConfigs__init (OnvifMedia1AudioOutputConfigs * self){

}

OnvifMedia1AudioOutputConfigs* 
OnvifMedia1AudioOutputConfigs__new(struct _trt__GetAudioOutputConfigurationsResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA1_AUDIO_OUTPUT_CONFIGS, "data", resp, NULL);
}
