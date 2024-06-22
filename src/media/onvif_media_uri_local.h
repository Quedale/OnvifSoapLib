#ifndef ONVIF_SNAPSHOT_URI_LOCAL_H_ 
#define ONVIF_SNAPSHOT_URI_LOCAL_H_

#include "onvif_media_uri.h"
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifMediaUri * OnvifSnapshotUri__new(struct _trt__GetSnapshotUriResponse * resp);
OnvifMediaUri * OnvifStreamUri__new(struct _trt__GetStreamUriResponse * resp);
OnvifMediaUri * OnvifMediaUri__new(struct tt__MediaUri *MediaUri);
#endif