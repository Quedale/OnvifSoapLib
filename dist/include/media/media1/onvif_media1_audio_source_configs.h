#ifndef ONVIF_MEDIA1_AUDIO_SOURCE_CONFIGS_H_ 
#define ONVIF_MEDIA1_AUDIO_SOURCE_CONFIGS_H_

#include "../onvif_media_audio_source_config.h"
#include "../onvif_media_audio_source_configs.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA1_AUDIO_SOURCE_CONFIGS OnvifMedia1AudioSourceConfigs__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia1AudioSourceConfigs, OnvifMedia1AudioSourceConfigs_, ONVIF, MEDIA1_AUDIO_SOURCE_CONFIGS, OnvifMediaAudioSourceConfigs)

struct _OnvifMedia1AudioSourceConfigs {
    OnvifMediaAudioSourceConfigs parent_instance;
};


struct _OnvifMedia1AudioSourceConfigsClass {
    OnvifMediaAudioSourceConfigsClass parent_class;
};

G_END_DECLS

#endif
