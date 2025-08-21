#ifndef ONVIF_DEVICE_H_ 
#define ONVIF_DEVICE_H_

typedef struct _OnvifDevice OnvifDevice;

#include "shard_export.h"
#include "devicemgmt/onvif_device_service.h"
#include "deviceio/onvif_deviceio_service.h"
#include "media/media1/onvif_media1_service.h"
#include "media/media2/onvif_media2_service.h"
#include "SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICE OnvifDevice__get_type()
G_DECLARE_FINAL_TYPE (OnvifDevice, OnvifDevice_, ONVIF, DEVICE, GObject)

struct _OnvifDevice {
  GObject parent_instance;
};


struct _OnvifDeviceClass {
  GObjectClass parent_class;
};

#define ONVIF_DEVICE_LOG(fmt, dev, level, ...) \
  if(dev){ \
    char * asdendpoint = OnvifBaseService__get_endpoint(ONVIF_BASE_SERVICE(OnvifDevice__get_device_service(dev))); \
    C_##level(fmt,asdendpoint,##__VA_ARGS__); \
    free(asdendpoint); \
  } else { \
    C_##level(fmt,NULL,##__VA_ARGS__); \
  }

#define ONVIF_DEVICE_TRAIL(fmt,dev, ...) ONVIF_DEVICE_LOG(fmt,dev,TRAIL,##__VA_ARGS__)
#define ONVIF_DEVICE_TRACE(fmt,dev, ...) ONVIF_DEVICE_LOG(fmt,dev,TRACE,##__VA_ARGS__)
#define ONVIF_DEVICE_DEBUG(fmt,dev, ...) ONVIF_DEVICE_LOG(fmt,dev,DEBUG,##__VA_ARGS__)
#define ONVIF_DEVICE_INFO(fmt,dev, ...) ONVIF_DEVICE_LOG(fmt,dev,INFO,##__VA_ARGS__)
#define ONVIF_DEVICE_WARN(fmt,dev, ...) ONVIF_DEVICE_LOG(fmt,dev,WARN,##__VA_ARGS__)
#define ONVIF_DEVICE_ERROR(fmt,dev, ...) ONVIF_DEVICE_LOG(fmt,dev,ERROR,##__VA_ARGS__)
#define ONVIF_DEVICE_FATAL(fmt,dev, ...) ONVIF_DEVICE_LOG(fmt,dev,FATAL,##__VA_ARGS__)

#define ONVIF_MEDIA_LOG(fmt, dev, level, ...) \
  if(dev){ \
    char * asdendpoint = OnvifBaseService__get_endpoint(ONVIF_BASE_SERVICE(OnvifDevice__get_media_service(dev))); \
    C_##level(fmt,asdendpoint,##__VA_ARGS__); \
    free(asdendpoint); \
  } else { \
    C_##level(fmt,NULL,##__VA_ARGS__); \
  }

#define ONVIF_MEDIA_TRAIL(fmt,dev, ...) ONVIF_MEDIA_LOG(fmt,dev,TRAIL,##__VA_ARGS__)
#define ONVIF_MEDIA_TRACE(fmt,dev, ...) ONVIF_MEDIA_LOG(fmt,dev,TRACE,##__VA_ARGS__)
#define ONVIF_MEDIA_DEBUG(fmt,dev, ...) ONVIF_MEDIA_LOG(fmt,dev,DEBUG,##__VA_ARGS__)
#define ONVIF_MEDIA_INFO(fmt,dev, ...) ONVIF_MEDIA_LOG(fmt,dev,INFO,##__VA_ARGS__)
#define ONVIF_MEDIA_WARN(fmt,dev, ...) ONVIF_MEDIA_LOG(fmt,dev,WARN,##__VA_ARGS__)
#define ONVIF_MEDIA_ERROR(fmt,dev, ...) ONVIF_MEDIA_LOG(fmt,dev,ERROR,##__VA_ARGS__)
#define ONVIF_MEDIA_FATAL(fmt,dev, ...) ONVIF_MEDIA_LOG(fmt,dev,FATAL,##__VA_ARGS__)

SHARD_EXPORT OnvifDevice * OnvifDevice__new(char * device_url);
SHARD_EXPORT char * OnvifDevice__get_host(OnvifDevice* self);
SHARD_EXPORT char * OnvifDevice__get_port(OnvifDevice* self);
SHARD_EXPORT SoapFault OnvifDevice__authenticate(OnvifDevice* self);
SHARD_EXPORT int OnvifDevice__is_authenticated(OnvifDevice* self);
SHARD_EXPORT void OnvifDevice__set_credentials(OnvifDevice* self,const char * user,const char* pass);
SHARD_EXPORT OnvifCredentials * OnvifDevice__get_credentials(OnvifDevice * self);
SHARD_EXPORT int OnvifDevice__is_valid(OnvifDevice* self);
SHARD_EXPORT OnvifDeviceService * OnvifDevice__get_device_service(OnvifDevice* self);
SHARD_EXPORT OnvifDeviceIOService * OnvifDevice__get_deviceio_service(OnvifDevice* self);
SHARD_EXPORT OnvifMediaService * OnvifDevice__get_media_service(OnvifDevice* self);
SHARD_EXPORT OnvifMedia1Service * OnvifDevice__get_media1_service(OnvifDevice* self);
SHARD_EXPORT OnvifMedia2Service * OnvifDevice__get_media2_service(OnvifDevice* self);
SHARD_EXPORT time_t * OnvifDevice__getSystemDateTime(OnvifDevice * self);
SHARD_EXPORT double OnvifDevice__getTimeOffset(OnvifDevice * self);

G_END_DECLS

#endif
