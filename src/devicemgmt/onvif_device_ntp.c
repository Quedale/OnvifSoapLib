#include "onvif_device_ntp_local.h"
#include "onvif_prefix_ip_address_local.h"
#include "clogger.h"

typedef struct _OnvifHost {
    OnvifHostType type;
    char * address;
} OnvifHost;

typedef struct {
    gboolean fromdhcp;
    int dhcp_count;
    OnvifHost ** dhcp_hosts;
    int manual_count;
    OnvifHost ** manual_hosts;
} OnvifDeviceNTPPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifDeviceNTP, OnvifDeviceNTP_, SOAP_TYPE_OBJECT)

static void
OnvifDeviceNTP__destroy_hosts(OnvifHost *** hostsptr, int * countptr){
    OnvifHost ** hosts = *hostsptr;
    int count = *countptr;

    if(count > 0){
        for(int i=0;i<count;i++){
            free(hosts[i]->address);
            free(hosts[i]);
        }
        free(hosts);
        hosts = NULL;
        count = 0;
    }
}

static void
OnvifDeviceNTP__reset (OnvifDeviceNTP *self){
    OnvifDeviceNTPPrivate *priv = OnvifDeviceNTP__get_instance_private (self);
    priv->fromdhcp = FALSE;

    OnvifDeviceNTP__destroy_hosts(&priv->dhcp_hosts,&priv->dhcp_count);
    OnvifDeviceNTP__destroy_hosts(&priv->manual_hosts,&priv->manual_count);
}

static void
OnvifDeviceNTP__create_hosts(OnvifHost *** hosts, int * host_count, struct tt__NetworkHost * tthosts, int tthost_count ){
    *host_count = tthost_count;
    *hosts = malloc(sizeof(OnvifHost*) * tthost_count);
    for(int i=0;i<tthost_count;i++){
        OnvifHost * host = malloc(sizeof(OnvifHost));
        *hosts[i] = host;
        struct tt__NetworkHost tthost = tthosts[i];
        char * address;
        switch(tthost.Type){
            case tt__NetworkHostType__IPv4:
                host->type = OnvifHostType__IPv4;
                address = tthost.IPv4Address;
                break;
            case tt__NetworkHostType__IPv6:
                host->type = OnvifHostType__IPv6;
                address = tthost.IPv6Address;
                break;
            case tt__NetworkHostType__DNS:
                host->type = OnvifHostType__DNS;
                address = tthost.DNSname;
                break;
            default:
                host->type = OnvifHostType__ERROR;
                address = NULL;
                break;
        }

        if(address){
            host->address = (char*) malloc(strlen(address)+1);
            strcpy(host->address,address);
        } else {
            host->address = NULL;
        }
    }
}

static void
OnvifDeviceNTP__construct(SoapObject * obj, gpointer ptr){
    OnvifDeviceNTP *self = ONVIF_DEVICE_NTP(obj);
    struct _tds__GetNTPResponse * resp = ptr;
    OnvifDeviceNTPPrivate *priv = OnvifDeviceNTP__get_instance_private (self);

    OnvifDeviceNTP__reset(self);

    if(resp && resp->NTPInformation){
        priv->fromdhcp = resp->NTPInformation->FromDHCP;
        OnvifDeviceNTP__create_hosts(&priv->dhcp_hosts, &priv->dhcp_count, resp->NTPInformation->NTPFromDHCP,resp->NTPInformation->__sizeNTPFromDHCP);
        OnvifDeviceNTP__create_hosts(&priv->manual_hosts, &priv->manual_count, resp->NTPInformation->NTPManual,resp->NTPInformation->__sizeNTPManual);
    }
}

static void
OnvifDeviceNTP__dispose (GObject *self){
    OnvifDeviceNTP__reset(ONVIF_DEVICE_NTP(self));
    G_OBJECT_CLASS (OnvifDeviceNTP__parent_class)->dispose (self);
}

static void
OnvifDeviceNTP__class_init (OnvifDeviceNTPClass * klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifDeviceNTP__dispose;
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifDeviceNTP__construct;
}

static void
OnvifDeviceNTP__init (OnvifDeviceNTP * self){
    OnvifDeviceNTPPrivate *priv = OnvifDeviceNTP__get_instance_private (self);
    priv->fromdhcp = FALSE;
    priv->dhcp_count = 0;
    priv->manual_count = 0;
    priv->dhcp_hosts = NULL;
    priv->manual_hosts = NULL;
}

OnvifDeviceNTP* 
OnvifDeviceNTP__new(struct _tds__GetNTPResponse * resp){
    return g_object_new (ONVIF_TYPE_DEVICE_NTP, "data", resp, NULL);
}

gboolean 
OnvifDeviceNTP__is_fromDHCP(OnvifDeviceNTP * self){
    g_return_val_if_fail(self != NULL, FALSE);
    OnvifDeviceNTPPrivate *priv = OnvifDeviceNTP__get_instance_private (self);
    return priv->fromdhcp;
}

int OnvifDeviceNTP__get_manual_count(OnvifDeviceNTP * self){
    g_return_val_if_fail(self != NULL, 0);
    OnvifDeviceNTPPrivate *priv = OnvifDeviceNTP__get_instance_private (self);
    return priv->manual_count;
}

int OnvifDeviceNTP__get_dhcp_count(OnvifDeviceNTP * self){
    g_return_val_if_fail(self != NULL, 0);
    OnvifDeviceNTPPrivate *priv = OnvifDeviceNTP__get_instance_private (self);
    return priv->dhcp_count;
}

OnvifHost ** OnvifDeviceNTP__get_dhcp_hosts(OnvifDeviceNTP * self){
    g_return_val_if_fail(self != NULL, NULL);
    OnvifDeviceNTPPrivate *priv = OnvifDeviceNTP__get_instance_private (self);
    return priv->dhcp_hosts;
}

OnvifHost ** OnvifDeviceNTP__get_manual_hosts(OnvifDeviceNTP * self){
    g_return_val_if_fail(self != NULL, NULL);
    OnvifDeviceNTPPrivate *priv = OnvifDeviceNTP__get_instance_private (self);
    return priv->manual_hosts;
}

OnvifHostType OnvifHost__getType(OnvifHost * self){
    g_return_val_if_fail(self != NULL, OnvifHostType__ERROR);
    return self->type;
}

char * OnvifHost__getAddress(OnvifHost * self){
    g_return_val_if_fail(self != NULL, NULL);
    return self->address;
}