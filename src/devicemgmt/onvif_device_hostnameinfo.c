#include "onvif_device_hostnameinfo_local.h"
#include "clogger.h"
#include "../SoapObject_local.h"

typedef struct {
    char * name;
    gboolean fromDHCP;
} OnvifDeviceHostnameInfoPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifDeviceHostnameInfo, OnvifDeviceHostnameInfo_, SOAP_TYPE_OBJECT)

static void
OnvifDeviceHostnameInfo__construct(SoapObject * obj, gpointer ptr){
    struct _tds__GetHostnameResponse * response = ptr;
    OnvifDeviceHostnameInfoPrivate *priv = OnvifDeviceHostnameInfo__get_instance_private (ONVIF_DEVICE_HOSTNAME_INFO(obj));

    if(!response || !response->HostnameInformation){
        if(priv->name){
            free(priv->name);
            priv->name = NULL;
        }
        return;
    }

    if(response->HostnameInformation->Name){
        if(!priv->name){
            priv->name = malloc(strlen(response->HostnameInformation->Name)+1);
        } else {
            priv->name = realloc(priv->name,strlen(response->HostnameInformation->Name)+1);
        }
        strcpy(priv->name,response->HostnameInformation->Name);
        //TODO Map Extension once we have a need for it
    } else {
        SoapObject__set_fault(obj,SOAP_FAULT_UNEXPECTED);
        return;
    }
    
    switch(response->HostnameInformation->FromDHCP){
        case xsd__boolean__true_:
            priv->fromDHCP = 1;
            break;
        case xsd__boolean__false_:
            priv->fromDHCP = 0;
            break;
        default:
            if(priv->name){
                free(priv->name);
                priv->name = NULL;
            }
            SoapObject__set_fault(obj,SOAP_FAULT_UNEXPECTED);
            C_WARN("Invalid HostnameInformation->FromDHCP %d",response->HostnameInformation->FromDHCP);
            
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
    G_OBJECT_CLASS (OnvifDeviceHostnameInfo__parent_class)->dispose (self);
}

static void
OnvifDeviceHostnameInfo__class_init (OnvifDeviceHostnameInfoClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifDeviceHostnameInfo__dispose;
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifDeviceHostnameInfo__construct;
}

static void
OnvifDeviceHostnameInfo__init (OnvifDeviceHostnameInfo * self)
{
    OnvifDeviceHostnameInfoPrivate *priv = OnvifDeviceHostnameInfo__get_instance_private (ONVIF_DEVICE_HOSTNAME_INFO(self));
    priv->name = NULL;
    priv->fromDHCP = -1;
}

OnvifDeviceHostnameInfo* OnvifDeviceHostnameInfo__new(struct _tds__GetHostnameResponse * resp){
    return g_object_new (ONVIF_TYPE_DEVICE_HOSTNAME_INFO, "data", resp, NULL);
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

