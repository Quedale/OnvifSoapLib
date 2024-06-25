#ifndef ONVIF_BASE_SERVICE_H_ 
#define ONVIF_BASE_SERVICE_H_

#include <glib-object.h>
#include "shard_export.h"


G_BEGIN_DECLS

#define ONVIF_TYPE_BASE_SERVICE OnvifBaseService__get_type()
G_DECLARE_FINAL_TYPE (OnvifBaseService, OnvifBaseService_, ONVIF, BASE_SERVICE, GObject)

typedef struct soap SoapDef;

struct _OnvifBaseService
{
  GObject parent_instance;

  /* Other members, including private data. */
};


struct _OnvifBaseServiceClass
{
  GObjectClass parent_class;

};

#include "onvif_device.h"

SHARD_EXPORT char * OnvifBaseService__get_endpoint(OnvifBaseService * self);
SHARD_EXPORT OnvifDevice * OnvifBaseService__get_device(OnvifBaseService * self);

G_END_DECLS

#endif