#ifndef ONVIF_CAPS_LOCAL_H_ 
#define ONVIF_CAPS_LOCAL_H_

#include "onvif_device_capabilities.h"
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifCapabilities * OnvifCapabilities__create(struct _tds__GetCapabilitiesResponse * response);
void OnvifCapabilities__init(OnvifCapabilities * self, struct _tds__GetCapabilitiesResponse * response);

#endif