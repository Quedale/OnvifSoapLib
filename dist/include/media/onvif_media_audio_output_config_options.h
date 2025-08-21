#ifndef ONVIF_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS_H_ 
#define ONVIF_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS_H_

#include "../SoapObject.h"
#include "../shard_export.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS OnvifMediaAudioOutputConfigOptions__get_type()
G_DECLARE_FINAL_TYPE (OnvifMediaAudioOutputConfigOptions, OnvifMediaAudioOutputConfigOptions_, ONVIF, MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS, SoapObject)

struct _OnvifMediaAudioOutputConfigOptions {
    GObject parent_instance;
};


struct _OnvifMediaAudioOutputConfigOptionsClass {
    GObjectClass parent_class;
};

SHARD_EXPORT char * OnvifMediaAudioOutputConfigOptions__get_ouput_token_available(OnvifMediaAudioOutputConfigOptions * self, int index);
SHARD_EXPORT int OnvifMediaAudioOutputConfigOptions__ouput_tokens_available_count(OnvifMediaAudioOutputConfigOptions * self);
SHARD_EXPORT char * OnvifMediaAudioOutputConfigOptions__get_send_primacy_option(OnvifMediaAudioOutputConfigOptions * self, int index);
SHARD_EXPORT int OnvifMediaAudioOutputConfigOptions__get_send_primacy_options_count(OnvifMediaAudioOutputConfigOptions * self);
SHARD_EXPORT int OnvifMediaAudioOutputConfigOptions__get_min_level(OnvifMediaAudioOutputConfigOptions * self);
SHARD_EXPORT int OnvifMediaAudioOutputConfigOptions__get_max_level(OnvifMediaAudioOutputConfigOptions * self);

G_END_DECLS

#endif
