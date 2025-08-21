#include "../onvif_base_service_local.h"
#include "onvif_deviceio_service.h"
#include "onvif_deviceio_audio_output_configuration_options_local.h"
#include "onvif_tokens_local.h"

G_DEFINE_TYPE(OnvifDeviceIOService, OnvifDeviceIOService_, ONVIF_TYPE_BASE_SERVICE)

static void
OnvifDeviceIOService__class_init (OnvifDeviceIOServiceClass * klass)
{
    //Nothing todo here
}

static void
OnvifDeviceIOService__init (OnvifDeviceIOService * self)
{
    //Nothing todo here
}

OnvifDeviceIOService* OnvifDeviceIOService__new(OnvifDevice * device, const char * endpoint){
    return g_object_new (ONVIF_TYPE_DEVICEIO_SERVICE, "device", device, "uri", endpoint, NULL);
}

OnvifDeviceIOAudioOutputConfigurationOptions * OnvifDeviceIOService__getAudioOutputConfigurationOptions(OnvifDeviceIOService * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICEIO_SERVICE (self), NULL);

    ONVIF_PREPARE_SOAP_CALL(OnvifDeviceIOAudioOutputConfigurationOptions, _tmd__GetAudioOutputConfigurationOptions, _tmd__GetAudioOutputConfigurationOptionsResponse);
    ONVIF_INVOKE_SOAP_CALL(self, tmd__GetAudioOutputConfigurationOptions, OnvifDeviceIOAudioOutputConfigurationOptions__new, ret_val, &request,  &response);

    return ret_val;
}

OnvifTokens * OnvifDeviceIOService__getAudioOutputs(OnvifDeviceIOService * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICEIO_SERVICE (self), NULL);

    ONVIF_PREPARE_SOAP_CALL(OnvifTokens, tmd__Get, tmd__GetResponse);
    ONVIF_INVOKE_SOAP_CALL(self, tmd__GetAudioOutputs, OnvifTokens__new, ret_val, &request,  &response);

    return ret_val;
}

OnvifTokens * OnvifDeviceIOService__getAudioSources(OnvifDeviceIOService * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICEIO_SERVICE (self), NULL);

    ONVIF_PREPARE_SOAP_CALL(OnvifTokens, tmd__Get, tmd__GetResponse);
    ONVIF_INVOKE_SOAP_CALL(self, tmd__GetAudioSources, OnvifTokens__new, ret_val, &request,  &response);

    return ret_val;
}
