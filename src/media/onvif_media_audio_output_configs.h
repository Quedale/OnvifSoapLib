#ifndef ONVIF_MEDIA_AUDIO_OUTPUT_CONFIGS_H_ 
#define ONVIF_MEDIA_AUDIO_OUTPUT_CONFIGS_H_

#include "onvif_media_audio_output_config.h"
#include "../shard_export.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_AUDIO_OUTPUT_CONFIGS OnvifMediaAudioOutputConfigs__get_type()
G_DECLARE_FINAL_TYPE (OnvifMediaAudioOutputConfigs, OnvifMediaAudioOutputConfigs_, ONVIF, MEDIA_AUDIO_OUTPUT_CONFIGS, SoapObject)

struct _OnvifMediaAudioOutputConfigs {
    GObject parent_instance;
};


struct _OnvifMediaAudioOutputConfigsClass {
    GObjectClass parent_class;
};

SHARD_EXPORT int OnvifMediaAudioOutputConfigs__get_size(OnvifMediaAudioOutputConfigs * self);
SHARD_EXPORT OnvifMediaAudioOutputConfig * OnvifMediaAudioOutputConfigs__get_config(OnvifMediaAudioOutputConfigs * self,int index);

G_END_DECLS

#endif