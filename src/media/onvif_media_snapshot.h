#ifndef ONVIF_SNAPSHOT_H_ 
#define ONVIF_SNAPSHOT_H_

#include "shard_export.h"
#include "SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_SNAPSHOT OnvifSnapshot__get_type()
G_DECLARE_FINAL_TYPE (OnvifSnapshot, OnvifSnapshot_, ONVIF, SNAPSHOT, SoapObject)

struct _OnvifSnapshot
{
    SoapObject parent_instance;
};


struct _OnvifSnapshotClass
{
    SoapObjectClass parent_class;
};

SHARD_EXPORT int OnvifSnapshot__get_size(OnvifSnapshot * self);
SHARD_EXPORT char * OnvifSnapshot__get_buffer(OnvifSnapshot * self);

G_END_DECLS


#endif