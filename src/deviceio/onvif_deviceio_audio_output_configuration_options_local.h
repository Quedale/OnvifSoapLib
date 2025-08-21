#ifndef ONVIF_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS_LOCAL_H_ 
#define ONVIF_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS_LOCAL_H_

#include "onvif_deviceio_audio_output_configuration_options.h"
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifDeviceIOAudioOutputConfigurationOptions * OnvifDeviceIOAudioOutputConfigurationOptions__new(struct _tmd__GetAudioOutputConfigurationOptionsResponse * response);

#endif
