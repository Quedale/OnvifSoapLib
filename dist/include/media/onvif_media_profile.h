#ifndef ONVIF_MEDIA_PROFILE_H_ 
#define ONVIF_MEDIA_PROFILE_H_

#include "../shard_export.h"
#include "../SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_MEDIA_PROFILE OnvifMediaProfile__get_type()
G_DECLARE_FINAL_TYPE (OnvifMediaProfile, OnvifMediaProfile_, ONVIF, MEDIA_PROFILE, SoapObject)

typedef struct soap SoapDef;

struct _OnvifMediaProfile {
  GObject parent_instance;
};


struct _OnvifMediaProfileClass {
  GObjectClass parent_class;
};

SHARD_EXPORT const char * OnvifMediaProfile__get_name(OnvifMediaProfile* self);
SHARD_EXPORT const char * OnvifMediaProfile__get_token(OnvifMediaProfile* self);
SHARD_EXPORT int OnvifMediaProfile__get_index(OnvifMediaProfile* self);

G_END_DECLS

#endif
