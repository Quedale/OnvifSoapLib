#ifndef ONVIF_DEV_INTERFACE_LOCAL_H_ 
#define ONVIF_DEV_INTERFACE_LOCAL_H_

#include "onvif_device_interface.h"
#include "generated/soapH.h"

OnvifInterfaces * OnvifInterfaces__create(struct _tds__GetNetworkInterfacesResponse * intefaces);
void OnvifInterfaces__init(OnvifInterfaces * self, struct _tds__GetNetworkInterfacesResponse * interfaces);

#endif