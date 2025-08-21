#ifndef ONVIF_MEDIA1_URI_H_ 
#define ONVIF_MEDIA1_URI_H_

#include "../../shard_export.h"
#include "../onvif_uri.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA1_URI OnvifMedia1Uri__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia1Uri, OnvifMedia1Uri_, ONVIF, MEDIA1_URI, OnvifUri)

struct _OnvifMedia1Uri {
    SoapObject parent_instance;
};


struct _OnvifMedia1UriClass {
    SoapObjectClass parent_class;
};

SHARD_EXPORT char * OnvifMedia1Uri__get_uri(OnvifMedia1Uri* self);

G_END_DECLS

#endif
