#ifndef ONVIF_MEDIA1_PROFILE_LOCAL_H_ 
#define ONVIF_MEDIA1_PROFILE_LOCAL_H_

#include "onvif_media1_profile.h"
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifMedia1Profile * OnvifMedia1Profile__new(struct tt__Profile * profile, int index);

#endif
