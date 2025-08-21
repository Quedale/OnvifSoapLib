#ifndef ONVIF_MEDIA_SNAPSHOT_H_ 
#define ONVIF_MEDIA_SNAPSHOT_H_

#include "../shard_export.h"
#include "../SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_SNAPSHOT OnvifMediaSnapshot__get_type()
G_DECLARE_FINAL_TYPE (OnvifMediaSnapshot, OnvifMediaSnapshot_, ONVIF, MEDIA_SNAPSHOT, SoapObject)

struct _OnvifMediaSnapshot {
    SoapObject parent_instance;
};


struct _OnvifMediaSnapshotClass {
    SoapObjectClass parent_class;
};

SHARD_EXPORT int OnvifMediaSnapshot__get_size(OnvifMediaSnapshot * self);
SHARD_EXPORT char * OnvifMediaSnapshot__get_buffer(OnvifMediaSnapshot * self);

G_END_DECLS


#endif
