#include "SoapObject.h"

#ifndef ONVIF_MEDIASERVICECAPABILITIES_H_ 
#define ONVIF_MEDIASERVICECAPABILITIES_H_

#include "shard_export.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIASERVICECAPABILITIES OnvifMediaServiceCapabilities__get_type()
G_DECLARE_FINAL_TYPE (OnvifMediaServiceCapabilities, OnvifMediaServiceCapabilities_, ONVIF, MEDIASERVICECAPABILITIES, SoapObject)

// typedef struct _OnvifMediaServiceCapabilities OnvifMediaServiceCapabilities;
typedef struct _OnvifMediaServiceCapabilities OnvifMediaServiceCapabilities;
// typedef struct _OnvifMediaServiceCapabilitiesClass OnvifMediaServiceCapabilitiesClass;

struct _OnvifMediaServiceCapabilities
{
    SoapObject parent_instance;
};


struct _OnvifMediaServiceCapabilitiesClass
{
    SoapObjectClass parent_class;
};

SHARD_EXPORT int OnvifMediaServiceCapabilities__get_snapshot_uri(OnvifMediaServiceCapabilities* self);

G_END_DECLS

#endif