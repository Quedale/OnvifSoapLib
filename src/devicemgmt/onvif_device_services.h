#ifndef ONVIF_DEVICE_SERVICES_H_ 
#define ONVIF_DEVICE_SERVICES_H_

#include "../shard_export.h"
#include "../SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICE_SERVICES OnvifDeviceServices__get_type()
G_DECLARE_FINAL_TYPE (OnvifDeviceServices, OnvifDeviceServices_, ONVIF, DEVICE_SERVICES, SoapObject)

typedef struct _OnvifService OnvifService;

struct _OnvifDeviceServices {
    SoapObject parent_instance;
};


struct _OnvifDeviceServicesClass {
    SoapObjectClass parent_class;
};

SHARD_EXPORT OnvifService * OnvifDeviceServices__get_deviceio(OnvifDeviceServices * self);
SHARD_EXPORT OnvifService * OnvifDeviceServices__get_media(OnvifDeviceServices * self);
SHARD_EXPORT OnvifService * OnvifDeviceServices__get_media2(OnvifDeviceServices * self);
SHARD_EXPORT OnvifService * OnvifDeviceServices__get_events(OnvifDeviceServices * self);
SHARD_EXPORT OnvifService * OnvifDeviceServices__get_imaging(OnvifDeviceServices * self);

SHARD_EXPORT char * OnvifService__get_address(OnvifService * self);

G_END_DECLS

#endif