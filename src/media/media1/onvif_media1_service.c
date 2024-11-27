#include "../onvif_media_snapshot_local.h"
#include "../onvif_media_service_local.h"
#include "onvif_media1_service.h"
#include "onvif_media1_profiles_local.h"
#include "onvif_media1_uri_local.h"
#include "onvif_media1_service_capabilities_local.h"

G_DEFINE_TYPE(OnvifMedia1Service, OnvifMedia1Service_, ONVIF_TYPE_MEDIA_SERVICE)

static void
OnvifMedia1Service__class_init (OnvifMedia1ServiceClass * klass) {
    OnvifMediaServiceClass * media_class = ONVIF_MEDIA_SERVICE_CLASS(klass);
    media_class->get_stream_uri = ONVIF_MEDIA_SERVICE_GET_URI_FUNC(OnvifMedia1Service__getStreamUri);
    media_class->get_snapshot_uri = ONVIF_MEDIA_SERVICE_GET_URI_FUNC(OnvifMedia1Service__getSnapshotUri);
    media_class->get_profiles = ONVIF_MEDIA_SERVICE_GET_PROFILES_FUNC(OnvifMedia1Service__getProfiles);
    media_class->get_service_capabilities = ONVIF_MEDIA_SERVICE_GET_SERVICE_CAPS_FUNC(OnvifMedia1Service__getServiceCapabilities);
}

static void
OnvifMedia1Service__init (OnvifMedia1Service * self) {

}

OnvifMedia1Service* 
OnvifMedia1Service__new(OnvifDevice * device, const char * endpoint){
    return g_object_new (ONVIF_TYPE_MEDIA1_SERVICE, "device", device, "uri", endpoint, NULL);
}

OnvifMedia1ServiceCapabilities * 
OnvifMedia1Service__getServiceCapabilities(OnvifMedia1Service* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA1_SERVICE (self), NULL);
    
    ONVIF_PREPARE_SOAP_CALL(OnvifMedia1ServiceCapabilities, _trt__GetServiceCapabilities, _trt__GetServiceCapabilitiesResponse);
    ONVIF_INVOKE_SOAP_CALL(self, trt__GetServiceCapabilities, OnvifMedia1ServiceCapabilities__new, ret_val, &request,  &response);
    
    return ret_val;
}

OnvifMedia1Profiles * 
OnvifMedia1Service__getProfiles(OnvifMedia1Service* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA1_SERVICE (self), NULL);
    
    ONVIF_PREPARE_SOAP_CALL(OnvifMedia1Profiles, _trt__GetProfiles, _trt__GetProfilesResponse);
    ONVIF_INVOKE_SOAP_CALL(self, trt__GetProfiles, OnvifMedia1Profiles__new, ret_val, &request,  &response);
    
    return ret_val;
}

OnvifMedia1Uri * 
OnvifMedia1Service__getStreamUri(OnvifMedia1Service* self, int profile_index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA1_SERVICE (self), NULL);
    
    ONVIF_PREPARE_SOAP_CALL(OnvifMedia1Uri, _trt__GetStreamUri, _trt__GetStreamUriResponse);

    ONVIF_MEDIA_SERVICE_GET_PROFILE_TOKEN_OR_GOTO(token, exit, OnvifMedia1StreamUri__new);
    
    request.ProfileToken = token;

    struct tt__StreamSetup ssetup;
    memset(&ssetup,0,sizeof(ssetup));
    request.StreamSetup = &ssetup;
    request.StreamSetup->Stream = tt__StreamType__RTP_Unicast;//stream type

    struct tt__Transport transport;
    memset(&transport,0,sizeof(transport));
    request.StreamSetup->Transport = &transport;
    request.StreamSetup->Transport->Protocol = tt__TransportProtocol__UDP;
    request.StreamSetup->Transport->Tunnel = 0;
    ONVIF_INVOKE_SOAP_CALL(self, trt__GetStreamUri, OnvifMedia1StreamUri__new, ret_val, &request,  &response);

exit:
    return ret_val;
}


OnvifMedia1Uri * 
OnvifMedia1Service__getSnapshotUri(OnvifMedia1Service *self, int profile_index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA1_SERVICE (self), NULL);

    ONVIF_PREPARE_SOAP_CALL(OnvifMedia1Uri, _trt__GetSnapshotUri, _trt__GetSnapshotUriResponse);
    char * endpoint = OnvifBaseService__get_endpoint(ONVIF_BASE_SERVICE(self));

    ONVIF_MEDIA_SERVICE_GET_PROFILE_TOKEN_OR_GOTO(token, exit, OnvifMedia1SnapshotUri__new);

    request.ProfileToken = token;

    ONVIF_INVOKE_SOAP_CALL(self, trt__GetSnapshotUri, OnvifMedia1SnapshotUri__new, ret_val, &request,  &response);

exit:
    free(endpoint);
    return ret_val;
}