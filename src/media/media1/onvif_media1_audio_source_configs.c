#include "onvif_media1_audio_source_configs_local.h"
#include "../onvif_media_audio_source_configs_local.h"
#include "../onvif_media_audio_source_config_local.h"

G_DEFINE_TYPE (OnvifMedia1AudioSourceConfigs, OnvifMedia1AudioSourceConfigs_, ONVIF_TYPE_MEDIA_AUDIO_SOURCE_CONFIGS)

static void
OnvifMedia1AudioSourceConfigs__construct(SoapObject * obj, gpointer ptr){
    OnvifMedia1AudioSourceConfigs * self = ONVIF_MEDIA1_AUDIO_SOURCE_CONFIGS(obj);
    struct _trt__GetAudioSourceConfigurationsResponse * resp = ptr;

    if(!resp){
        return;
    }

    for(int i = 0; i < resp->__sizeConfigurations; i++){
        struct tt__AudioSourceConfiguration config = resp->Configurations[i];
        OnvifMediaAudioSourceConfigs__insert(ONVIF_MEDIA_AUDIO_SOURCE_CONFIGS(self),OnvifMediaAudioSourceConfig__new(&config));
    }
}

static void
OnvifMedia1AudioSourceConfigs__class_init (OnvifMedia1AudioSourceConfigsClass * klass){
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMedia1AudioSourceConfigs__construct;
}

static void
OnvifMedia1AudioSourceConfigs__init (OnvifMedia1AudioSourceConfigs * self){

}

OnvifMedia1AudioSourceConfigs* 
OnvifMedia1AudioSourceConfigs__new(struct _trt__GetAudioSourceConfigurationsResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA1_AUDIO_SOURCE_CONFIGS, "data", resp, NULL);
}
