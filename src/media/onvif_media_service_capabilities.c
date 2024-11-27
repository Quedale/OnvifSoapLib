#include "onvif_media_service_capabilities_local.h"
#include "../onvif_base_service_local.h"

typedef struct {
    gboolean snapshot_uri;
} OnvifMediaServiceCapabilitiesPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(OnvifMediaServiceCapabilities, OnvifMediaServiceCapabilities_, SOAP_TYPE_OBJECT)

static void
OnvifMediaServiceCapabilities__class_init (OnvifMediaServiceCapabilitiesClass * klass){

}

static void
OnvifMediaServiceCapabilities__init (OnvifMediaServiceCapabilities * self){
    OnvifMediaServiceCapabilitiesPrivate *priv = OnvifMediaServiceCapabilities__get_instance_private (ONVIF_MEDIA_SERVICECAPABILITIES(self));
    priv->snapshot_uri = FALSE;
}

void 
OnvifMediaServiceCapabilities__set_snapshot_uri(OnvifMediaServiceCapabilities* self, gboolean val){
    g_return_if_fail (self != NULL);
    g_return_if_fail (ONVIF_IS_MEDIA_SERVICECAPABILITIES (self));
    OnvifMediaServiceCapabilitiesPrivate *priv = OnvifMediaServiceCapabilities__get_instance_private (ONVIF_MEDIA_SERVICECAPABILITIES(self));
    priv->snapshot_uri = val;
}

int 
OnvifMediaServiceCapabilities__get_snapshot_uri(OnvifMediaServiceCapabilities* self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_SERVICECAPABILITIES (self), 0);
    OnvifMediaServiceCapabilitiesPrivate *priv = OnvifMediaServiceCapabilities__get_instance_private (ONVIF_MEDIA_SERVICECAPABILITIES(self));
    return priv->snapshot_uri;
}