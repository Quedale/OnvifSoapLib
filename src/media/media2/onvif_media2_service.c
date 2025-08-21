#include "../onvif_base_service_local.h"
#include "onvif_media2_service.h"
#include "onvif_media2_service_capabilities_local.h"
#include "onvif_media2_profiles_local.h"
#include "onvif_media2_uri_local.h"
#include "onvif_media2_audio_source_configs_local.h"
#include "onvif_media2_audio_output_configs_local.h"
#include "onvif_media2_audio_output_config_options_local.h"

G_DEFINE_TYPE(OnvifMedia2Service, OnvifMedia2Service_, ONVIF_TYPE_MEDIA_SERVICE)

static void
OnvifMedia2Service__class_init (OnvifMedia2ServiceClass * klass){
    OnvifMediaServiceClass * media_class = ONVIF_MEDIA_SERVICE_CLASS(klass);
    media_class->get_stream_uri = ONVIF_MEDIA_SERVICE_GET_URI_FUNC(OnvifMedia2Service__getStreamUri);
    media_class->get_snapshot_uri = ONVIF_MEDIA_SERVICE_GET_URI_FUNC(OnvifMedia2Service__getSnapshotUri);
    media_class->get_profiles = ONVIF_MEDIA_SERVICE_GET_PROFILES_FUNC(OnvifMedia2Service__getProfiles);
    media_class->get_service_capabilities = ONVIF_MEDIA_SERVICE_GET_SERVICE_CAPS_FUNC(OnvifMedia2Service__getServiceCapabilities);
    media_class->get_audio_source_configs = ONVIF_MEDIA_SERVICE_GET_AUDIO_SRC_CONFIGS_FUNC(OnvifMedia2Service__getAudioSourceConfigurations);
    media_class->get_audio_output_configs = ONVIF_MEDIA_SERVICE_GET_AUDIO_OUT_CONFIGS_FUNC(OnvifMedia2Service__getAudioOutputConfigurations);
    media_class->get_audio_output_config_options = ONVIF_MEDIA_SERVICE_GET_AUDIO_OUT_CONFIG_OPTS_FUNC(OnvifMedia2Service__getAudioOutputConfigurationOptions);
}

static void
OnvifMedia2Service__init (OnvifMedia2Service * self){

}


OnvifMedia2Service* 
OnvifMedia2Service__new(OnvifDevice * device, const char * endpoint){
    return g_object_new (ONVIF_TYPE_MEDIA2_SERVICE, "device", device, "uri", endpoint, NULL);
}

OnvifMedia2ServiceCapabilities * 
OnvifMedia2Service__getServiceCapabilities(OnvifMedia2Service* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA2_SERVICE (self), NULL);
    
    ONVIF_PREPARE_SOAP_CALL(OnvifMedia2ServiceCapabilities, _ns1__GetServiceCapabilities, _ns1__GetServiceCapabilitiesResponse);
    ONVIF_INVOKE_SOAP_CALL(self, ns1__GetServiceCapabilities, OnvifMedia2ServiceCapabilities__new, ret_val, &request,  &response);
    
    return ret_val;
}

OnvifMedia2Profiles * 
OnvifMedia2Service__getProfiles(OnvifMedia2Service* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA2_SERVICE (self), NULL);
    
    ONVIF_PREPARE_SOAP_CALL(OnvifMedia2Profiles, _ns1__GetProfiles, _ns1__GetProfilesResponse);
    ONVIF_INVOKE_SOAP_CALL(self, ns1__GetProfiles, OnvifMedia2Profiles__new, ret_val, &request,  &response);
    
    return ret_val;
}

OnvifMedia2AudioSourceConfigs *
OnvifMedia2Service__getAudioSourceConfigurations(OnvifMedia2Service *self, int profile_index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA2_SERVICE (self), NULL);
    
    ONVIF_PREPARE_SOAP_CALL(OnvifMedia2AudioSourceConfigs, ns1__GetConfiguration, _ns1__GetAudioSourceConfigurationsResponse);
    ONVIF_MEDIA_SERVICE_GET_PROFILE_TOKEN_OR_GOTO(token, exit, OnvifMedia2AudioSourceConfigs__new);
    
    request.ProfileToken = token;

    ONVIF_INVOKE_SOAP_CALL(self, ns1__GetAudioSourceConfigurations, OnvifMedia2AudioSourceConfigs__new, ret_val, &request,  &response);
exit:
    return ret_val;
}

OnvifMedia2AudioOutputConfigs *
OnvifMedia2Service__getAudioOutputConfigurations(OnvifMedia2Service *self, int profile_index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA2_SERVICE (self), NULL);
    
    ONVIF_PREPARE_SOAP_CALL(OnvifMedia2AudioOutputConfigs, ns1__GetConfiguration, _ns1__GetAudioOutputConfigurationsResponse);
    ONVIF_MEDIA_SERVICE_GET_PROFILE_TOKEN_OR_GOTO(token, exit, OnvifMedia2AudioOutputConfigs__new);
    
    request.ProfileToken = token;
    request.ConfigurationToken = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, ns1__GetAudioOutputConfigurations, OnvifMedia2AudioOutputConfigs__new, ret_val, &request,  &response);
exit:
    return ret_val;
}

OnvifMedia2AudioOutputConfigOptions *
OnvifMedia2Service__getAudioOutputConfigurationOptions(OnvifMedia2Service *self, int profile_index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA2_SERVICE (self), NULL);
    
    ONVIF_PREPARE_SOAP_CALL(OnvifMedia2AudioOutputConfigOptions, ns1__GetConfiguration, _ns1__GetAudioOutputConfigurationOptionsResponse);
    ONVIF_MEDIA_SERVICE_GET_PROFILE_TOKEN_OR_GOTO(token, exit, OnvifMedia2AudioOutputConfigOptions__new);

    request.ProfileToken = token;
    request.ConfigurationToken = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, ns1__GetAudioOutputConfigurationOptions, OnvifMedia2AudioOutputConfigOptions__new, ret_val, &request,  &response);
exit:
    return ret_val;
}

OnvifMedia2Uri * 
OnvifMedia2Service__getStreamUri(OnvifMedia2Service* self, int profile_index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA2_SERVICE (self), NULL);
    
    ONVIF_PREPARE_SOAP_CALL(OnvifMedia2Uri, _ns1__GetStreamUri, _ns1__GetStreamUriResponse);
    ONVIF_MEDIA_SERVICE_GET_PROFILE_TOKEN_OR_GOTO(token, exit, OnvifMedia2StreamUri__new);
    
    request.ProfileToken = token;
    request.Protocol = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, ns1__GetStreamUri, OnvifMedia2StreamUri__new, ret_val, &request,  &response);

exit:
    return ret_val;
}


OnvifMedia2Uri * 
OnvifMedia2Service__getSnapshotUri(OnvifMedia2Service *self, int profile_index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA2_SERVICE (self), NULL);

    ONVIF_PREPARE_SOAP_CALL(OnvifMedia2Uri, _ns1__GetSnapshotUri, _ns1__GetSnapshotUriResponse);
    ONVIF_MEDIA_SERVICE_GET_PROFILE_TOKEN_OR_GOTO(token, exit, OnvifMedia2SnapshotUri__new);

    request.ProfileToken = token;

    ONVIF_INVOKE_SOAP_CALL(self, ns1__GetSnapshotUri, OnvifMedia2SnapshotUri__new, ret_val, &request,  &response);

exit:
    return ret_val;
}
