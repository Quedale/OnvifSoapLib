#ifndef ONVIF_DEV_PROFILE_LOCAL_H_ 
#define ONVIF_DEV_PROFILE_LOCAL_H_

#include "onvif_media_profile.h"
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifProfile * OnvifProfile__create(struct tt__Profile * profile);
void OnvifProfile__init(OnvifProfile * self,struct tt__Profile * profile);

OnvifProfiles * OnvifProfiles__create(struct _trt__GetProfilesResponse * resp);
void OnvifProfiles__init(OnvifProfiles * self,struct _trt__GetProfilesResponse * resp);

#endif