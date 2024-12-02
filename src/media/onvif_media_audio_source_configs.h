#ifndef ONVIF_MEDIA_AUDIO_SOURCE_CONFIGS_H_ 
#define ONVIF_MEDIA_AUDIO_SOURCE_CONFIGS_H_

#include "onvif_media_audio_source_config.h"
#include "../shard_export.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_AUDIO_SOURCE_CONFIGS OnvifMediaAudioSourceConfigs__get_type()
G_DECLARE_FINAL_TYPE (OnvifMediaAudioSourceConfigs, OnvifMediaAudioSourceConfigs_, ONVIF, MEDIA_AUDIO_SOURCE_CONFIGS, SoapObject)

struct _OnvifMediaAudioSourceConfigs {
    GObject parent_instance;
};


struct _OnvifMediaAudioSourceConfigsClass {
    GObjectClass parent_class;
};

SHARD_EXPORT int OnvifMediaAudioSourceConfigs__get_size(OnvifMediaAudioSourceConfigs * self);
SHARD_EXPORT OnvifMediaAudioSourceConfig * OnvifMediaAudioSourceConfigs__get_config(OnvifMediaAudioSourceConfigs * self,int index);

G_END_DECLS

#endif