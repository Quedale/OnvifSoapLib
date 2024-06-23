#include "onvif_device_hostnameinfo_local.h"
#include "clogger.h"

enum
{
  PROP_SOAP = 1,
  N_PROPERTIES
};

typedef struct {
    char * name;
    gboolean fromDHCP;
} OnvifDeviceHostnameInfoPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifDeviceHostnameInfo, OnvifDeviceHostnameInfo_, SOAP_TYPE_OBJECT)
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
OnvifDeviceHostnameInfo__set_soap(OnvifDeviceHostnameInfo * self, struct _tds__GetHostnameResponse * response){
    OnvifDeviceHostnameInfoPrivate *priv = OnvifDeviceHostnameInfo__get_instance_private (ONVIF_DEVICE_HOSTNAME_INFO(self));

    if(!response || !response->HostnameInformation){
        if(priv->name){
            free(priv->name);
            priv->name = NULL;
        }
        return;
    }

    if(response->HostnameInformation){
        if(!priv->name){
            priv->name = malloc(strlen(response->HostnameInformation->Name)+1);
        } else {
            priv->name = realloc(priv->name,strlen(response->HostnameInformation->Name)+1);
        }
        strcpy(priv->name,response->HostnameInformation->Name);

        switch(response->HostnameInformation->FromDHCP){
            case xsd__boolean__true_:
                priv->fromDHCP = 1;
                break;
            case xsd__boolean__false_:
                priv->fromDHCP = 0;
                break;
            default:
                priv->fromDHCP = 0;
                C_WARN("default HostnameInformation->FromDHCP %d",response->HostnameInformation->FromDHCP);
        }

        //TODO Map Extension once we have a need for it
    }
}

static void
OnvifDeviceHostnameInfo__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    OnvifDeviceHostnameInfo * self = ONVIF_DEVICE_HOSTNAME_INFO (object);
    // OnvifDeviceHostnameInfoPrivate *priv = OnvifDeviceHostnameInfo__get_instance_private (self);
    switch (prop_id){
        case PROP_SOAP:
            OnvifDeviceHostnameInfo__set_soap(self,g_value_get_pointer (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifDeviceHostnameInfo__get_property (GObject    *object,
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
OnvifDeviceHostnameInfo__dispose (GObject *self)
{
    OnvifDeviceHostnameInfoPrivate *priv = OnvifDeviceHostnameInfo__get_instance_private (ONVIF_DEVICE_HOSTNAME_INFO(self));
    if(priv->name){
        free(priv->name);
        priv->name = NULL;
    }
}

static void
OnvifDeviceHostnameInfo__class_init (OnvifDeviceHostnameInfoClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifDeviceHostnameInfo__dispose;
    object_class->set_property = OnvifDeviceHostnameInfo__set_property;
    object_class->get_property = OnvifDeviceHostnameInfo__get_property;
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
OnvifDeviceHostnameInfo__init (OnvifDeviceHostnameInfo * self)
{
    OnvifDeviceHostnameInfoPrivate *priv = OnvifDeviceHostnameInfo__get_instance_private (ONVIF_DEVICE_HOSTNAME_INFO(self));
    priv->name = NULL;
    priv->fromDHCP = -1;
}

OnvifDeviceHostnameInfo* OnvifDeviceHostnameInfo__new(struct _tds__GetHostnameResponse * resp){
    return g_object_new (ONVIF_TYPE_DEVICE_HOSTNAME_INFO, "soap", resp, NULL);
}

char * OnvifDeviceHostnameInfo__get_name(OnvifDeviceHostnameInfo * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_HOSTNAME_INFO (self), NULL);
    OnvifDeviceHostnameInfoPrivate *priv = OnvifDeviceHostnameInfo__get_instance_private (ONVIF_DEVICE_HOSTNAME_INFO(self));

    return priv->name;

}

gboolean OnvifDeviceHostnameInfo__is_fromDHCP(OnvifDeviceHostnameInfo * self){
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (ONVIF_IS_DEVICE_HOSTNAME_INFO (self), FALSE);
    OnvifDeviceHostnameInfoPrivate *priv = OnvifDeviceHostnameInfo__get_instance_private (ONVIF_DEVICE_HOSTNAME_INFO(self));

    return priv->fromDHCP;
}

