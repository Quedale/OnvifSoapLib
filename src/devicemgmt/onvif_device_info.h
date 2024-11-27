#ifndef ONVIF_DEV_INFO_H_ 
#define ONVIF_DEV_INFO_H_

#include "../shard_export.h"
#include "../SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICE_INFORMATION OnvifDeviceInformation__get_type()
G_DECLARE_FINAL_TYPE (OnvifDeviceInformation, OnvifDeviceInformation_, ONVIF, DEVICE_INFORMATION, SoapObject)

struct _OnvifDeviceInformation {
    SoapObject parent_instance;
};


struct _OnvifDeviceInformationClass {
    SoapObjectClass parent_class;
};

SHARD_EXPORT char * OnvifDeviceInformation__get_manufacturer(OnvifDeviceInformation *self);
SHARD_EXPORT char * OnvifDeviceInformation__get_model(OnvifDeviceInformation *self);
SHARD_EXPORT char * OnvifDeviceInformation__get_firmwareVersion(OnvifDeviceInformation *self);
SHARD_EXPORT char * OnvifDeviceInformation__get_serialNumber(OnvifDeviceInformation *self);
SHARD_EXPORT char * OnvifDeviceInformation__get_hardwareId(OnvifDeviceInformation *self);

G_END_DECLS

#endif