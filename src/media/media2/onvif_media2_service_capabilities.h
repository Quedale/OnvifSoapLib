#ifndef ONVIF_MEDIA2_SERVICECAPABILITIES_H_ 
#define ONVIF_MEDIA2_SERVICECAPABILITIES_H_

#include "../onvif_media_service_capabilities.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA2_SERVICECAPABILITIES OnvifMedia2ServiceCapabilities__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia2ServiceCapabilities, OnvifMedia2ServiceCapabilities_, ONVIF, MEDIA2_SERVICECAPABILITIES, OnvifMediaServiceCapabilities)

struct _OnvifMedia2ServiceCapabilities {
    OnvifMediaServiceCapabilities parent_instance;
};


struct _OnvifMedia2ServiceCapabilitiesClass {
    OnvifMediaServiceCapabilitiesClass parent_class;
};

G_END_DECLS

#endif