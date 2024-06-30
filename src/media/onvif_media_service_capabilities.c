#include "onvif_media_service_capabilities_local.h"
#include "clogger.h"

typedef struct {
    int snapshot_uri;
} OnvifMediaServiceCapabilitiesPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifMediaServiceCapabilities, OnvifMediaServiceCapabilities_, SOAP_TYPE_OBJECT)

static void
OnvifMediaServiceCapabilities__construct(SoapObject * obj, gpointer ptr){
    OnvifMediaServiceCapabilities * self = ONVIF_MEDIA_SERVICECAPABILITIES(obj);
    struct _trt__GetServiceCapabilitiesResponse * resp = ptr;

    OnvifMediaServiceCapabilitiesPrivate * priv = OnvifMediaServiceCapabilities__get_instance_private (self);
    if(!resp){
        priv->snapshot_uri = 0;
        return;
    }
    
    struct trt__Capabilities * caps = resp->Capabilities;
    // struct trt__ProfileCapabilities*     ProfileCapabilities = caps->ProfileCapabilities;
    // struct trt__StreamingCapabilities*   StreamingCapabilities = caps->StreamingCapabilities;
    if(!caps){
        C_ERROR("Media GetServiceCapabilities returned NULL. Default capabilities");
        priv->snapshot_uri = 0;
        return;
    }

    // enum xsd__boolean * Rotation = caps->Rotation;
    // enum xsd__boolean * VideoSourceMode = caps->VideoSourceMode;
    // enum xsd__boolean * OSD = caps->OSD;
    // enum xsd__boolean * TemporaryOSDText = caps->TemporaryOSDText;
    // enum xsd__boolean * EXICompression = caps->EXICompression;

    if(!caps->SnapshotUri){
        C_WARN("SnapshotUri is NULL");
        priv->snapshot_uri = 0;
    } else {
        switch(*caps->SnapshotUri){
            case xsd__boolean__false_:
                priv->snapshot_uri = 0;
                break;
            case xsd__boolean__true_:
                priv->snapshot_uri = 1;
                break;
            default:
                priv->snapshot_uri = 0;
                C_WARN(" default SnapshotUri %d",caps->SnapshotUri);
        }
    }
}

static void
OnvifMediaServiceCapabilities__class_init (OnvifMediaServiceCapabilitiesClass * klass)
{
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMediaServiceCapabilities__construct;
}

static void
OnvifMediaServiceCapabilities__init (OnvifMediaServiceCapabilities * self)
{
    // OnvifMediaServiceCapabilitiesPrivate *priv = OnvifMediaServiceCapabilities__get_instance_private (ONVIF_MEDIA_SERVICECAPABILITIES(self));


}

int OnvifMediaServiceCapabilities__get_snapshot_uri(OnvifMediaServiceCapabilities* self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_SERVICECAPABILITIES (self), 0);
    
    OnvifMediaServiceCapabilitiesPrivate *priv = OnvifMediaServiceCapabilities__get_instance_private (ONVIF_MEDIA_SERVICECAPABILITIES(self));
    return priv->snapshot_uri;
}

OnvifMediaServiceCapabilities* OnvifMediaServiceCapabilities__new(struct _trt__GetServiceCapabilitiesResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA_SERVICECAPABILITIES, "data", resp, NULL);
}