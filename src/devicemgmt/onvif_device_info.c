#include <stdlib.h>
#include <string.h>
#include "onvif_device_info_local.h"

enum
{
  PROP_SOAP = 1,
  N_PROPERTIES
};

typedef struct {
    char * manufacturer;
    char * model;
    char * firmwareVersion;
    char * serialNumber;
    char * hardwareId;
} OnvifDeviceInformationPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifDeviceInformation, OnvifDeviceInformation_, SOAP_TYPE_OBJECT)
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
OnvifDeviceInformation__set_soap(OnvifDeviceInformation * self, struct _tds__GetDeviceInformationResponse * response){
    OnvifDeviceInformationPrivate *priv = OnvifDeviceInformation__get_instance_private (ONVIF_DEVICE_INFORMATION(self));

    if(priv->firmwareVersion){
        free(priv->firmwareVersion);
        priv->firmwareVersion = NULL;
    }
    if(priv->hardwareId){
        free(priv->hardwareId);
        priv->hardwareId = NULL;
    }
    if(priv->manufacturer){
        free(priv->manufacturer);
        priv->manufacturer = NULL;
    }
    if(priv->model){
        free(priv->model);
        priv->model = NULL;
    }
    if(priv->serialNumber){
        free(priv->serialNumber);
        priv->serialNumber = NULL;
    }

    if(!response){
        return;
    }

    if(response->FirmwareVersion){
        priv->firmwareVersion = (char*) malloc(strlen(response->FirmwareVersion)+1);
        strcpy(priv->firmwareVersion,response->FirmwareVersion);
    } else {
        priv->firmwareVersion = NULL;
    }
    
    if(response->HardwareId){
        priv->hardwareId = (char*) malloc(strlen(response->HardwareId)+1);
        strcpy(priv->hardwareId,response->HardwareId);
    } else {
        priv->hardwareId = NULL;
    }

    if(response->Manufacturer){
        priv->manufacturer = (char*) malloc(strlen(response->Manufacturer)+1);
        strcpy(priv->manufacturer,response->Manufacturer);
    } else {
        priv->manufacturer = NULL;
    }

    if(response->Model){
        priv->model = (char*) malloc(strlen(response->Model)+1);
        strcpy(priv->model,response->Model);
    } else {
        priv->model = NULL;
    }

    if(response->SerialNumber){
        priv->serialNumber = (char*) malloc(strlen(response->SerialNumber)+1);
        strcpy(priv->serialNumber,response->SerialNumber);
    } else {
        priv->serialNumber = NULL;
    }

}

static void
OnvifDeviceInformation__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    OnvifDeviceInformation * self = ONVIF_DEVICE_INFORMATION (object);
    // OnvifDeviceInformationPrivate *priv = OnvifDeviceInformation__get_instance_private (self);
    switch (prop_id){
        case PROP_SOAP:
            OnvifDeviceInformation__set_soap(self,g_value_get_pointer (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifDeviceInformation__get_property (GObject    *object,
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
OnvifDeviceInformation__dispose (GObject *self)
{
    OnvifDeviceInformationPrivate *priv = OnvifDeviceInformation__get_instance_private (ONVIF_DEVICE_INFORMATION(self));
    if(priv->firmwareVersion){
        free(priv->firmwareVersion);
        priv->firmwareVersion = NULL;
    }
    if(priv->hardwareId){
        free(priv->hardwareId);
        priv->hardwareId = NULL;
    }
    if(priv->manufacturer){
        free(priv->manufacturer);
        priv->manufacturer = NULL;
    }
    if(priv->model){
        free(priv->model);
        priv->model = NULL;
    }
    if(priv->serialNumber){
        free(priv->serialNumber);
        priv->serialNumber = NULL;
    }
    G_OBJECT_CLASS (OnvifDeviceInformation__parent_class)->dispose (self);
}

static void
OnvifDeviceInformation__class_init (OnvifDeviceInformationClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifDeviceInformation__dispose;
    object_class->set_property = OnvifDeviceInformation__set_property;
    object_class->get_property = OnvifDeviceInformation__get_property;
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
OnvifDeviceInformation__init (OnvifDeviceInformation * self)
{
    OnvifDeviceInformationPrivate *priv = OnvifDeviceInformation__get_instance_private (ONVIF_DEVICE_INFORMATION(self));
    priv->manufacturer = NULL;
    priv->model = NULL;
    priv->firmwareVersion = NULL;
    priv->serialNumber = NULL;
    priv->hardwareId = NULL;
}

OnvifDeviceInformation* OnvifDeviceInformation__new(struct _tds__GetDeviceInformationResponse * resp){
    return g_object_new (ONVIF_TYPE_DEVICE_INFORMATION, "soap", resp, NULL);
}

char * OnvifDeviceInformation__get_manufacturer(OnvifDeviceInformation *self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_INFORMATION (self), NULL);
    OnvifDeviceInformationPrivate *priv = OnvifDeviceInformation__get_instance_private (ONVIF_DEVICE_INFORMATION(self));
    return priv->manufacturer;
}

char * OnvifDeviceInformation__get_model(OnvifDeviceInformation *self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_INFORMATION (self), NULL);
    OnvifDeviceInformationPrivate *priv = OnvifDeviceInformation__get_instance_private (ONVIF_DEVICE_INFORMATION(self));
    return priv->model;
}

char * OnvifDeviceInformation__get_firmwareVersion(OnvifDeviceInformation *self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_INFORMATION (self), NULL);
    OnvifDeviceInformationPrivate *priv = OnvifDeviceInformation__get_instance_private (ONVIF_DEVICE_INFORMATION(self));
    return priv->firmwareVersion;
}

char * OnvifDeviceInformation__get_serialNumber(OnvifDeviceInformation *self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_INFORMATION (self), NULL);
    OnvifDeviceInformationPrivate *priv = OnvifDeviceInformation__get_instance_private (ONVIF_DEVICE_INFORMATION(self));
    return priv->serialNumber;
}

char * OnvifDeviceInformation__get_hardwareId(OnvifDeviceInformation *self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_INFORMATION (self), NULL);
    OnvifDeviceInformationPrivate *priv = OnvifDeviceInformation__get_instance_private (ONVIF_DEVICE_INFORMATION(self));
    return priv->hardwareId;
}