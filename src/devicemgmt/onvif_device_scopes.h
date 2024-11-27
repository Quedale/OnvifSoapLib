#ifndef ONVIF_SCOPES_H_ 
#define ONVIF_SCOPES_H_

#include "../shard_export.h"
#include "../SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DEVICE_SCOPES OnvifScopes__get_type()
G_DECLARE_FINAL_TYPE (OnvifScopes, OnvifScopes_, ONVIF, DEVICE_SCOPES, SoapObject)

typedef struct _OnvifScope  OnvifScope;

struct _OnvifScopes {
    SoapObject parent_instance;
};


struct _OnvifScopesClass {
    SoapObjectClass parent_class;
};

SHARD_EXPORT char * OnvifScopes__extract_scope(OnvifScopes * scopes, char * key);

G_END_DECLS


#endif