#ifndef ONVIF_MEDIA1_SERVICECAPABILITIES_H_ 
#define ONVIF_MEDIA1_SERVICECAPABILITIES_H_

#include "../onvif_media_service_capabilities.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA1_SERVICECAPABILITIES OnvifMedia1ServiceCapabilities__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia1ServiceCapabilities, OnvifMedia1ServiceCapabilities_, ONVIF, MEDIA1_SERVICECAPABILITIES, OnvifMediaServiceCapabilities)

struct _OnvifMedia1ServiceCapabilities {
    SoapObject parent_instance;
};


struct _OnvifMedia1ServiceCapabilitiesClass {
    SoapObjectClass parent_class;
};

G_END_DECLS

#endif
