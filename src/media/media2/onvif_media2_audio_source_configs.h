#ifndef ONVIF_MEDIA2_AUDIO_SOURCE_CONFIGS_H_ 
#define ONVIF_MEDIA2_AUDIO_SOURCE_CONFIGS_H_

#include "../onvif_media_audio_source_config.h"
#include "../onvif_media_audio_source_configs.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA2_AUDIO_SOURCE_CONFIGS OnvifMedia2AudioSourceConfigs__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia2AudioSourceConfigs, OnvifMedia2AudioSourceConfigs_, ONVIF, MEDIA2_AUDIO_SOURCE_CONFIGS, OnvifMediaAudioSourceConfigs)

struct _OnvifMedia2AudioSourceConfigs {
    OnvifMediaAudioSourceConfigs parent_instance;
};


struct _OnvifMedia2AudioSourceConfigsClass {
    OnvifMediaAudioSourceConfigsClass parent_class;
};

G_END_DECLS

#endif