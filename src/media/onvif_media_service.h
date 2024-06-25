#ifndef ONVIF_MEDIA_SERVICE_H_ 
#define ONVIF_MEDIA_SERVICE_H_

#include "shard_export.h"
#include "../onvif_base_service.h"
#include "onvif_credentials.h"
#include "onvif_media_profile.h"
#include "onvif_media_snapshot.h"
#include "onvif_media_uri.h"
#include "onvif_media_service_capabilities.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_SERVICE OnvifMediaService__get_type()
G_DECLARE_FINAL_TYPE (OnvifMediaService, OnvifMediaService_, ONVIF, MEDIA_SERVICE, OnvifBaseService)

struct _OnvifMediaService
{
    OnvifBaseService parent_instance;
};


struct _OnvifMediaServiceClass
{
    OnvifBaseServiceClass parent_class;
};

SHARD_EXPORT OnvifMediaService * OnvifMediaService__new(OnvifDevice * device, const char * endpoint);

//No wait functions
SHARD_EXPORT SoapFault OnvifMediaService__get_profile_token(OnvifMediaService *self, int index, char * ret);

//Lazy cache function
SHARD_EXPORT OnvifMediaProfiles * OnvifMediaService__get_profiles(OnvifMediaService * self);
SHARD_EXPORT OnvifMediaServiceCapabilities * OnvifMediaService__getServiceCapabilities(OnvifMediaService* self);

//Remote Sevice function
SHARD_EXPORT OnvifMediaProfiles * OnvifMediaService__getProfiles(OnvifMediaService* self);
SHARD_EXPORT OnvifMediaUri * OnvifMediaService__getSnapshotUri(OnvifMediaService *self, int profile_index);
SHARD_EXPORT OnvifSnapshot * OnvifMediaService__getSnapshot(OnvifMediaService *self, int profile_index);
SHARD_EXPORT OnvifMediaUri * OnvifMediaService__getStreamUri(OnvifMediaService* self, int profile_index);

#endif