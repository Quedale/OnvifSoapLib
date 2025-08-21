#ifndef ONVIF_MEDIA2_URI_H_ 
#define ONVIF_MEDIA2_URI_H_

#include "../onvif_uri.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA2_URI OnvifMedia2Uri__get_type()
G_DECLARE_FINAL_TYPE (OnvifMedia2Uri, OnvifMedia2Uri_, ONVIF, MEDIA2_URI, OnvifUri)

struct _OnvifMedia2Uri {
    OnvifUri parent_instance;
};


struct _OnvifMedia2UriClass {
    OnvifUriClass parent_class;
};

G_END_DECLS

#endif
