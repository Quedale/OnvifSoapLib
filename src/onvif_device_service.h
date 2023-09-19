#ifndef ONVIF_DEV_SERVICE_H_ 
#define ONVIF_DEV_SERVICE_H_

#include "shard_export.h"
#include "onvif_base_service.h"
#include "onvif_credentials.h"
#include "onvif_device_info.h"
#include "onvif_device_interface.h"
#include "onvif_device_capabilities.h"
#include "onvif_device_scopes.h"

typedef struct _OnvifScope OnvifScope;
typedef struct _OnvifScopes OnvifScopes;

typedef struct _OnvifDeviceService OnvifDeviceService;


OnvifDeviceService * OnvifDeviceService__create(const char * endpoint, OnvifCredentials * credentials, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data);
void OnvifDeviceService__init(OnvifDeviceService * self,const char * endpoint, OnvifCredentials * credentials, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data);
SHARD_EXPORT void OnvifDeviceService__destroy(OnvifDeviceService * self);
SHARD_EXPORT OnvifBaseService * OnvifDeviceService__get_parent(OnvifDeviceService * self);

SHARD_EXPORT OnvifScopes * OnvifDeviceService__getScopes(OnvifDeviceService * self);
SHARD_EXPORT char * OnvifDeviceService__get_endpoint(OnvifDeviceService * self);
SHARD_EXPORT char * OnvifDeviceService__getHostname(OnvifDeviceService * self); 
SHARD_EXPORT OnvifCapabilities* OnvifDeviceService__getCapabilities(OnvifDeviceService * self);
SHARD_EXPORT OnvifDeviceInformation * OnvifDeviceService__getDeviceInformation(OnvifDeviceService *self);
SHARD_EXPORT OnvifInterfaces * OnvifDeviceService__getNetworkInterfaces(OnvifDeviceService * self);

#endif