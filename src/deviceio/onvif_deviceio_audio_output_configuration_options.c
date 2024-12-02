#include "onvif_deviceio_audio_output_configuration_options_local.h"
#include "../media/onvif_media_audio_output_config_options_local.h"
#include "clogger.h"

G_DEFINE_TYPE(OnvifDeviceIOAudioOutputConfigurationOptions, OnvifDeviceIOAudioOutputConfigurationOptions_, ONVIF_TYPE_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS)

static void
OnvifDeviceIOAudioOutputConfigurationOptions__construct(SoapObject * obj, gpointer ptr){
    struct _tmd__GetAudioOutputConfigurationOptionsResponse * response = ptr;
    OnvifDeviceIOAudioOutputConfigurationOptions * self = ONVIF_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS(obj);

    if(!response || !response->AudioOutputOptions){
        return;
    }
    OnvifMediaAudioOutputConfigOptions__set_option(ONVIF_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS(self),response->AudioOutputOptions);
}

static void
OnvifDeviceIOAudioOutputConfigurationOptions__class_init (OnvifDeviceIOAudioOutputConfigurationOptionsClass * klass){
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifDeviceIOAudioOutputConfigurationOptions__construct;
}

static void
OnvifDeviceIOAudioOutputConfigurationOptions__init (OnvifDeviceIOAudioOutputConfigurationOptions * self){

}

OnvifDeviceIOAudioOutputConfigurationOptions* 
OnvifDeviceIOAudioOutputConfigurationOptions__new(struct _tmd__GetAudioOutputConfigurationOptionsResponse * resp){
    return g_object_new (ONVIF_TYPE_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS, "data", resp, NULL);
}