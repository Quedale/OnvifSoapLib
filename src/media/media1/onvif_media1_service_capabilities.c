#include "onvif_media1_service_capabilities_local.h"
#include "clogger.h"
#include "../../xsd_simple_type_converter.h"

typedef struct {
    char * placeholder;
} OnvifMedia1ServiceCapabilitiesPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifMedia1ServiceCapabilities, OnvifMedia1ServiceCapabilities_, ONVIF_TYPE_MEDIA_SERVICECAPABILITIES)

static void
OnvifMedia1ServiceCapabilities__construct(SoapObject * obj, gpointer ptr){
    OnvifMedia1ServiceCapabilities * self = ONVIF_MEDIA1_SERVICECAPABILITIES(obj);
    struct _trt__GetServiceCapabilitiesResponse * resp = ptr;

    // OnvifMedia1ServiceCapabilitiesPrivate * priv = OnvifMedia1ServiceCapabilities__get_instance_private (self);
    if(!resp){
        return;
    }
    
    struct trt__Capabilities * caps = resp->Capabilities;
    // struct trt__ProfileCapabilities*     ProfileCapabilities = caps->ProfileCapabilities;
    // struct trt__StreamingCapabilities*   StreamingCapabilities = caps->StreamingCapabilities;
    if(!caps){
        C_ERROR("Media GetServiceCapabilities returned NULL. Default capabilities");
        OnvifMediaServiceCapabilities__set_snapshot_uri(ONVIF_MEDIA_SERVICECAPABILITIES(self),FALSE);
        return;
    }

    // enum xsd__boolean * Rotation = caps->Rotation;
    // enum xsd__boolean * VideoSourceMode = caps->VideoSourceMode;
    // enum xsd__boolean * OSD = caps->OSD;
    // enum xsd__boolean * TemporaryOSDText = caps->TemporaryOSDText;
    // enum xsd__boolean * EXICompression = caps->EXICompression;

    if(!caps->SnapshotUri){
        C_WARN("SnapshotUri is NULL");
        OnvifMediaServiceCapabilities__set_snapshot_uri(ONVIF_MEDIA_SERVICECAPABILITIES(self),FALSE);
    }

    OnvifMediaServiceCapabilities__set_snapshot_uri(ONVIF_MEDIA_SERVICECAPABILITIES(self),xsd__boolean_to_bool(caps->SnapshotUri,FALSE));
}

static void
OnvifMedia1ServiceCapabilities__class_init (OnvifMedia1ServiceCapabilitiesClass * klass){
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMedia1ServiceCapabilities__construct;
}

static void
OnvifMedia1ServiceCapabilities__init (OnvifMedia1ServiceCapabilities * self){
    // OnvifMedia1ServiceCapabilitiesPrivate *priv = OnvifMedia1ServiceCapabilities__get_instance_private (ONVIF_MEDIA1_SERVICECAPABILITIES(self));
}

OnvifMedia1ServiceCapabilities* OnvifMedia1ServiceCapabilities__new(struct _trt__GetServiceCapabilitiesResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA1_SERVICECAPABILITIES, "data", resp, NULL);
}