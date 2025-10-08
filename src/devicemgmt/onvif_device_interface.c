#include "onvif_device_interface_local.h"
#include "onvif_prefix_ip_address_local.h"
#include "clogger.h"

typedef struct _OnvifIPv4Configuration {
    int enabled;
    int dhcp; // DHCP
    int manual_count;
    OnvifPrefixedIPAddress ** manual;
    OnvifPrefixedIPAddress * local;
    OnvifPrefixedIPAddress * fromdhcp;
} OnvifIPv4Configuration;

typedef struct _OnvifIPv6Configuration {
    int enabled;
    int dhcp; // DHCP
    int manual_count;
    OnvifPrefixedIPAddress ** manual;
    int local_count;
    OnvifPrefixedIPAddress ** local;
    int dhcp_count;
    OnvifPrefixedIPAddress ** fromdhcp;
    int ra_count;
    OnvifPrefixedIPAddress ** fromra;
} OnvifIPv6Configuration;

typedef struct _OnvifDeviceInterface {
    char * token; ///< Required attribute.
    int enabled; ///< Required element.

    //Info ///< Optional element.
    char * name;
    char * mac;
    int mtu;

    OnvifIPv4Configuration * ipv4;
    OnvifIPv6Configuration * ipv6;

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
OnvifDeviceInterfaces__reset (OnvifDeviceInterfaces *self){
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
OnvifDeviceInterfaces__dispose (GObject *self){
    OnvifDeviceInterfaces__reset(ONVIF_DEVICE_INTERFACES(self));
    G_OBJECT_CLASS (OnvifDeviceInterfaces__parent_class)->dispose (self);
}

static void
OnvifDeviceInterfaces__class_init (OnvifDeviceInterfacesClass * klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifDeviceInterfaces__dispose;
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifDeviceInterfaces__construct;
}

static void
OnvifDeviceInterfaces__init (OnvifDeviceInterfaces * self){
    OnvifDeviceInterfacesPrivate *priv = OnvifDeviceInterfaces__get_instance_private (ONVIF_DEVICE_INTERFACES(self));
    priv->interfaces = NULL;
    priv->count = 0;
}

OnvifDeviceInterfaces* 
OnvifDeviceInterfaces__new(struct _tds__GetNetworkInterfacesResponse * resp){
    return g_object_new (ONVIF_TYPE_DEVICE_INTERFACES, "data", resp, NULL);
}

int 
OnvifDeviceInterfaces__get_count(OnvifDeviceInterfaces * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_DEVICE_INTERFACES (self), 0);
    OnvifDeviceInterfacesPrivate *priv = OnvifDeviceInterfaces__get_instance_private (ONVIF_DEVICE_INTERFACES(self));
    return priv->count;
}

OnvifDeviceInterface * 
OnvifDeviceInterfaces__get_interface(OnvifDeviceInterfaces * self, int index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_INTERFACES (self), NULL);
    OnvifDeviceInterfacesPrivate *priv = OnvifDeviceInterfaces__get_instance_private (ONVIF_DEVICE_INTERFACES(self));
    if(index >= priv->count){
        return NULL;
    }
    return priv->interfaces[index];
}

static void
OnvifIPv6Configuration__create_prefix_addresses(OnvifPrefixedIPAddress *** addresses, int * count, struct tt__PrefixedIPv6Address * ttaddresses, int ttcount){
    *count = ttcount;
    if(ttcount > 0){
        *addresses = malloc(sizeof(OnvifPrefixedIPAddress *) * ttcount);
        for(int a=0;a<ttcount;a++){
            //tt__PrefixedIPv4Address and tt__PrefixedIPv6Address have the same signature
            *addresses[ttcount-1] = OnvifPrefixedIPAddress__create6(&ttaddresses[a]);
        }
    } else {
        *addresses = NULL;
    }
}

static OnvifIPv6Configuration * 
OnvifIPv6Configuration__create(struct tt__IPv6NetworkInterface *IPv6){
    OnvifIPv6Configuration * ret = NULL;
    if(IPv6){
        ret = malloc(sizeof(OnvifIPv6Configuration));
        ret->enabled = IPv6->Enabled;
        if(IPv6->Config){
            ret->dhcp = IPv6->Config->DHCP;
            OnvifIPv6Configuration__create_prefix_addresses(&ret->manual,&ret->manual_count,IPv6->Config->Manual,IPv6->Config->__sizeManual);
            OnvifIPv6Configuration__create_prefix_addresses(&ret->local,&ret->local_count,IPv6->Config->LinkLocal,IPv6->Config->__sizeLinkLocal);
            OnvifIPv6Configuration__create_prefix_addresses(&ret->fromra,&ret->ra_count,IPv6->Config->FromRA,IPv6->Config->__sizeFromRA);
            OnvifIPv6Configuration__create_prefix_addresses(&ret->fromdhcp,&ret->dhcp_count,IPv6->Config->FromDHCP,IPv6->Config->__sizeFromDHCP);
        } else {
            ret->dhcp = 0;
            ret->manual_count = 0;
            ret->manual = NULL;
            ret->local_count = 0;
            ret->local = NULL;
            ret->ra_count = 0;
            ret->fromra = NULL;
            ret->dhcp_count = 0;
            ret->fromdhcp = NULL;
        }
    }

    return ret;
}

int 
OnvifIPv6Configuration__is_enabled(OnvifIPv6Configuration * self){
    return self->enabled;
}

int 
OnvifIPv6Configuration__is_dhcp(OnvifIPv6Configuration * self){
    return self->dhcp;
}

int 
OnvifIPv6Configuration__get_manual_count(OnvifIPv6Configuration * self){
    return self->manual_count;
}

OnvifPrefixedIPAddress ** 
OnvifIPv6Configuration__get_manuals(OnvifIPv6Configuration * self){
    return self->manual;
}

int 
OnvifIPv6Configuration__get_local_count(OnvifIPv6Configuration * self){
    return self->local_count;
}

OnvifPrefixedIPAddress ** 
OnvifIPv6Configuration__get_local(OnvifIPv6Configuration * self){
    return self->local;
}

OnvifPrefixedIPAddress ** 
OnvifIPv6Configuration__get_fromdhcp(OnvifIPv6Configuration * self){
    return self->fromdhcp;
}

int 
OnvifIPv6Configuration__get_fromdhcp_count(OnvifIPv6Configuration * self){
    return self->dhcp_count;
}

OnvifPrefixedIPAddress ** 
OnvifIPv6Configuration__get_fromra(OnvifIPv6Configuration * self){
    return self->fromra;
}

int 
OnvifIPv6Configuration__get_fromra_count(OnvifIPv6Configuration * self){
    return self->ra_count;
}

static void destroy_addresslist(OnvifPrefixedIPAddress ** list, int * count){
    for(int i=0;i<*count;i++){
        OnvifPrefixedIPAddress__destroy(list[i]);
    }
    free(list);
    *count = 0;
}

static void 
OnvifIPv6Configuration__destroy(OnvifIPv6Configuration * self){
    self->enabled = 0;
    self->dhcp = 0;

    destroy_addresslist(self->manual,&self->manual_count);
    destroy_addresslist(self->local,&self->local_count);
    destroy_addresslist(self->fromdhcp,&self->dhcp_count);
    destroy_addresslist(self->fromra,&self->ra_count);

    free(self);
}

static OnvifIPv4Configuration * 
OnvifIPv4Configuration__create(struct tt__IPv4NetworkInterface *IPv4){
    OnvifIPv4Configuration * ret = NULL;
    if(IPv4){
        ret = malloc(sizeof(OnvifIPv4Configuration));
        ret->enabled = IPv4->Enabled;
        ret->manual = NULL;
        ret->local = NULL;
        ret->fromdhcp = NULL;

        if(IPv4->Config){
            ret->dhcp = IPv4->Config->DHCP;
            ret->manual_count = IPv4->Config->__sizeManual;
            //Manually configured IPs
            if(ret->manual_count > 0){
                ret->manual = malloc(sizeof(OnvifPrefixedIPAddress *) * ret->manual_count);
                struct tt__PrefixedIPv4Address * manuals = IPv4->Config->Manual;
                for(int a=0;a<ret->manual_count;a++){
                    ret->manual[ret->manual_count-1] = OnvifPrefixedIPAddress__create4(&manuals[a]);
                }
            }

            //Link local address.
            if(IPv4->Config->LinkLocal)
                ret->local = OnvifPrefixedIPAddress__create4(IPv4->Config->LinkLocal);

            //DHCP IP
            if(IPv4->Config->FromDHCP)
                ret->fromdhcp = OnvifPrefixedIPAddress__create4(IPv4->Config->FromDHCP);

        } else {
            ret->manual_count = 0;
            ret->dhcp = 0;
        }
    } else {
        C_WARN("No IPv4 defined");
    }

    return ret;
}

int 
OnvifIPv4Configuration__is_enabled(OnvifIPv4Configuration * self){
    return self->enabled;
}

int 
OnvifIPv4Configuration__is_dhcp(OnvifIPv4Configuration * self){
    return self->dhcp;
}

int 
OnvifIPv4Configuration__get_manual_count(OnvifIPv4Configuration * self){
    return self->manual_count;
}

OnvifPrefixedIPAddress * 
OnvifIPv4Configuration__get_manual(OnvifIPv4Configuration * self, int index){
    if(index < 0 || index >= self->manual_count){
        C_ERROR("IPv4Configuration manual address out-of-bounds.");
        return NULL;
    }

    return self->manual[index];
}

OnvifPrefixedIPAddress ** 
OnvifIPv4Configuration__get_manuals(OnvifIPv4Configuration * self){
    return self->manual;
}

OnvifPrefixedIPAddress * 
OnvifIPv4Configuration__get_local(OnvifIPv4Configuration * self){
    return self->local;
}

OnvifPrefixedIPAddress * 
OnvifIPv4Configuration__get_fromdhcp(OnvifIPv4Configuration * self){
    return self->fromdhcp;
}

static void 
OnvifIPv4Configuration__destroy(OnvifIPv4Configuration * self){
    self->enabled = 0;
    self->dhcp = 0;

    destroy_addresslist(self->manual,&self->manual_count);

    if(self->local)
        OnvifPrefixedIPAddress__destroy(self->local);
    if(self->fromdhcp)
        OnvifPrefixedIPAddress__destroy(self->fromdhcp); 
        
    free(self);
}

OnvifDeviceInterface * 
OnvifDeviceInterface__create(struct tt__NetworkInterface * interface){
    OnvifDeviceInterface * onvifinterface = malloc(sizeof(OnvifDeviceInterface));
    onvifinterface->enabled = interface->Enabled;
    onvifinterface->token = (char*) malloc(strlen(interface->token)+1);
    strcpy(onvifinterface->token,interface->token);
    
    if(interface->Info){
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
        onvifinterface->mtu = -1;
        C_WARN("NetworkInterface has no Info defined.");
    }
    
    onvifinterface->ipv4 = OnvifIPv4Configuration__create(interface->IPv4);
    onvifinterface->ipv6 = OnvifIPv6Configuration__create(interface->IPv6);


    return onvifinterface;
}

void 
OnvifDeviceInterface__destroy(OnvifDeviceInterface * interface){
    if(interface->token)
        free(interface->token);
    interface->enabled = 0;
    if(interface->name)
        free(interface->name);
    if(interface->mac)
        free(interface->mac);
    interface->mtu = 0;

    if(interface->ipv4)
        OnvifIPv4Configuration__destroy(interface->ipv4);

    if(interface->ipv6)
        OnvifIPv6Configuration__destroy(interface->ipv6);
    
    free(interface);
}

char * 
OnvifDeviceInterface__get_token(OnvifDeviceInterface * self){
    return self->token;
}

int 
OnvifDeviceInterface__get_enabled(OnvifDeviceInterface * self){
    return self->enabled;
}

char * 
OnvifDeviceInterface__get_name(OnvifDeviceInterface * self){
    return self->name;
}

char * 
OnvifDeviceInterface__get_mac(OnvifDeviceInterface * self){
    return self->mac;
}

int 
OnvifDeviceInterface__get_mtu(OnvifDeviceInterface * self){
    return self->mtu;
}

OnvifIPv4Configuration * 
OnvifDeviceInterface__get_ipv4(OnvifDeviceInterface * self){
    return self->ipv4;
}

OnvifIPv6Configuration *
OnvifDeviceInterface__get_ipv6(OnvifDeviceInterface * self){
    return self->ipv6;
}
