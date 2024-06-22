#ifndef ONVIF_DEV_PROFILE_LOCAL_H_ 
#define ONVIF_DEV_PROFILE_LOCAL_H_

#include "onvif_media_profile.h"
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifProfile * OnvifProfile__create(struct tt__Profile * profile, int index);
SHARD_EXPORT void OnvifProfile__real_destroy(OnvifProfile* self);

OnvifMediaProfiles * OnvifMediaProfiles__new(struct _trt__GetProfilesResponse * resp);
void OnvifMediaProfiles__insert(OnvifMediaProfiles * self,OnvifProfile * profile);

#endif