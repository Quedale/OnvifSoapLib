#ifndef ONVIF_MEDIA1_AUDIO_OUTPUT_CONFIGS_H_ 
#define ONVIF_MEDIA1_AUDIO_OUTPUT_CONFIGS_H_

#include "../onvif_media_audio_output_config.h"
#include "../onvif_media_audio_output_configs.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA1_AUDIO_OUTPUT_CONFIGS OnvifMedia1AudioOutputConfigs__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia1AudioOutputConfigs, OnvifMedia1AudioOutputConfigs_, ONVIF, MEDIA1_AUDIO_OUTPUT_CONFIGS, OnvifMediaAudioOutputConfigs)

struct _OnvifMedia1AudioOutputConfigs {
    OnvifMediaAudioOutputConfigs parent_instance;
};


struct _OnvifMedia1AudioOutputConfigsClass {
    OnvifMediaAudioOutputConfigsClass parent_class;
};

G_END_DECLS

#endif