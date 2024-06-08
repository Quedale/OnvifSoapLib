#ifndef ONVIF_CAPS_H_ 
#define ONVIF_CAPS_H_

#include "shard_export.h"

typedef struct _OnvifMedia OnvifMedia;
typedef struct _OnvifCapabilities OnvifCapabilities;

SHARD_EXPORT void OnvifCapabilities__destroy(OnvifCapabilities * self);
SHARD_EXPORT OnvifMedia * OnvifCapabilities__get_media(OnvifCapabilities * self);

SHARD_EXPORT char * OnvifMedia__get_address(OnvifMedia * self);

#endif