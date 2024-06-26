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

    struct _tmd__GetAudioOutputConfigurationOptions request;
    struct _tmd__GetAudioOutputConfigurationOptionsResponse response;

    OnvifDeviceIOAudioOutputConfigurationOptions * ret_val;
    memset (&ret_val, 0, sizeof (ret_val));
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    ONVIF_INVOKE_SOAP_CALL(self, tmd__GetAudioOutputConfigurationOptions, OnvifDeviceIOAudioOutputConfigurationOptions__new, ret_val, soap, NULL, &request,  &response);

    return ret_val;
}

OnvifTokens * OnvifDeviceIOService__getAudioOutputs(OnvifDeviceIOService * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICEIO_SERVICE (self), NULL);

    struct tmd__Get request;
    struct tmd__GetResponse response;

    OnvifTokens * ret_val;
    memset (&ret_val, 0, sizeof (ret_val));
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    ONVIF_INVOKE_SOAP_CALL(self, tmd__GetAudioOutputs, OnvifTokens__new, ret_val, soap, NULL, &request,  &response);

    return ret_val;
}

OnvifTokens * OnvifDeviceIOService__getAudioSources(OnvifDeviceIOService * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICEIO_SERVICE (self), NULL);

    struct tmd__Get request;
    struct tmd__GetResponse response;

    OnvifTokens * ret_val;
    memset (&ret_val, 0, sizeof (ret_val));
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));
    // request.AudioOutputToken = 1;

    ONVIF_INVOKE_SOAP_CALL(self, tmd__GetAudioSources, OnvifTokens__new, ret_val, soap, NULL, &request,  &response);

    return ret_val;
}