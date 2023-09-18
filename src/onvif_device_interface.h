#ifndef ONVIF_DEV_INTERFACE_H_ 
#define ONVIF_DEV_INTERFACE_H_

#include "shard_export.h"

typedef struct _OnvifInterface OnvifInterface;
typedef struct _OnvifInterfaces OnvifInterfaces;

SHARD_EXPORT int OnvifInterfaces__get_count(OnvifInterfaces * self);
SHARD_EXPORT OnvifInterface * OnvifInterfaces__get_interface(OnvifInterfaces * self, int index);
SHARD_EXPORT void OnvifInterfaces__destroy(OnvifInterfaces * self);

SHARD_EXPORT char * OnvifInterface__get_token(OnvifInterface * self);
SHARD_EXPORT int OnvifInterface__get_enabled(OnvifInterface * self);
SHARD_EXPORT int OnvifInterface__has_info(OnvifInterface * self);
SHARD_EXPORT char * OnvifInterface__get_name(OnvifInterface * self);
SHARD_EXPORT char * OnvifInterface__get_mac(OnvifInterface * self);
SHARD_EXPORT int OnvifInterface__get_mtu(OnvifInterface * self);
SHARD_EXPORT int OnvifInterface__is_ipv4_enabled(OnvifInterface * self);
SHARD_EXPORT int OnvifInterface__get_ipv4_dhcp(OnvifInterface * self);
SHARD_EXPORT int OnvifInterface__get_ipv4_manual_count(OnvifInterface * self);
SHARD_EXPORT char ** OnvifInterface__get_ipv4_manual(OnvifInterface * self);
SHARD_EXPORT char * OnvifInterface__get_ipv4_link_local(OnvifInterface * self);
SHARD_EXPORT char * OnvifInterface__get_ipv4_from_dhcp(OnvifInterface * self);

#endif