#ifndef ONVIF_MEDIA2_URI_LOCAL_H_ 
#define ONVIF_MEDIA2_URI_LOCAL_H_

#include "onvif_media2_uri.h"
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifMedia2Uri * OnvifMedia2StreamUri__new(struct _ns1__GetStreamUriResponse * resp);
OnvifMedia2Uri * OnvifMedia2SnapshotUri__new(struct _ns1__GetSnapshotUriResponse * resp);

#endif