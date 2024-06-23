#ifndef ONVIF_DEV_INTERFACE_H_ 
#define ONVIF_DEV_INTERFACE_H_

#include "shard_export.h"
#include "SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICE_INTERFACES OnvifDeviceInterfaces__get_type()
G_DECLARE_FINAL_TYPE (OnvifDeviceInterfaces, OnvifDeviceInterfaces_, ONVIF, DEVICE_INTERFACES, SoapObject)

typedef struct _OnvifDeviceInterface OnvifDeviceInterface;

struct _OnvifDeviceInterfaces
{
    SoapObject parent_instance;
};


struct _OnvifDeviceInterfacesClass
{
    SoapObjectClass parent_class;
};

SHARD_EXPORT int OnvifDeviceInterfaces__get_count(OnvifDeviceInterfaces * self);
SHARD_EXPORT OnvifDeviceInterface * OnvifDeviceInterfaces__get_interface(OnvifDeviceInterfaces * self, int index);

SHARD_EXPORT char * OnvifDeviceInterface__get_token(OnvifDeviceInterface * self);
SHARD_EXPORT int OnvifDeviceInterface__get_enabled(OnvifDeviceInterface * self);
SHARD_EXPORT int OnvifDeviceInterface__has_info(OnvifDeviceInterface * self);
SHARD_EXPORT char * OnvifDeviceInterface__get_name(OnvifDeviceInterface * self);
SHARD_EXPORT char * OnvifDeviceInterface__get_mac(OnvifDeviceInterface * self);
SHARD_EXPORT int OnvifDeviceInterface__get_mtu(OnvifDeviceInterface * self);
SHARD_EXPORT int OnvifDeviceInterface__is_ipv4_enabled(OnvifDeviceInterface * self);
SHARD_EXPORT int OnvifDeviceInterface__get_ipv4_dhcp(OnvifDeviceInterface * self);
SHARD_EXPORT int OnvifDeviceInterface__get_ipv4_manual_count(OnvifDeviceInterface * self);
SHARD_EXPORT char ** OnvifDeviceInterface__get_ipv4_manual(OnvifDeviceInterface * self);
SHARD_EXPORT char * OnvifDeviceInterface__get_ipv4_link_local(OnvifDeviceInterface * self);
SHARD_EXPORT char * OnvifDeviceInterface__get_ipv4_from_dhcp(OnvifDeviceInterface * self);


G_END_DECLS


#endif