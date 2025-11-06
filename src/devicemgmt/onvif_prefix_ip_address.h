#ifndef ONVIF_PREFIX_IP_ADDRESS_H_ 
#define ONVIF_PREFIX_IP_ADDRESS_H_

#include "shard_export.h"

typedef struct _OnvifPrefixedIPAddress OnvifPrefixedIPAddress;

SHARD_EXPORT char * OnvifPrefixedIPAddress__get_address(OnvifPrefixedIPAddress * self);
SHARD_EXPORT int OnvifPrefixedIPAddress__get_prefix(OnvifPrefixedIPAddress * self);
SHARD_EXPORT void OnvifPrefixedIPAddress__destroy(OnvifPrefixedIPAddress * self);

#endif