#ifndef ONVIF_DEV_INFO_H_ 
#define ONVIF_DEV_INFO_H_

#include "shard_export.h"

typedef struct _OnvifDeviceInformation OnvifDeviceInformation;

SHARD_EXPORT void OnvifDeviceInformation__destroy(OnvifDeviceInformation *self);
SHARD_EXPORT char * OnvifDeviceInformation__get_manufacturer(OnvifDeviceInformation *self);
SHARD_EXPORT char * OnvifDeviceInformation__get_model(OnvifDeviceInformation *self);
SHARD_EXPORT char * OnvifDeviceInformation__get_firmwareVersion(OnvifDeviceInformation *self);
SHARD_EXPORT char * OnvifDeviceInformation__get_serialNumber(OnvifDeviceInformation *self);
SHARD_EXPORT char * OnvifDeviceInformation__get_hardwareId(OnvifDeviceInformation *self);

#endif