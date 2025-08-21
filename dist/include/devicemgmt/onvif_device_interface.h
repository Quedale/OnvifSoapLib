#ifndef ONVIF_DEV_INTERFACE_H_ 
#define ONVIF_DEV_INTERFACE_H_

#include "../shard_export.h"
#include "../SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICE_INTERFACES OnvifDeviceInterfaces__get_type()
G_DECLARE_FINAL_TYPE (OnvifDeviceInterfaces, OnvifDeviceInterfaces_, ONVIF, DEVICE_INTERFACES, SoapObject)

typedef struct _OnvifIPv4Configuration OnvifIPv4Configuration;
typedef struct _OnvifIPv6Configuration OnvifIPv6Configuration;
typedef struct _OnvifPrefixedIPAddress OnvifPrefixedIPAddress;
typedef struct _OnvifDeviceInterface OnvifDeviceInterface;

struct _OnvifDeviceInterfaces {
    SoapObject parent_instance;
};


struct _OnvifDeviceInterfacesClass {
    SoapObjectClass parent_class;
};

SHARD_EXPORT int OnvifDeviceInterfaces__get_count(OnvifDeviceInterfaces * self);
SHARD_EXPORT OnvifDeviceInterface * OnvifDeviceInterfaces__get_interface(OnvifDeviceInterfaces * self, int index);

SHARD_EXPORT char * OnvifDeviceInterface__get_token(OnvifDeviceInterface * self);
SHARD_EXPORT int OnvifDeviceInterface__get_enabled(OnvifDeviceInterface * self);
SHARD_EXPORT char * OnvifDeviceInterface__get_name(OnvifDeviceInterface * self);
SHARD_EXPORT char * OnvifDeviceInterface__get_mac(OnvifDeviceInterface * self);
SHARD_EXPORT int OnvifDeviceInterface__get_mtu(OnvifDeviceInterface * self);
SHARD_EXPORT OnvifIPv4Configuration * OnvifDeviceInterface__get_ipv4(OnvifDeviceInterface * self);
SHARD_EXPORT OnvifIPv6Configuration * OnvifDeviceInterface__get_ipv6(OnvifDeviceInterface * self);

int OnvifIPv6Configuration__is_enabled(OnvifIPv6Configuration * self);
int OnvifIPv6Configuration__is_dhcp(OnvifIPv6Configuration * self);
int OnvifIPv6Configuration__get_manual_count(OnvifIPv6Configuration * self);
int OnvifIPv6Configuration__get_local_count(OnvifIPv6Configuration * self);
int OnvifIPv6Configuration__get_fromdhcp_count(OnvifIPv6Configuration * self);
int OnvifIPv6Configuration__get_fromra_count(OnvifIPv6Configuration * self);
OnvifPrefixedIPAddress ** OnvifIPv6Configuration__get_manuals(OnvifIPv6Configuration * self);
OnvifPrefixedIPAddress ** OnvifIPv6Configuration__get_local(OnvifIPv6Configuration * self);
OnvifPrefixedIPAddress ** OnvifIPv6Configuration__get_fromdhcp(OnvifIPv6Configuration * self);
OnvifPrefixedIPAddress ** OnvifIPv6Configuration__get_fromra(OnvifIPv6Configuration * self);

int OnvifIPv4Configuration__is_enabled(OnvifIPv4Configuration * self);
int OnvifIPv4Configuration__is_dhcp(OnvifIPv4Configuration * self);
int OnvifIPv4Configuration__get_manual_count(OnvifIPv4Configuration * self);
OnvifPrefixedIPAddress ** OnvifIPv4Configuration__get_manuals(OnvifIPv4Configuration * self);
OnvifPrefixedIPAddress * OnvifIPv4Configuration__get_manual(OnvifIPv4Configuration * self, int index);
OnvifPrefixedIPAddress * OnvifIPv4Configuration__get_local(OnvifIPv4Configuration * self);
OnvifPrefixedIPAddress * OnvifIPv4Configuration__get_fromdhcp(OnvifIPv4Configuration * self);

char * OnvifPrefixedIPAddress__get_address(OnvifPrefixedIPAddress * self);
int OnvifPrefixedIPAddress__get_prefix(OnvifPrefixedIPAddress * self);

G_END_DECLS


#endif
