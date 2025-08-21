#ifndef ONVIF_MEDIA_SERVICECAPABILITIES_H_ 
#define ONVIF_MEDIA_SERVICECAPABILITIES_H_

#include "../shard_export.h"
#include "../SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_SERVICECAPABILITIES OnvifMediaServiceCapabilities__get_type()
G_DECLARE_FINAL_TYPE (OnvifMediaServiceCapabilities, OnvifMediaServiceCapabilities_, ONVIF, MEDIA_SERVICECAPABILITIES, SoapObject)

struct _OnvifMediaServiceCapabilities {
    SoapObject parent_instance;
};


struct _OnvifMediaServiceCapabilitiesClass {
    SoapObjectClass parent_class;
};

SHARD_EXPORT int OnvifMediaServiceCapabilities__get_snapshot_uri(OnvifMediaServiceCapabilities* self);

G_END_DECLS

#endif
