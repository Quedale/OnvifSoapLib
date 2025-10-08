#ifndef ONVIF_DEV_NTP_H_ 
#define ONVIF_DEV_NTP_H_

#include "../shard_export.h"
#include "../SoapObject.h"
#include "onvif_prefix_ip_address.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICE_NTP OnvifDeviceNTP__get_type()
G_DECLARE_FINAL_TYPE (OnvifDeviceNTP, OnvifDeviceNTP_, ONVIF, DEVICE_NTP, SoapObject)

typedef struct _OnvifHost OnvifHost;
typedef enum {
    OnvifHostType__IPv4,
	OnvifHostType__IPv6,
	OnvifHostType__DNS,
    OnvifHostType__ERROR,
} OnvifHostType;

struct _OnvifDeviceNTP {
    SoapObject parent_instance;
};


struct _OnvifDeviceNTPClass {
    SoapObjectClass parent_class;
};

SHARD_EXPORT gboolean OnvifDeviceNTP__is_fromDHCP(OnvifDeviceNTP * self);
SHARD_EXPORT int OnvifDeviceNTP__get_manual_count(OnvifDeviceNTP * self);
SHARD_EXPORT int OnvifDeviceNTP__get_dhcp_count(OnvifDeviceNTP * self);
SHARD_EXPORT OnvifHost ** OnvifDeviceNTP__get_dhcp_hosts(OnvifDeviceNTP * self);
SHARD_EXPORT OnvifHost ** OnvifDeviceNTP__get_manual_hosts(OnvifDeviceNTP * self);

SHARD_EXPORT OnvifHostType OnvifHost__getType(OnvifHost * self);
SHARD_EXPORT char * OnvifHost__getAddress(OnvifHost * self);

G_END_DECLS


#endif
