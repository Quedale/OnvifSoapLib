#include "onvif_device_interface_local.h"
#include "clogger.h"

typedef struct _OnvifPrefixedIPv4Address {
    char * address;
    int prefix;
} OnvifPrefixedIPv4Address;

typedef struct _OnvifIPv4Configuration {
    int enabled;
    int dhcp; // DHCP
    int manual_count;
    OnvifPrefixedIPv4Address ** manual;
    OnvifPrefixedIPv4Address * local;
    OnvifPrefixedIPv4Address * fromdhcp;
} OnvifIPv4Configuration;

typedef struct _OnvifDeviceInterface {
    char * token; ///< Required attribute.
    int enabled; ///< Required element.

    //Info ///< Optional element.
    char * name;
    char * mac;
    int mtu;

    OnvifIPv4Configuration * ipv4;
    // OnvifIPv6Configuration * ipv6;
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

OnvifPrefixedIPv4Address * OnvifPrefixedIPv4Address__create(struct tt__PrefixedIPv4Address * addr){
    if(!addr){
        return NULL;
    }

    OnvifPrefixedIPv4Address * ret = malloc(sizeof(OnvifPrefixedIPv4Address));
    if(addr->Address){
        ret->address = malloc(strlen(addr->Address) + 1);
        strcpy(ret->address,addr->Address);
    } else {
        ret->address = malloc(strlen("No IPv4 Manual Address defined") + 1);
        strcpy(ret->address,"No IPv4 Manual Address defined");
        C_ERROR("No IPv4 Manual Address defined in soap response");
    }

    ret->prefix = addr->PrefixLength;
    return ret;
}

char * OnvifPrefixedIPv4Address__get_address(OnvifPrefixedIPv4Address * self){
    return self->address;
}

int OnvifPrefixedIPv4Address__get_prefix(OnvifPrefixedIPv4Address * self){
    return self->prefix;
}

void OnvifPrefixedIPv4Address__destroy(OnvifPrefixedIPv4Address * self){
    if(self->address)
        free(self->address);
    self->prefix = 0;
}

OnvifIPv4Configuration * OnvifIPv4Configuration__create(struct tt__IPv4NetworkInterface *IPv4){
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
                ret->manual = malloc(sizeof(OnvifPrefixedIPv4Address) * ret->manual_count);
                struct tt__PrefixedIPv4Address * manuals = IPv4->Config->Manual;
                for(int a=0;a<ret->manual_count;a++){
                    ret->manual[ret->manual_count-1] = OnvifPrefixedIPv4Address__create(&manuals[a]);

                }
            }

            //Link local address.
            if(IPv4->Config->LinkLocal)
                ret->local = OnvifPrefixedIPv4Address__create(IPv4->Config->LinkLocal);

            //DHCP IP
            if(IPv4->Config->FromDHCP)
                ret->fromdhcp = OnvifPrefixedIPv4Address__create(IPv4->Config->FromDHCP);

        } else {
            ret->manual_count = 0;
            ret->dhcp = 0;
        }
    } else {
        C_WARN("No IPv4 defined");
    }

    return ret;
}

int OnvifIPv4Configuration__is_enabled(OnvifIPv4Configuration * self){
    return self->enabled;
}

int OnvifIPv4Configuration__is_dhcp(OnvifIPv4Configuration * self){
    return self->dhcp;
}

int OnvifIPv4Configuration__get_manual_count(OnvifIPv4Configuration * self){
    return self->manual_count;
}

OnvifPrefixedIPv4Address * OnvifIPv4Configuration__get_manual(OnvifIPv4Configuration * self, int index){
    if(index < 0 || index >= self->manual_count){
        C_ERROR("IPv4Configuration manual address out-of-bounds.");
        return NULL;
    }

    return self->manual[index];
}

OnvifPrefixedIPv4Address * OnvifIPv4Configuration__get_local(OnvifIPv4Configuration * self){
    return self->local;
}

OnvifPrefixedIPv4Address * OnvifIPv4Configuration__get_fromdhcp(OnvifIPv4Configuration * self){
    return self->fromdhcp;
}

void OnvifIPv4Configuration__destroy(OnvifIPv4Configuration * self){
    self->enabled = 0;
    self->dhcp = 0;

    if(self->manual){
        for(int i=0;i<self->manual_count;i++){
            OnvifPrefixedIPv4Address__destroy(self->manual[i]);
        }
    }
    self->manual_count = 0;

    if(self->local)
        OnvifPrefixedIPv4Address__destroy(self->local);
    if(self->fromdhcp)
        OnvifPrefixedIPv4Address__destroy(self->fromdhcp);   
}

OnvifDeviceInterface * OnvifDeviceInterface__create(struct tt__NetworkInterface * interface){
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
        C_WARN("NetworkInterface has no Info defined.");
    }
    
    onvifinterface->ipv4 = OnvifIPv4Configuration__create(interface->IPv4);



    return onvifinterface;
}

void OnvifDeviceInterface__destroy(OnvifDeviceInterface * interface){
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

    //TODO Destroy ipv4
    
    free(interface);
}

char * OnvifDeviceInterface__get_token(OnvifDeviceInterface * self){
    return self->token;
}

int OnvifDeviceInterface__get_enabled(OnvifDeviceInterface * self){
    return self->enabled;
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

OnvifIPv4Configuration * OnvifDeviceInterface__get_ipv4(OnvifDeviceInterface * self){
    return self->ipv4;
}

