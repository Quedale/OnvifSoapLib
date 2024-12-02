#include "onvif_media2_audio_source_configs_local.h"
#include "../onvif_media_audio_source_configs_local.h"
#include "../onvif_media_audio_source_config_local.h"

G_DEFINE_TYPE (OnvifMedia2AudioSourceConfigs, OnvifMedia2AudioSourceConfigs_, ONVIF_TYPE_MEDIA_AUDIO_SOURCE_CONFIGS)

static void
OnvifMedia2AudioSourceConfigs__construct(SoapObject * obj, gpointer ptr){
    OnvifMedia2AudioSourceConfigs * self = ONVIF_MEDIA2_AUDIO_SOURCE_CONFIGS(obj);
    struct _ns1__GetAudioSourceConfigurationsResponse * resp = ptr;

    if(!resp){
        return;
    }

    for(int i = 0; i < resp->__sizeConfigurations; i++){
        struct tt__AudioSourceConfiguration config = resp->Configurations[i];
        OnvifMediaAudioSourceConfigs__insert(ONVIF_MEDIA_AUDIO_SOURCE_CONFIGS(self),OnvifMediaAudioSourceConfig__new(&config));
    }
}

static void
OnvifMedia2AudioSourceConfigs__class_init (OnvifMedia2AudioSourceConfigsClass * klass){
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMedia2AudioSourceConfigs__construct;
}

static void
OnvifMedia2AudioSourceConfigs__init (OnvifMedia2AudioSourceConfigs * self){

}

OnvifMedia2AudioSourceConfigs* 
OnvifMedia2AudioSourceConfigs__new(struct _ns1__GetAudioSourceConfigurationsResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA2_AUDIO_SOURCE_CONFIGS, "data", resp, NULL);
}
