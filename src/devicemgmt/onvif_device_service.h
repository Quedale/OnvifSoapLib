#ifndef ONVIF_DEVIO_SERVICE_H_ 
#define ONVIF_DEVIO_SERVICE_H_

#include "shard_export.h"
#include "../onvif_base_service.h"
#include "onvif_credentials.h"
#include "onvif_device_capabilities.h"
#include "onvif_device_hostnameinfo.h"
#include "onvif_device_info.h"
#include "onvif_device_interface.h"
#include "onvif_device_scopes.h"
#include "onvif_device_datetime.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICE_SERVICE OnvifDeviceService__get_type()
G_DECLARE_FINAL_TYPE (OnvifDeviceService, OnvifDeviceService_, ONVIF, DEVICE_SERVICE, OnvifBaseService)

struct _OnvifDeviceService
{
    OnvifBaseService parent_instance;
};


struct _OnvifDeviceServiceClass
{
    OnvifBaseServiceClass parent_class;
};

SHARD_EXPORT OnvifDeviceService * OnvifDeviceService__new(OnvifDevice * device, const char * endpoint);
SHARD_EXPORT OnvifScopes * OnvifDeviceService__getScopes(OnvifDeviceService * self);
SHARD_EXPORT OnvifDeviceHostnameInfo * OnvifDeviceService__getHostname(OnvifDeviceService * self); 
SHARD_EXPORT OnvifCapabilities* OnvifDeviceService__getCapabilities(OnvifDeviceService * self);
SHARD_EXPORT OnvifDeviceInformation * OnvifDeviceService__getDeviceInformation(OnvifDeviceService *self);
SHARD_EXPORT OnvifDeviceInterfaces * OnvifDeviceService__getNetworkInterfaces(OnvifDeviceService * self);
SHARD_EXPORT OnvifDeviceDateTime * OnvifDeviceService__getSystemDateAndTime(OnvifDeviceService * self);

G_END_DECLS

#endif