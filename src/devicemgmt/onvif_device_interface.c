#include "onvif_device_interface_local.h"
#include "clogger.h"

typedef struct _OnvifDeviceInterface {
    char * token; ///< Required attribute.
    int enabled; ///< Required element.

    //Info ///< Optional element.
    int has_info;
    char * name;
    char * mac;
    int mtu;

    //IPv4 ///< Optional element.
    //__sizeManual
    int ipv4_enabled;
    int ipv4_dhcp; // DHCP
    int ipv4_manual_count;
    char ** ipv4_manual; // Manual
    char * ipv4_link_local; // LinkLocal
    char * ipv4_from_dhcp; // FromDHCP

    //TODO Support IPv6

} OnvifDeviceInterface;

typedef struct {
    int count;
    OnvifDeviceInterface ** interfaces;
} OnvifDeviceInterfacesPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifDeviceInterfaces, OnvifDeviceInterfaces_, SOAP_TYPE_OBJECT)

static void
OnvifDeviceInterfaces__reset (OnvifDeviceInterfaces *self);

static void
OnvifDeviceInterfaces__construct(SoapObject * obj, gpointer ptr){
    OnvifDeviceInterfaces *self = ONVIF_DEVICE_INTERFACES(obj);
    struct _tds__GetNetworkInterfacesResponse * resp = ptr;
    OnvifDeviceInterfacesPrivate *priv = OnvifDeviceInterfaces__get_instance_private (self);

    OnvifDeviceInterfaces__reset(self);

    if(!resp){
        return;
    }

    priv->interfaces = malloc(0);
    int i;
    for(i=0;i<resp->__sizeNetworkInterfaces;i++){
        struct tt__NetworkInterface interf = resp->NetworkInterfaces[i];
        OnvifDeviceInterface * onvifinterface = malloc(sizeof(OnvifDeviceInterface));
        onvifinterface->enabled = interf.Enabled;
        onvifinterface->token = (char*) malloc(strlen(interf.token)+1);
        strcpy(onvifinterface->token,interf.token);
        
        if(interf.Info){
            onvifinterface->has_info = 1;
            if(interf.Info->Name){
                onvifinterface->name = malloc(strlen(interf.Info->Name)+1);
                strcpy(onvifinterface->name,interf.Info->Name);
            } else {
                onvifinterface->name = NULL;
            }

            if(interf.Info->HwAddress){
                onvifinterface->mac = (char*) malloc(strlen(interf.Info->HwAddress)+1);
                strcpy(onvifinterface->mac,interf.Info->HwAddress);
            } else {
                onvifinterface->mac = NULL;
            }


            if(interf.Info->MTU){
                onvifinterface->mtu = interf.Info->MTU[0];
            } else {
                onvifinterface->mtu = -1;
            }
        } else {
            onvifinterface->name = NULL;
            onvifinterface->mac = NULL;
            onvifinterface->has_info = 0;
        }
        
        //struct tt__IPv4NetworkInterface*     IPv4
        if(interf.IPv4){
            onvifinterface->ipv4_enabled = interf.IPv4->Enabled;
            onvifinterface->ipv4_dhcp = interf.IPv4->Config->DHCP;
            onvifinterface->ipv4_manual_count = interf.IPv4->Config->__sizeManual;
            onvifinterface->ipv4_manual = NULL;
            onvifinterface->ipv4_link_local = NULL;
            onvifinterface->ipv4_from_dhcp = NULL;

            //Manually configured IPs
            if(interf.IPv4->Config->__sizeManual > 0){
                struct tt__PrefixedIPv4Address * manuals = interf.IPv4->Config->Manual;
                for(int a=0;a<onvifinterface->ipv4_manual_count;a++){
                    struct tt__PrefixedIPv4Address manual = manuals[a];
                    onvifinterface->ipv4_manual = realloc(onvifinterface->ipv4_manual,sizeof(char *) * onvifinterface->ipv4_manual_count);
                    onvifinterface->ipv4_manual[onvifinterface->ipv4_manual_count-1] = malloc(strlen(manual.Address) + 1);
                    strcpy(onvifinterface->ipv4_manual[onvifinterface->ipv4_manual_count-1],manual.Address);
                }
            }

            //Link local address.
            if(interf.IPv4->Config->LinkLocal){
                onvifinterface->ipv4_link_local = malloc(strlen(interf.IPv4->Config->LinkLocal->Address)+1);
                strcpy(onvifinterface->ipv4_link_local,interf.IPv4->Config->LinkLocal->Address);
            }

            //DHCP IP
            if(interf.IPv4->Config->FromDHCP){
                onvifinterface->ipv4_from_dhcp = malloc(strlen(interf.IPv4->Config->FromDHCP->Address)+1);
                strcpy(onvifinterface->ipv4_from_dhcp,interf.IPv4->Config->FromDHCP->Address);
            }
        } else {
            onvifinterface->ipv4_enabled = 0;
            onvifinterface->ipv4_manual = NULL;
            onvifinterface->ipv4_link_local = NULL;
            onvifinterface->ipv4_from_dhcp = NULL;

        }

        //TODO IPv6

        priv->count++;
        priv->interfaces = realloc (priv->interfaces, sizeof (OnvifDeviceInterface *) * priv->count);
        priv->interfaces[priv->count-1] = onvifinterface;
    }
}

static void
OnvifDeviceInterfaces__reset (OnvifDeviceInterfaces *self)
{
    OnvifDeviceInterfacesPrivate *priv = OnvifDeviceInterfaces__get_instance_private (self);
    if(priv->interfaces){
        for(int i=0;i<priv->count;i++){
            free(priv->interfaces[i]->token);
            if(priv->interfaces[i]->name)
                free(priv->interfaces[i]->name);
            if(priv->interfaces[i]->mac)
                free(priv->interfaces[i]->mac);
            if(priv->interfaces[i]->ipv4_manual){
                for(int a=0;a<priv->interfaces[i]->ipv4_manual_count;a++){
                    free(priv->interfaces[i]->ipv4_manual[a]);
                }
                free(priv->interfaces[i]->ipv4_manual);
            }
            if(priv->interfaces[i]->ipv4_link_local)
                free(priv->interfaces[i]->ipv4_link_local);
            if(priv->interfaces[i]->ipv4_from_dhcp)
                free(priv->interfaces[i]->ipv4_from_dhcp);
            
            free(priv->interfaces[i]);
        }
        free(priv->interfaces);
        priv->interfaces = NULL;
        priv->count = 0;
    }
}

static void
OnvifDeviceInterfaces__dispose (GObject *self)
{
    OnvifDeviceInterfaces__reset(ONVIF_DEVICE_INTERFACES(self));
    G_OBJECT_CLASS (OnvifDeviceInterfaces__parent_class)->dispose (self);
}

static void
OnvifDeviceInterfaces__class_init (OnvifDeviceInterfacesClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifDeviceInterfaces__dispose;
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifDeviceInterfaces__construct;
}

static void
OnvifDeviceInterfaces__init (OnvifDeviceInterfaces * self)
{
    OnvifDeviceInterfacesPrivate *priv = OnvifDeviceInterfaces__get_instance_private (ONVIF_DEVICE_INTERFACES(self));
    priv->interfaces = NULL;
    priv->count = 0;
}

OnvifDeviceInterfaces* OnvifDeviceInterfaces__new(struct _tds__GetNetworkInterfacesResponse * resp){
    return g_object_new (ONVIF_TYPE_DEVICE_INTERFACES, "soap", resp, NULL);
}

int OnvifDeviceInterfaces__get_count(OnvifDeviceInterfaces * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_DEVICE_INTERFACES (self), 0);
    OnvifDeviceInterfacesPrivate *priv = OnvifDeviceInterfaces__get_instance_private (ONVIF_DEVICE_INTERFACES(self));
    return priv->count;
}

OnvifDeviceInterface * OnvifDeviceInterfaces__get_interface(OnvifDeviceInterfaces * self, int index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_INTERFACES (self), NULL);
    OnvifDeviceInterfacesPrivate *priv = OnvifDeviceInterfaces__get_instance_private (ONVIF_DEVICE_INTERFACES(self));
    if(index >= priv->count){
        return NULL;
    }
    return priv->interfaces[index];
}


char * OnvifDeviceInterface__get_token(OnvifDeviceInterface * self){
    return self->token;
}

int OnvifDeviceInterface__get_enabled(OnvifDeviceInterface * self){
    return self->enabled;
}

int OnvifDeviceInterface__has_info(OnvifDeviceInterface * self){
    return self->has_info;
}

char * OnvifDeviceInterface__get_name(OnvifDeviceInterface * self){
    return self->name;
}

char * OnvifDeviceInterface__get_mac(OnvifDeviceInterface * self){
    return self->mac;
}

int OnvifDeviceInterface__get_mtu(OnvifDeviceInterface * self){
    return self->mtu;
}

int OnvifDeviceInterface__is_ipv4_enabled(OnvifDeviceInterface * self){
    return self->ipv4_enabled;
}

int OnvifDeviceInterface__get_ipv4_dhcp(OnvifDeviceInterface * self){
    return self->ipv4_dhcp;
}

int OnvifDeviceInterface__get_ipv4_manual_count(OnvifDeviceInterface * self){
    return self->ipv4_manual_count;
}

char ** OnvifDeviceInterface__get_ipv4_manual(OnvifDeviceInterface * self){
    return self->ipv4_manual;
}

char * OnvifDeviceInterface__get_ipv4_link_local(OnvifDeviceInterface * self){
    return self->ipv4_link_local;
}

char * OnvifDeviceInterface__get_ipv4_from_dhcp(OnvifDeviceInterface * self){
    return self->ipv4_from_dhcp;
}
