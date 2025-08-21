#ifndef ONVIF_URI_H_ 
#define ONVIF_URI_H_

#include "../shard_export.h"
#include "../SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_URI OnvifUri__get_type()
G_DECLARE_FINAL_TYPE (OnvifUri, OnvifUri_, ONVIF, URI, SoapObject)

struct _OnvifUri {
  SoapObject parent_instance;
};


struct _OnvifUriClass {
  SoapObjectClass parent_class;
};

SHARD_EXPORT char * OnvifUri__get_uri(OnvifUri* self);

G_END_DECLS

#endif
