#ifndef ONVIF_MEDIA2_SERVICE_H_ 
#define ONVIF_MEDIA2_SERVICE_H_

#include "../../shard_export.h"
#include "../onvif_media_service.h"
#include "onvif_media2_service_capabilities.h"
#include "onvif_media2_profiles.h"
#include "onvif_media2_uri.h"
#include "onvif_media2_audio_source_configs.h"
#include "onvif_media2_audio_output_configs.h"
#include "onvif_media2_audio_output_config_options.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA2_SERVICE OnvifMedia2Service__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia2Service, OnvifMedia2Service_, ONVIF, MEDIA2_SERVICE, OnvifMediaService)

struct _OnvifMedia2Service {
    OnvifMediaService parent_instance;
};


struct _OnvifMedia2ServiceClass {
    OnvifMediaServiceClass parent_class;
};

SHARD_EXPORT OnvifMedia2Service * OnvifMedia2Service__new(OnvifDevice * device, const char * endpoint);

//Remote Sevice function
SHARD_EXPORT OnvifMedia2ServiceCapabilities * OnvifMedia2Service__getServiceCapabilities(OnvifMedia2Service* self);
SHARD_EXPORT OnvifMedia2Profiles * OnvifMedia2Service__getProfiles(OnvifMedia2Service* self);
SHARD_EXPORT OnvifMedia2Uri * OnvifMedia2Service__getSnapshotUri(OnvifMedia2Service *self, int profile_index);
SHARD_EXPORT OnvifMedia2Uri * OnvifMedia2Service__getStreamUri(OnvifMedia2Service* self, int profile_index);
SHARD_EXPORT OnvifMedia2AudioSourceConfigs * OnvifMedia2Service__getAudioSourceConfigurations(OnvifMedia2Service *self, int profile_index);
SHARD_EXPORT OnvifMedia2AudioOutputConfigs * OnvifMedia2Service__getAudioOutputConfigurations(OnvifMedia2Service *self, int profile_index);
SHARD_EXPORT OnvifMedia2AudioOutputConfigOptions * OnvifMedia2Service__getAudioOutputConfigurationOptions(OnvifMedia2Service *self, int profile_index);

G_END_DECLS

#endif