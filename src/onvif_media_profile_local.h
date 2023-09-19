#ifndef ONVIF_DEV_PROFILE_LOCAL_H_ 
#define ONVIF_DEV_PROFILE_LOCAL_H_

#include "onvif_media_profile.h"
#include "generated/soapH.h"

OnvifProfile * OnvifProfile__create(struct tt__Profile * profile);
void OnvifProfile__init(OnvifProfile * self,struct tt__Profile * profile);

OnvifProfiles * OnvifProfiles__create(struct _trt__GetProfilesResponse * resp);
void OnvifProfiles__init(OnvifProfiles * self,struct _trt__GetProfilesResponse * resp);

#endif