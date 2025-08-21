#ifndef ONVIF_DEVICE_DATETIME_LOCAL_H_ 
#define ONVIF_DEVICE_DATETIME_LOCAL_H_

#include "onvif_device_datetime.h"
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifDeviceDateTime * OnvifDeviceDateTime__new(struct _tds__GetSystemDateAndTimeResponse * response);

#endif
