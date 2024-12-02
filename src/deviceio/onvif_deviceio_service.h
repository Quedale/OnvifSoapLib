#ifndef ONVIF_DEVICEIO_SERVICE_H_ 
#define ONVIF_DEVICEIO_SERVICE_H_

#include "../shard_export.h"
#include "../onvif_base_service.h"
#include "onvif_deviceio_audio_output_configuration_options.h"
#include "onvif_tokens.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICEIO_SERVICE OnvifDeviceIOService__get_type()
G_DECLARE_FINAL_TYPE (OnvifDeviceIOService, OnvifDeviceIOService_, ONVIF, DEVICEIO_SERVICE, OnvifBaseService)

struct _OnvifDeviceIOService
{
    OnvifBaseService parent_instance;
};


struct _OnvifDeviceIOServiceClass
{
    OnvifBaseServiceClass parent_class;
};

SHARD_EXPORT OnvifDeviceIOService * OnvifDeviceIOService__new(OnvifDevice * device, const char * endpoint);
SHARD_EXPORT OnvifTokens * OnvifDeviceIOService__getAudioOutputs(OnvifDeviceIOService * self);
SHARD_EXPORT OnvifTokens * OnvifDeviceIOService__getAudioSources(OnvifDeviceIOService * self);
SHARD_EXPORT OnvifDeviceIOAudioOutputConfigurationOptions * OnvifDeviceIOService__getAudioOutputConfigurationOptions(OnvifDeviceIOService * self);

#endif