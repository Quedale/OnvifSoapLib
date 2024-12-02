#ifndef ONVIF_MEDIA1_SERVICECAPABILITIES_LOCAL_H_ 
#define ONVIF_MEDIA1_SERVICECAPABILITIES_LOCAL_H_

#include "onvif_media1_service_capabilities.h"
#include "../onvif_media_service_capabilities_local.h"

#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifMedia1ServiceCapabilities* OnvifMedia1ServiceCapabilities__new(struct _trt__GetServiceCapabilitiesResponse * resp);

#endif