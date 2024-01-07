#ifndef ONVIF_DEV_INTERFACE_LOCAL_H_ 
#define ONVIF_DEV_INTERFACE_LOCAL_H_

#include "onvif_device_interface.h"
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifInterfaces * OnvifInterfaces__create(struct _tds__GetNetworkInterfacesResponse * intefaces);
void OnvifInterfaces__init(OnvifInterfaces * self, struct _tds__GetNetworkInterfacesResponse * interfaces);

#endif