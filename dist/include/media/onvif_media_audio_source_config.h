#ifndef ONVIF_MEDIA_AUDIO_SOURCE_CONFIG_H_ 
#define ONVIF_MEDIA_AUDIO_SOURCE_CONFIG_H_

#include "../SoapObject.h"
#include "../shard_export.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_AUDIO_SOURCE_CONFIG OnvifMediaAudioSourceConfig__get_type()
G_DECLARE_FINAL_TYPE (OnvifMediaAudioSourceConfig, OnvifMediaAudioSourceConfig_, ONVIF, MEDIA_AUDIO_SOURCE_CONFIG, SoapObject)

struct _OnvifMediaAudioSourceConfig {
    GObject parent_instance;
};


struct _OnvifMediaAudioSourceConfigClass {
    GObjectClass parent_class;
};

SHARD_EXPORT char * OnvifMediaAudioSourceConfig__get_token(OnvifMediaAudioSourceConfig * self);
SHARD_EXPORT char * OnvifMediaAudioSourceConfig__get_name(OnvifMediaAudioSourceConfig * self);
SHARD_EXPORT int OnvifMediaAudioSourceConfig__get_use_count(OnvifMediaAudioSourceConfig * self);
SHARD_EXPORT char * OnvifMediaAudioSourceConfig__get_source_token(OnvifMediaAudioSourceConfig * self);

G_END_DECLS

#endif
