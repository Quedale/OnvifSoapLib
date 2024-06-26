#ifndef ONVIF_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS_H_ 
#define ONVIF_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS_H_

#include "shard_export.h"
#include "SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS OnvifDeviceIOAudioOutputConfigurationOptions__get_type()
G_DECLARE_FINAL_TYPE (OnvifDeviceIOAudioOutputConfigurationOptions, OnvifDeviceIOAudioOutputConfigurationOptions_, ONVIF, DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS, SoapObject)

struct _OnvifDeviceIOAudioOutputConfigurationOptions
{
    SoapObject parent_instance;
};


struct _OnvifDeviceIOAudioOutputConfigurationOptionsClass
{
    SoapObjectClass parent_class;
};

//TODO Implement getters

G_END_DECLS

#endif