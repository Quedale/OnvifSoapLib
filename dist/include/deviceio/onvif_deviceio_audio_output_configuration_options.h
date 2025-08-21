#ifndef ONVIF_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS_H_ 
#define ONVIF_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS_H_

#include "../media/onvif_media_audio_output_config_options.h"
#include "../shard_export.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS OnvifDeviceIOAudioOutputConfigurationOptions__get_type()
G_DECLARE_FINAL_TYPE (OnvifDeviceIOAudioOutputConfigurationOptions, OnvifDeviceIOAudioOutputConfigurationOptions_, ONVIF, DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS, OnvifMediaAudioOutputConfigOptions)

struct _OnvifDeviceIOAudioOutputConfigurationOptions {
    OnvifMediaAudioOutputConfigOptions parent_instance;
};


struct _OnvifDeviceIOAudioOutputConfigurationOptionsClass {
    OnvifMediaAudioOutputConfigOptionsClass parent_class;
};

G_END_DECLS

#endif
