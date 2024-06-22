#ifndef ONVIF_SNAPSHOT_URI_H_ 
#define ONVIF_SNAPSHOT_URI_H_

#include "shard_export.h"
#include "SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_URI OnvifMediaUri__get_type()
G_DECLARE_FINAL_TYPE (OnvifMediaUri, OnvifMediaUri_, ONVIF, MEDIA_URI, SoapObject)

struct _OnvifMediaUri
{
    SoapObject parent_instance;
};


struct _OnvifMediaUriClass
{
    SoapObjectClass parent_class;
};

SHARD_EXPORT char * OnvifMediaUri__get_uri(OnvifMediaUri* self);

G_END_DECLS


#endif