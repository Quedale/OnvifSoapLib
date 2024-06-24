#include "onvif_device_capabilities_local.h"
#include "clogger.h"

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

typedef struct _OnvifSystemCapabilities {
    gboolean discoveryResolve;
    gboolean discoveryBye;
    gboolean remoteDiscovery;
    gboolean systemBackup;
    gboolean systemLogging;
    gboolean firmwareUpgrade;
    int major_version;
    int minor_version;
    //Extension
} OnvifSystemCapabilities;

typedef struct _OnvifDeviceCapabilities {
    char * xaddr;
    //Network
    OnvifSystemCapabilities * system;
    //IO
    //Security
} OnvifDeviceCapabilities;

typedef struct {
    //Analytics
    OnvifDeviceCapabilities * device;
    //Events
    //Imaging
    OnvifMedia *media;
    //PTZ
    //Extension
} OnvifCapabilitiesPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifCapabilities, OnvifCapabilities_, SOAP_TYPE_OBJECT)
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void OnvifCapabilities__reset (OnvifCapabilities *self);
OnvifMedia * OnvifMedia__create(struct tt__MediaCapabilities * caps);
void OnvifMedia__destroy(OnvifMedia * self);
OnvifDeviceCapabilities * OnvifDeviceCapabilities__create(struct tt__DeviceCapabilities * caps);
void OnvifDeviceCapabilities__destroy(OnvifDeviceCapabilities * self);
OnvifSystemCapabilities * OnvifSystemCapabilities__create(struct tt__SystemCapabilities * caps);
void OnvifSystemCapabilities__destroy(OnvifSystemCapabilities * self);

static void
OnvifCapabilities__set_soap(OnvifCapabilities * self, struct _tds__GetCapabilitiesResponse * response){
    OnvifCapabilitiesPrivate *priv = OnvifCapabilities__get_instance_private (ONVIF_DEVICE_CAPABILITIES(self));

    OnvifCapabilities__reset(self);

    if(!response || !response->Capabilities){
        return;
    }

     priv->media = OnvifMedia__create(response->Capabilities->Media);
     priv->device = OnvifDeviceCapabilities__create(response->Capabilities->Device);
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
OnvifCapabilities__reset (OnvifCapabilities *self){
    OnvifCapabilitiesPrivate *priv = OnvifCapabilities__get_instance_private (self);
    OnvifMedia__destroy(priv->media);
    priv->media = NULL;
    OnvifDeviceCapabilities__destroy(priv->device);
    priv->device = NULL;
}

static void
OnvifCapabilities__dispose (GObject *self)
{
    OnvifCapabilities__reset(ONVIF_DEVICE_CAPABILITIES(self));

    G_OBJECT_CLASS (OnvifCapabilities__parent_class)->dispose (self);
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
    priv->device = NULL;

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

OnvifDeviceCapabilities * OnvifCapabilities__get_device(OnvifCapabilities * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_CAPABILITIES (self), NULL);
    OnvifCapabilitiesPrivate *priv = OnvifCapabilities__get_instance_private (ONVIF_DEVICE_CAPABILITIES(self));
    return priv->device;
}

/*
    Start of OnvifMedia definitions
*/
OnvifMedia * OnvifMedia__create(struct tt__MediaCapabilities * caps){
    OnvifMedia* self =  (OnvifMedia *) malloc(sizeof(OnvifMedia));
    self->xaddr = malloc(strlen(caps->XAddr)+1);
    strcpy(self->xaddr,caps->XAddr);

    return self;
}

void OnvifMedia__destroy(OnvifMedia * self){
    if(self){
        free(self->xaddr);
        self->xaddr = NULL;
        free(self);
    }
}


char * OnvifMedia__get_address(OnvifMedia * self){
    return self->xaddr;
}

/*
    Start of OnvifDeviceCapabilities definitions
*/
OnvifDeviceCapabilities * OnvifDeviceCapabilities__create(struct tt__DeviceCapabilities * caps){
    OnvifDeviceCapabilities* self =  (OnvifDeviceCapabilities *) malloc(sizeof(OnvifDeviceCapabilities));
    self->xaddr = malloc(strlen(caps->XAddr)+1);
    strcpy(self->xaddr,caps->XAddr);

    self->system = OnvifSystemCapabilities__create(caps->System);

    return self;
}

void OnvifDeviceCapabilities__destroy(OnvifDeviceCapabilities * self){
    if(self){
        free(self->xaddr);
        self->xaddr = NULL;
        OnvifSystemCapabilities__destroy(self->system);
        self->system = NULL;
        free(self);
    }
}

/*
    Start of OnvifSystemCapabilities definitions
*/
OnvifSystemCapabilities * OnvifDeviceCapabilities__get_system(OnvifDeviceCapabilities * self){
    return self->system;
}

OnvifSystemCapabilities * OnvifSystemCapabilities__create(struct tt__SystemCapabilities * caps){
    OnvifSystemCapabilities* self =  (OnvifSystemCapabilities *) malloc(sizeof(OnvifSystemCapabilities));

    switch(caps->DiscoveryResolve){
        case xsd__boolean__true_:
            self->discoveryResolve = 1;
            break;
        case xsd__boolean__false_:
            self->discoveryResolve = 0;
            break;
        default:
            self->discoveryResolve = 0;
            C_WARN("default Capabilities->DeviceCapabilities->SystemCapabilities->DiscoveryResolve %d",caps->DiscoveryResolve);
    }

    switch(caps->DiscoveryBye){
        case xsd__boolean__true_:
            self->discoveryBye = 1;
            break;
        case xsd__boolean__false_:
            self->discoveryBye = 0;
            break;
        default:
            self->discoveryBye = 0;
            C_WARN("default Capabilities->DeviceCapabilities->SystemCapabilities->DiscoveryBye %d",caps->DiscoveryBye);
    }

    switch(caps->RemoteDiscovery){
        case xsd__boolean__true_:
            self->remoteDiscovery = 1;
            break;
        case xsd__boolean__false_:
            self->remoteDiscovery = 0;
            break;
        default:
            self->remoteDiscovery = 0;
            C_WARN("default Capabilities->DeviceCapabilities->SystemCapabilities->RemoteDiscovery %d",caps->RemoteDiscovery);
    }

    switch(caps->SystemBackup){
        case xsd__boolean__true_:
            self->systemBackup = 1;
            break;
        case xsd__boolean__false_:
            self->systemBackup = 0;
            break;
        default:
            self->systemBackup = 0;
            C_WARN("default Capabilities->DeviceCapabilities->SystemCapabilities->SystemBackup %d",caps->SystemBackup);
    }

    switch(caps->SystemLogging){
        case xsd__boolean__true_:
            self->systemLogging = 1;
            break;
        case xsd__boolean__false_:
            self->systemLogging = 0;
            break;
        default:
            self->systemLogging = 0;
            C_WARN("default Capabilities->DeviceCapabilities->SystemCapabilities->SystemLogging %d",caps->SystemLogging);
    }

    switch(caps->FirmwareUpgrade){
        case xsd__boolean__true_:
            self->firmwareUpgrade = 1;
            break;
        case xsd__boolean__false_:
            self->firmwareUpgrade = 0;
            break;
        default:
            self->firmwareUpgrade = 0;
            C_WARN("default Capabilities->DeviceCapabilities->SystemCapabilities->FirmwareUpgrade %d",caps->FirmwareUpgrade);
    }

    self->major_version = caps->SupportedVersions->Major;
    self->minor_version = caps->SupportedVersions->Minor;
    
    return self;
}

void OnvifSystemCapabilities__destroy(OnvifSystemCapabilities * self){
    if(self){

        free(self);
    }
}

gboolean OnvifSystemCapabilities__is_discoveryResolve(OnvifSystemCapabilities * self){
    return self->discoveryResolve;
}

gboolean OnvifSystemCapabilities__is_discoveryBye(OnvifSystemCapabilities * self){
    return self->discoveryBye;
}

gboolean OnvifSystemCapabilities__is_remoteDiscovery(OnvifSystemCapabilities * self){
    return self->remoteDiscovery;
}

gboolean OnvifSystemCapabilities__is_systemBackup(OnvifSystemCapabilities * self){
    return self->systemBackup;
}

gboolean OnvifSystemCapabilities__is_systemLogging(OnvifSystemCapabilities * self){
    return self->systemLogging;
}

gboolean OnvifSystemCapabilities__is_firmwareUpgrade(OnvifSystemCapabilities * self){
    return self->firmwareUpgrade;
}

int OnvifSystemCapabilities__get_majorVersion(OnvifSystemCapabilities * self){
    return self->major_version;
}

int OnvifSystemCapabilities__get_minorVersion(OnvifSystemCapabilities * self){
    return self->minor_version;
}
