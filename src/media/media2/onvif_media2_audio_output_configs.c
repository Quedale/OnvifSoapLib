#include "onvif_media2_audio_output_configs_local.h"
#include "../onvif_media_audio_output_configs_local.h"
#include "../onvif_media_audio_output_config_local.h"

G_DEFINE_TYPE (OnvifMedia2AudioOutputConfigs, OnvifMedia2AudioOutputConfigs_, ONVIF_TYPE_MEDIA_AUDIO_OUTPUT_CONFIGS)

static void
OnvifMedia2AudioOutputConfigs__construct(SoapObject * obj, gpointer ptr){
    OnvifMedia2AudioOutputConfigs * self = ONVIF_MEDIA2_AUDIO_OUTPUT_CONFIGS(obj);
    struct _ns1__GetAudioOutputConfigurationsResponse * resp = ptr;

    if(!resp){
        return;
    }

    for(int i = 0; i < resp->__sizeConfigurations; i++){
        struct tt__AudioOutputConfiguration config = resp->Configurations[i];
        OnvifMediaAudioOutputConfigs__insert(ONVIF_MEDIA_AUDIO_OUTPUT_CONFIGS(self),OnvifMediaAudioOutputConfig__new(&config));
    }
}

static void
OnvifMedia2AudioOutputConfigs__class_init (OnvifMedia2AudioOutputConfigsClass * klass){
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMedia2AudioOutputConfigs__construct;
}

static void
OnvifMedia2AudioOutputConfigs__init (OnvifMedia2AudioOutputConfigs * self){

}

OnvifMedia2AudioOutputConfigs* 
OnvifMedia2AudioOutputConfigs__new(struct _ns1__GetAudioOutputConfigurationsResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA2_AUDIO_OUTPUT_CONFIGS, "data", resp, NULL);
}
