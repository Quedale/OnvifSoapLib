#ifndef ONVIF_MEDIA_AUDIO_OUTPUT_CONFIG_H_ 
#define ONVIF_MEDIA_AUDIO_OUTPUT_CONFIG_H_

#include "../SoapObject.h"
#include "../shard_export.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_AUDIO_OUTPUT_CONFIG OnvifMediaAudioOutputConfig__get_type()
G_DECLARE_FINAL_TYPE (OnvifMediaAudioOutputConfig, OnvifMediaAudioOutputConfig_, ONVIF, MEDIA_AUDIO_OUTPUT_CONFIG, SoapObject)

struct _OnvifMediaAudioOutputConfig {
    GObject parent_instance;
};


struct _OnvifMediaAudioOutputConfigClass {
    GObjectClass parent_class;
};

SHARD_EXPORT char * OnvifMediaAudioOutputConfig__get_token(OnvifMediaAudioOutputConfig * self);
SHARD_EXPORT char * OnvifMediaAudioOutputConfig__get_name(OnvifMediaAudioOutputConfig * self);
SHARD_EXPORT int OnvifMediaAudioOutputConfig__get_use_count(OnvifMediaAudioOutputConfig * self);
SHARD_EXPORT char * OnvifMediaAudioOutputConfig__get_output_token(OnvifMediaAudioOutputConfig * self);

G_END_DECLS

#endif
