#ifndef ONVIF_SNAPSHOT_URI_H_ 
#define ONVIF_SNAPSHOT_URI_H_

#include "shard_export.h"
#include "SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_SNAPSHOTURI OnvifSnapshotUri__get_type()
G_DECLARE_FINAL_TYPE (OnvifSnapshotUri, OnvifSnapshotUri_, ONVIF, SNAPSHOTURI, SoapObject)

struct _OnvifSnapshotUri
{
    SoapObject parent_instance;
};


struct _OnvifSnapshotUriClass
{
    SoapObjectClass parent_class;
};

SHARD_EXPORT char * OnvifSnapshotUri__get_uri(OnvifSnapshotUri* self);

G_END_DECLS


#endif