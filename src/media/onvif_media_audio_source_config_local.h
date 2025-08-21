#ifndef ONVIF_MEDIA_AUDIO_SOURCE_CONFIG_LOCAL_H_ 
#define ONVIF_MEDIA_AUDIO_SOURCE_CONFIG_LOCAL_H_

#include "onvif_media_audio_source_config.h"

#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifMediaAudioSourceConfig * OnvifMediaAudioSourceConfig__new(struct tt__AudioSourceConfiguration * resp);

#endif
