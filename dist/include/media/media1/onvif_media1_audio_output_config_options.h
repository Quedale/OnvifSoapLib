#ifndef ONVIF_MEDIA1_AUDIO_OUTPUT_CONFIG_OPTIONS_H_ 
#define ONVIF_MEDIA1_AUDIO_OUTPUT_CONFIG_OPTIONS_H_

#include "../onvif_media_audio_output_config_options.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA1_AUDIO_OUTPUT_CONFIG_OPTIONS OnvifMedia1AudioOutputConfigOptions__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia1AudioOutputConfigOptions, OnvifMedia1AudioOutputConfigOptions_, ONVIF, MEDIA1_AUDIO_OUTPUT_CONFIG_OPTIONS, OnvifMediaAudioOutputConfigOptions)

struct _OnvifMedia1AudioOutputConfigOptions {
    OnvifMediaAudioOutputConfigOptions parent_instance;
};


struct _OnvifMedia1AudioOutputConfigOptionsClass {
    OnvifMediaAudioOutputConfigOptionsClass parent_class;
};

G_END_DECLS

#endif
