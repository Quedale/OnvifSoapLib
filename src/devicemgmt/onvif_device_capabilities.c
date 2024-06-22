#include "onvif_device_capabilities_local.h"

enum
{
  PROP_SOAP = 1,
  N_PROPERTIES
};

typedef struct _OnvifMedia {
    char *xaddr;
    //TODO StreamingCapabilities
    //TODO Extension
} OnvifMedia;

typedef struct {
    OnvifMedia *media;
} OnvifCapabilitiesPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifCapabilities, OnvifCapabilities_, SOAP_TYPE_OBJECT)
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
OnvifCapabilities__set_soap(OnvifCapabilities * self, struct _tds__GetCapabilitiesResponse * response){
    OnvifCapabilitiesPrivate *priv = OnvifCapabilities__get_instance_private (ONVIF_DEVICE_CAPABILITIES(self));

    if(priv->media){
        free(priv->media->xaddr);
        priv->media->xaddr = NULL;
        free(priv->media);
        priv->media = NULL;
    }
    
    if(!response || !response->Capabilities){
        return;
    }

    OnvifMedia* media =  (OnvifMedia *) malloc(sizeof(OnvifMedia));
    media->xaddr = malloc(strlen(response->Capabilities->Media->XAddr)+1);
    strcpy(media->xaddr,response->Capabilities->Media->XAddr);

    priv->media = media;
}

static void
OnvifCapabilities__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    OnvifCapabilities * self = ONVIF_DEVICE_CAPABILITIES (object);
    // OnvifCapabilitiesPrivate *priv = OnvifCapabilities__get_instance_private (self);
    switch (prop_id){
        case PROP_SOAP:
            OnvifCapabilities__set_soap(self,g_value_get_pointer (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifCapabilities__get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    // OnvifCapabilities *thread = ONVIF_DEVICE_CAPABILITIES (object);
    // OnvifCapabilitiesPrivate *priv = OnvifCapabilities__get_instance_private (thread);
    switch (prop_id){
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifCapabilities__dispose (GObject *self)
{
    OnvifCapabilitiesPrivate *priv = OnvifCapabilities__get_instance_private (ONVIF_DEVICE_CAPABILITIES(self));
    if(priv->media){
        free(priv->media->xaddr);
        priv->media->xaddr = NULL;
        free(priv->media);
        priv->media = NULL;
    }
}

static void
OnvifCapabilities__class_init (OnvifCapabilitiesClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifCapabilities__dispose;
    object_class->set_property = OnvifCapabilities__set_property;
    object_class->get_property = OnvifCapabilities__get_property;
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
OnvifCapabilities__init (OnvifCapabilities * self)
{
    OnvifCapabilitiesPrivate *priv = OnvifCapabilities__get_instance_private (ONVIF_DEVICE_CAPABILITIES(self));
    priv->media = NULL;

}

OnvifCapabilities* OnvifCapabilities__new(struct _tds__GetCapabilitiesResponse * resp){
    return g_object_new (ONVIF_TYPE_DEVICE_CAPABILITIES, "soap", resp, NULL);
}

OnvifMedia * OnvifCapabilities__get_media(OnvifCapabilities * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_CAPABILITIES (self), NULL);

    OnvifCapabilitiesPrivate *priv = OnvifCapabilities__get_instance_private (ONVIF_DEVICE_CAPABILITIES(self));
    return priv->media;
}

char * OnvifMedia__get_address(OnvifMedia * self){
    return self->xaddr;
}