#ifndef ONVIF_MEDIA1_SERVICE_H_ 
#define ONVIF_MEDIA1_SERVICE_H_

#include "../../shard_export.h"
#include "../onvif_media_service.h"
#include "onvif_media1_profiles.h"
#include "onvif_media1_uri.h"
#include "onvif_media1_service_capabilities.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA1_SERVICE OnvifMedia1Service__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia1Service, OnvifMedia1Service_, ONVIF, MEDIA1_SERVICE, OnvifMediaService)

struct _OnvifMedia1Service {
    OnvifMediaService parent_instance;
};

struct _OnvifMedia1ServiceClass {
    OnvifMediaServiceClass parent_class;
};

SHARD_EXPORT OnvifMedia1Service * OnvifMedia1Service__new(OnvifDevice * device, const char * endpoint);

//Remote Sevice function
SHARD_EXPORT OnvifMedia1Profiles * OnvifMedia1Service__getProfiles(OnvifMedia1Service* self);
SHARD_EXPORT OnvifMedia1Uri * OnvifMedia1Service__getSnapshotUri(OnvifMedia1Service *self, int profile_index);
SHARD_EXPORT OnvifMedia1Uri * OnvifMedia1Service__getStreamUri(OnvifMedia1Service* self, int profile_index);
SHARD_EXPORT OnvifMedia1ServiceCapabilities * OnvifMedia1Service__getServiceCapabilities(OnvifMedia1Service* self);

G_END_DECLS

#endif