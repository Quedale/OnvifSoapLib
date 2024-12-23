#ifndef ONVIF_MEDIA2_PROFILES_LOCAL_H_ 
#define ONVIF_MEDIA2_PROFILES_LOCAL_H_


#include "onvif_media2_profiles.h"
#include "onvif_media2_profile_local.h"

#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifMedia2Profiles * OnvifMedia2Profiles__new(struct _ns1__GetProfilesResponse * resp);

#endif