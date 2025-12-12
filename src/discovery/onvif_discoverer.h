#ifndef ANDRONVIF_ONVIF_DISCOVERER_H
#define ANDRONVIF_ONVIF_DISCOVERER_H

#include <glib-object.h>
#include "shard_export.h"
#include "onvif_probematches.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DISCOVERER OnvifDiscoverer__get_type()
G_DECLARE_FINAL_TYPE (OnvifDiscoverer, OnvifDiscoverer_, ONVIF, DISCOVERER, GObject)

struct _OnvifDiscoverer {
    GObject parent_instance;
};


struct _OnvifDiscovererClass {
    GObjectClass parent_class;
};

SHARD_EXPORT OnvifDiscoverer * OnvifDiscoverer__new(int retry, int timeout);
SHARD_EXPORT void OnvifDiscoverer__start(OnvifDiscoverer * self);
SHARD_EXPORT void OnvifDiscoverer__stop(OnvifDiscoverer * self);

#endif //ANDRONVIF_ONVIF_DISCOVERER_H
