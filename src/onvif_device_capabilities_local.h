#ifndef ONVIF_CAPS_LOCAL_H_ 
#define ONVIF_CAPS_LOCAL_H_

#include "onvif_device_capabilities.h"
#include "generated/soapH.h"

OnvifCapabilities * OnvifCapabilities__create(struct _tds__GetCapabilitiesResponse * response);
void OnvifCapabilities__init(OnvifCapabilities * self, struct _tds__GetCapabilitiesResponse * response);

#endif