#ifndef ONVIF_MEDIA_SERVICE_H_ 
#define ONVIF_MEDIA_SERVICE_H_

#include "../shard_export.h"
#include "../onvif_base_service.h"
#include "onvif_media_profiles.h"
#include "onvif_media_snapshot.h"
#include "onvif_media_service_capabilities.h"
#include "onvif_uri.h"
#include "onvif_media_audio_source_configs.h"
#include "onvif_media_audio_output_configs.h"
#include "onvif_media_audio_output_config_options.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_SERVICE OnvifMediaService__get_type()
G_DECLARE_DERIVABLE_TYPE (OnvifMediaService, OnvifMediaService_, ONVIF, MEDIA_SERVICE, OnvifBaseService)

typedef OnvifUri* (*MediaGetUriFunc) (OnvifMediaService  *self, int profile_index);
#define ONVIF_MEDIA_SERVICE_GET_URI_FUNC(f) ((MediaGetUriFunc) (void (*)(void)) (f))

typedef OnvifMediaProfiles* (*MediaGetProfilesFunc) (OnvifMediaService  *self);
#define ONVIF_MEDIA_SERVICE_GET_PROFILES_FUNC(f) ((MediaGetProfilesFunc) (void (*)(void)) (f))

typedef OnvifMediaServiceCapabilities* (*MediaGetServiceCapsFunc) (OnvifMediaService  *self);
#define ONVIF_MEDIA_SERVICE_GET_SERVICE_CAPS_FUNC(f) ((MediaGetServiceCapsFunc) (void (*)(void)) (f))

typedef OnvifMediaAudioSourceConfigs* (*MediaGetAudioSrcConfigsFunc) (OnvifMediaService  *self, int profile_index);
#define ONVIF_MEDIA_SERVICE_GET_AUDIO_SRC_CONFIGS_FUNC(f) ((MediaGetAudioSrcConfigsFunc) (void (*)(void)) (f))

typedef OnvifMediaAudioOutputConfigs* (*MediaGetAudioOutConfigsFunc) (OnvifMediaService  *self, int profile_index);
#define ONVIF_MEDIA_SERVICE_GET_AUDIO_OUT_CONFIGS_FUNC(f) ((MediaGetAudioOutConfigsFunc) (void (*)(void)) (f))

typedef OnvifMediaAudioOutputConfigOptions* (*MediaGetAudioOutConfigOptsFunc) (OnvifMediaService  *self, int profile_index);
#define ONVIF_MEDIA_SERVICE_GET_AUDIO_OUT_CONFIG_OPTS_FUNC(f) ((MediaGetAudioOutConfigOptsFunc) (void (*)(void)) (f))

#define ONVIF_MEDIA_SERVICE_GET_PROFILE_TOKEN_OR_GOTO(a,b,c) \
    char * a; \
    SoapFault fault = OnvifMediaService__get_profile_token(ONVIF_MEDIA_SERVICE(self),profile_index, (const char **) &a); \
    if(fault != SOAP_FAULT_NONE){ \
        C_WARN("Profile token not found %d.",fault); \
        ret_val = c(NULL); \
        SoapObject__set_fault(SOAP_OBJECT(ret_val),fault); \
        goto b; \
    } \
    if(!token || strlen(token) == 0) {  \
        C_WARN("Profile token not found."); \
        ret_val = c(NULL); \
        SoapObject__set_fault(SOAP_OBJECT(ret_val),SOAP_FAULT_UNEXPECTED); \
        goto b; \
    } 


struct _OnvifMediaServiceClass {
    OnvifBaseServiceClass parent_class;
    MediaGetUriFunc get_stream_uri;
    MediaGetUriFunc get_snapshot_uri;
    MediaGetServiceCapsFunc get_service_capabilities;
    MediaGetProfilesFunc get_profiles;
    MediaGetAudioSrcConfigsFunc get_audio_source_configs;
    MediaGetAudioOutConfigsFunc get_audio_output_configs;
    MediaGetAudioOutConfigOptsFunc get_audio_output_config_options;
};

//Lazy cache function
SHARD_EXPORT OnvifMediaProfiles * OnvifMediaService__get_profiles(OnvifMediaService * self);
SoapFault OnvifMediaService__get_profile_token(OnvifMediaService *self, int index, const char ** ret);

//Remote Sevice function
SHARD_EXPORT OnvifMediaProfiles * OnvifMediaService__getProfiles(OnvifMediaService *self);
SHARD_EXPORT OnvifUri * OnvifMediaService__getStreamUri(OnvifMediaService *self, int profile_index);
SHARD_EXPORT OnvifUri * OnvifMediaService__getSnapshotUri(OnvifMediaService *self, int profile_index);
SHARD_EXPORT OnvifMediaSnapshot * OnvifMediaService__getSnapshot(OnvifMediaService *self, int profile_index);
SHARD_EXPORT OnvifMediaServiceCapabilities * OnvifMediaService__getServiceCapabilities(OnvifMediaService *self);
SHARD_EXPORT OnvifMediaAudioSourceConfigs * OnvifMediaService__getAudioSourceConfigurations(OnvifMediaService *self, int profile_index);
SHARD_EXPORT OnvifMediaAudioOutputConfigs * OnvifMediaService__getAudioOutputConfigurations(OnvifMediaService *self, int profile_index);
SHARD_EXPORT OnvifMediaAudioOutputConfigOptions * OnvifMediaService__getAudioOutputConfigurationOptions(OnvifMediaService *self, int profile_index);

G_END_DECLS

#endif
