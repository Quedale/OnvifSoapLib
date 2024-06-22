#ifndef ONVIF_CAPS_H_ 
#define ONVIF_CAPS_H_

#include "shard_export.h"
#include "SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICE_CAPABILITIES OnvifCapabilities__get_type()
G_DECLARE_FINAL_TYPE (OnvifCapabilities, OnvifCapabilities_, ONVIF, DEVICE_CAPABILITIES, SoapObject)

struct _OnvifCapabilities
{
    SoapObject parent_instance;
};


struct _OnvifCapabilitiesClass
{
    SoapObjectClass parent_class;
};

typedef struct _OnvifMedia OnvifMedia;

SHARD_EXPORT OnvifMedia * OnvifCapabilities__get_media(OnvifCapabilities * self);
SHARD_EXPORT char * OnvifMedia__get_address(OnvifMedia * self);

G_END_DECLS

#endif