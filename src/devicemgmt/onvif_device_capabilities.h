#ifndef ONVIF_CAPS_H_ 
#define ONVIF_CAPS_H_

#include "shard_export.h"
#include "SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICE_CAPABILITIES OnvifCapabilities__get_type()
G_DECLARE_FINAL_TYPE (OnvifCapabilities, OnvifCapabilities_, ONVIF, DEVICE_CAPABILITIES, SoapObject)

struct _OnvifCapabilities
{
    SoapObject parent_instance;
};


struct _OnvifCapabilitiesClass
{
    SoapObjectClass parent_class;
};

typedef struct _OnvifMedia OnvifMedia;
typedef struct _OnvifDeviceCapabilities OnvifDeviceCapabilities;
typedef struct _OnvifSystemCapabilities OnvifSystemCapabilities;

SHARD_EXPORT OnvifMedia * OnvifCapabilities__get_media(OnvifCapabilities * self);
SHARD_EXPORT OnvifDeviceCapabilities * OnvifCapabilities__get_device(OnvifCapabilities * self);
SHARD_EXPORT OnvifSystemCapabilities * OnvifCapabilities__get_system(OnvifCapabilities * self);

SHARD_EXPORT char * OnvifMedia__get_address(OnvifMedia * self);

SHARD_EXPORT OnvifSystemCapabilities * OnvifDeviceCapabilities__get_system(OnvifDeviceCapabilities * self);

SHARD_EXPORT gboolean OnvifSystemCapabilities__is_discoveryResolve(OnvifSystemCapabilities * self);
SHARD_EXPORT gboolean OnvifSystemCapabilities__is_discoveryBye(OnvifSystemCapabilities * self);
SHARD_EXPORT gboolean OnvifSystemCapabilities__is_remoteDiscovery(OnvifSystemCapabilities * self);
SHARD_EXPORT gboolean OnvifSystemCapabilities__is_systemBackup(OnvifSystemCapabilities * self);
SHARD_EXPORT gboolean OnvifSystemCapabilities__is_systemLogging(OnvifSystemCapabilities * self);
SHARD_EXPORT gboolean OnvifSystemCapabilities__is_firmwareUpgrade(OnvifSystemCapabilities * self);
SHARD_EXPORT int OnvifSystemCapabilities__get_majorVersion(OnvifSystemCapabilities * self);
SHARD_EXPORT int OnvifSystemCapabilities__get_minorVersion(OnvifSystemCapabilities * self);



G_END_DECLS

#endif