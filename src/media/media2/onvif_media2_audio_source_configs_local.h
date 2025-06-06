#ifndef ONVIF_MEDIA2_AUDIO_SOURCE_CONFIGS_LOCAL_H_ 
#define ONVIF_MEDIA2_AUDIO_SOURCE_CONFIGS_LOCAL_H_

#include "onvif_media2_audio_source_configs.h"

#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifMedia2AudioSourceConfigs * OnvifMedia2AudioSourceConfigs__new(struct _ns1__GetAudioSourceConfigurationsResponse * resp);

#endif