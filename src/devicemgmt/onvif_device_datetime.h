#ifndef ONVIF_DEVICE_DATETIME_H_ 
#define ONVIF_DEVICE_DATETIME_H_

#include "../shard_export.h"
#include "../SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICE_DATETIME OnvifDeviceDateTime__get_type()
G_DECLARE_FINAL_TYPE (OnvifDeviceDateTime, OnvifDeviceDateTime_, ONVIF, DEVICE_DATETIME, SoapObject)

struct _OnvifDeviceDateTime {
    SoapObject parent_instance;
};


struct _OnvifDeviceDateTimeClass {
    SoapObjectClass parent_class;
};

SHARD_EXPORT time_t * OnvifDeviceDateTime__get_datetime(OnvifDeviceDateTime * self);

G_END_DECLS

#endif