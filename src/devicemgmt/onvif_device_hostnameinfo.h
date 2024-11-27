#ifndef ONVIF_DEV_HOST_INFO_H_ 
#define ONVIF_DEV_HOST_INFO_H_

#include "../shard_export.h"
#include "../SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICE_HOSTNAME_INFO OnvifDeviceHostnameInfo__get_type()
G_DECLARE_FINAL_TYPE (OnvifDeviceHostnameInfo, OnvifDeviceHostnameInfo_, ONVIF, DEVICE_HOSTNAME_INFO, SoapObject)

struct _OnvifDeviceHostnameInfo {
    SoapObject parent_instance;
};


struct _OnvifDeviceHostnameInfoClass {
    SoapObjectClass parent_class;
};

SHARD_EXPORT char * OnvifDeviceHostnameInfo__get_name(OnvifDeviceHostnameInfo * self);
SHARD_EXPORT gboolean OnvifDeviceHostnameInfo__is_fromDHCP(OnvifDeviceHostnameInfo * self);

G_END_DECLS

#endif