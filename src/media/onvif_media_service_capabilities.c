#include "onvif_media_service_capabilities_local.h"
#include "clogger.h"

enum
{
  PROP_SOAP = 1,
  N_PROPERTIES
};

typedef struct {
    int snapshot_uri;
} OnvifMediaServiceCapabilitiesPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifMediaServiceCapabilities, OnvifMediaServiceCapabilities_, SOAP_TYPE_OBJECT)
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
OnvifMediaServiceCapabilities__set_soap(OnvifMediaServiceCapabilities * self, struct _trt__GetServiceCapabilitiesResponse * resp){
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
OnvifMediaServiceCapabilities__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    OnvifMediaServiceCapabilities * self = ONVIF_MEDIASERVICECAPABILITIES (object);
    switch (prop_id){
        case PROP_SOAP:
            OnvifMediaServiceCapabilities__set_soap(self,g_value_get_pointer (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifMediaServiceCapabilities__get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    switch (prop_id){
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifMediaServiceCapabilities__class_init (OnvifMediaServiceCapabilitiesClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = OnvifMediaServiceCapabilities__set_property;
    object_class->get_property = OnvifMediaServiceCapabilities__get_property;
    obj_properties[PROP_SOAP] =
        g_param_spec_pointer ("soap",
                            "SoapMessage",
                            "Pointer to original soap message.",
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);

    g_object_class_install_properties (object_class,
                                        N_PROPERTIES,
                                        obj_properties);
}

static void
OnvifMediaServiceCapabilities__init (OnvifMediaServiceCapabilities * self)
{
    // OnvifMediaServiceCapabilitiesPrivate *priv = OnvifMediaServiceCapabilities__get_instance_private (ONVIF_MEDIASERVICECAPABILITIES(self));


}

int OnvifMediaServiceCapabilities__get_snapshot_uri(OnvifMediaServiceCapabilities* self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIASERVICECAPABILITIES (self), 0);
    
    OnvifMediaServiceCapabilitiesPrivate *priv = OnvifMediaServiceCapabilities__get_instance_private (ONVIF_MEDIASERVICECAPABILITIES(self));
    return priv->snapshot_uri;
}

OnvifMediaServiceCapabilities* OnvifMediaServiceCapabilities__new(struct _trt__GetServiceCapabilitiesResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIASERVICECAPABILITIES, "soap", resp, NULL);
}