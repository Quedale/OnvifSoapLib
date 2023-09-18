#ifndef ONVIF_DEV_INFO_LOCAL_H_ 
#define ONVIF_DEV_INFO_LOCAL_H_

#include "onvif_device_info.h"
#include "generated/soapH.h"

OnvifDeviceInformation * OnvifDeviceInformation__create(struct _tds__GetDeviceInformationResponse *);
void OnvifDeviceInformation__init(OnvifDeviceInformation *self, struct _tds__GetDeviceInformationResponse *);

#endif