#include "onvif_media2_service_capabilities_local.h"

typedef struct {
    char * placeholder;
} OnvifMedia2ServiceCapabilitiesPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifMedia2ServiceCapabilities, OnvifMedia2ServiceCapabilities_, ONVIF_TYPE_MEDIA_SERVICECAPABILITIES)

static void
OnvifMedia2ServiceCapabilities__construct(SoapObject * obj, gpointer ptr){
    OnvifMedia2ServiceCapabilities * self = ONVIF_MEDIA2_SERVICECAPABILITIES(obj);
    struct _ns1__GetServiceCapabilitiesResponse * resp = ptr;

    // OnvifMedia2ServiceCapabilitiesPrivate * priv = OnvifMedia2ServiceCapabilities__get_instance_private (self);
    if(!resp){
        OnvifMediaServiceCapabilities__set_snapshot_uri(ONVIF_MEDIA_SERVICECAPABILITIES(self),FALSE);
        return;
    }

    struct ns1__Capabilities2 * caps = resp->Capabilities;
    if(!caps){
        C_ERROR("Media2 GetServiceCapabilities returned NULL. Default capabilities");
        OnvifMediaServiceCapabilities__set_snapshot_uri(ONVIF_MEDIA_SERVICECAPABILITIES(self),FALSE);
        return;
    }

    gboolean snapshoturi;
    xsd__boolean_to_bool(caps->SnapshotUri,snapshoturi,"SnapshotUri");
    OnvifMediaServiceCapabilities__set_snapshot_uri(ONVIF_MEDIA_SERVICECAPABILITIES(self),snapshoturi);
}

static void
OnvifMedia2ServiceCapabilities__class_init (OnvifMedia2ServiceCapabilitiesClass * klass){
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMedia2ServiceCapabilities__construct;
}

static void
OnvifMedia2ServiceCapabilities__init (OnvifMedia2ServiceCapabilities * self){
    // OnvifMediaServiceCapabilitiesPrivate *priv = OnvifMediaServiceCapabilities__get_instance_private (ONVIF_MEDIA_SERVICECAPABILITIES(self));
}

OnvifMedia2ServiceCapabilities* OnvifMedia2ServiceCapabilities__new(struct _ns1__GetServiceCapabilitiesResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA2_SERVICECAPABILITIES, "data", resp, NULL);
}