#ifndef ONVIF_MEDIA2_AUDIO_OUTPUT_CONFIGS_H_ 
#define ONVIF_MEDIA2_AUDIO_OUTPUT_CONFIGS_H_

#include "../onvif_media_audio_output_config.h"
#include "../onvif_media_audio_output_configs.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA2_AUDIO_OUTPUT_CONFIGS OnvifMedia2AudioOutputConfigs__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia2AudioOutputConfigs, OnvifMedia2AudioOutputConfigs_, ONVIF, MEDIA2_AUDIO_OUTPUT_CONFIGS, OnvifMediaAudioOutputConfigs)

struct _OnvifMedia2AudioOutputConfigs {
    OnvifMediaAudioOutputConfigs parent_instance;
};


struct _OnvifMedia2AudioOutputConfigsClass {
    OnvifMediaAudioOutputConfigsClass parent_class;
};

G_END_DECLS

#endif