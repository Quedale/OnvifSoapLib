#ifndef ONVIF_MEDIA2_AUDIO_OUTPUT_CONFIG_OPTIONS_H_ 
#define ONVIF_MEDIA2_AUDIO_OUTPUT_CONFIG_OPTIONS_H_

#include "../onvif_media_audio_output_config_options.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA2_AUDIO_OUTPUT_CONFIG_OPTIONS OnvifMedia2AudioOutputConfigOptions__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia2AudioOutputConfigOptions, OnvifMedia2AudioOutputConfigOptions_, ONVIF, MEDIA2_AUDIO_OUTPUT_CONFIG_OPTIONS, OnvifMediaAudioOutputConfigOptions)

struct _OnvifMedia2AudioOutputConfigOptions {
    OnvifMediaAudioOutputConfigOptions parent_instance;
};


struct _OnvifMedia2AudioOutputConfigOptionsClass {
    OnvifMediaAudioOutputConfigOptionsClass parent_class;
};

G_END_DECLS

#endif