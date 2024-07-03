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

OnvifDeviceInterface * OnvifDeviceInterface__create(struct tt__NetworkInterface * interface);
void OnvifDeviceInterface__destroy(OnvifDeviceInterface * interface);

static void
OnvifDeviceInterfaces__reset (OnvifDeviceInterfaces *self);

static void
OnvifDeviceInterfaces__construct(SoapObject * obj, gpointer ptr){
    OnvifDeviceInterfaces *self = ONVIF_DEVICE_INTERFACES(obj);
    struct _tds__GetNetworkInterfacesResponse * resp = ptr;
    OnvifDeviceInterfacesPrivate *priv = OnvifDeviceInterfaces__get_instance_private (self);

    OnvifDeviceInterfaces__reset(self);

    if(!resp || resp->__sizeNetworkInterfaces <= 0){
        return;
    }
    
    priv->interfaces = malloc(sizeof (OnvifDeviceInterface *) * resp->__sizeNetworkInterfaces);
    for(int i=0;i<resp->__sizeNetworkInterfaces;i++){
        struct tt__NetworkInterface interf = resp->NetworkInterfaces[i];

        //TODO IPv6
        priv->count++;
        priv->interfaces[priv->count-1] = OnvifDeviceInterface__create(&interf);
    }
}

static void
OnvifDeviceInterfaces__reset (OnvifDeviceInterfaces *self)
{
    OnvifDeviceInterfacesPrivate *priv = OnvifDeviceInterfaces__get_instance_private (self);
    if(priv->interfaces){
        for(int i=0;i<priv->count;i++){
            OnvifDeviceInterface__destroy(priv->interfaces[i]);
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
    return g_object_new (ONVIF_TYPE_DEVICE_INTERFACES, "data", resp, NULL);
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

OnvifDeviceInterface * OnvifDeviceInterface__create(struct tt__NetworkInterface * interface){
    OnvifDeviceInterface * onvifinterface = malloc(sizeof(OnvifDeviceInterface));
    onvifinterface->enabled = interface->Enabled;
    onvifinterface->token = (char*) malloc(strlen(interface->token)+1);
    strcpy(onvifinterface->token,interface->token);
    
    if(interface->Info){
        onvifinterface->has_info = 1;
        if(interface->Info->Name){
            onvifinterface->name = malloc(strlen(interface->Info->Name)+1);
            strcpy(onvifinterface->name,interface->Info->Name);
        } else {
            onvifinterface->name = NULL;
        }

        if(interface->Info->HwAddress){
            onvifinterface->mac = (char*) malloc(strlen(interface->Info->HwAddress)+1);
            strcpy(onvifinterface->mac,interface->Info->HwAddress);
        } else {
            onvifinterface->mac = NULL;
        }


        if(interface->Info->MTU){
            onvifinterface->mtu = interface->Info->MTU[0];
        } else {
            onvifinterface->mtu = -1;
        }
    } else {
        onvifinterface->name = NULL;
        onvifinterface->mac = NULL;
        onvifinterface->has_info = 0;
    }
    
    //struct tt__IPv4NetworkInterface*     IPv4
    if(interface->IPv4){
        onvifinterface->ipv4_enabled = interface->IPv4->Enabled;
        onvifinterface->ipv4_manual = NULL;
        onvifinterface->ipv4_link_local = NULL;
        onvifinterface->ipv4_from_dhcp = NULL;

        if(interface->IPv4->Config){
            onvifinterface->ipv4_dhcp = interface->IPv4->Config->DHCP;
            onvifinterface->ipv4_manual_count = interface->IPv4->Config->__sizeManual;
            //Manually configured IPs
            if(interface->IPv4->Config->__sizeManual > 0){
                struct tt__PrefixedIPv4Address * manuals = interface->IPv4->Config->Manual;
                for(int a=0;a<onvifinterface->ipv4_manual_count;a++){
                    struct tt__PrefixedIPv4Address manual = manuals[a];
                    onvifinterface->ipv4_manual = realloc(onvifinterface->ipv4_manual,sizeof(char *) * onvifinterface->ipv4_manual_count);
                    if(manual.Address){
                        onvifinterface->ipv4_manual[onvifinterface->ipv4_manual_count-1] = malloc(strlen(manual.Address) + 1);
                        strcpy(onvifinterface->ipv4_manual[onvifinterface->ipv4_manual_count-1],manual.Address);
                    } else {
                        onvifinterface->ipv4_manual[onvifinterface->ipv4_manual_count-1] = malloc(strlen("No IPv4 Manual Address defined") + 1);
                        strcpy(onvifinterface->ipv4_manual[onvifinterface->ipv4_manual_count-1],"No IPv4 Manual Address defined");
                        C_ERROR("No IPv4 Manual Address defined in soap response");
                    }

                }
            }

            //Link local address.
            if(interface->IPv4->Config->LinkLocal){
                onvifinterface->ipv4_link_local = malloc(strlen(interface->IPv4->Config->LinkLocal->Address)+1);
                strcpy(onvifinterface->ipv4_link_local,interface->IPv4->Config->LinkLocal->Address);
            }

            //DHCP IP
            if(interface->IPv4->Config->FromDHCP){
                onvifinterface->ipv4_from_dhcp = malloc(strlen(interface->IPv4->Config->FromDHCP->Address)+1);
                strcpy(onvifinterface->ipv4_from_dhcp,interface->IPv4->Config->FromDHCP->Address);
            }
        } else {
            onvifinterface->ipv4_manual_count = 0;
            onvifinterface->ipv4_dhcp = 0;
        }
    } else {
        C_WARN("No IPv4 defined");
        onvifinterface->ipv4_enabled = 0;
        onvifinterface->ipv4_manual = NULL;
        onvifinterface->ipv4_link_local = NULL;
        onvifinterface->ipv4_from_dhcp = NULL;
        onvifinterface->ipv4_manual_count = 0;
        onvifinterface->ipv4_dhcp = 0;
    }
    return onvifinterface;
}

void OnvifDeviceInterface__destroy(OnvifDeviceInterface * interface){
    free(interface->token);
    if(interface->name)
        free(interface->name);
    if(interface->mac)
        free(interface->mac);
    if(interface->ipv4_manual){
        for(int a=0;a<interface->ipv4_manual_count;a++){
            free(interface->ipv4_manual[a]);
        }
        free(interface->ipv4_manual);
    }
    if(interface->ipv4_link_local)
        free(interface->ipv4_link_local);
    if(interface->ipv4_from_dhcp)
        free(interface->ipv4_from_dhcp);
    
    free(interface);
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
