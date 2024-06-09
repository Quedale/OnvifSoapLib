#include "onvif_media_snapshot.h"
#include <stdlib.h>
#include <string.h>
#include "clogger.h"


enum
{
  PROP_SIZE = 1,
  PROP_BUFFER = 2,
  N_PROPERTIES
};

typedef struct {
    char * buffer;
    size_t size;
} OnvifSnapshotPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifSnapshot, OnvifSnapshot_, SOAP_TYPE_OBJECT)
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
OnvifSnapshot__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    size_t new_size;
    OnvifSnapshot * self = ONVIF_SNAPSHOT (object);
    OnvifSnapshotPrivate *priv = OnvifSnapshot__get_instance_private (self);
    switch (prop_id){
        case PROP_BUFFER:
            if(!priv->buffer)
                priv->buffer = malloc(priv->size);
            else
                priv->buffer = realloc(priv->buffer,priv->size);
            char * buffer = g_value_get_pointer (value);
            memcpy(priv->buffer,buffer,priv->size);
            break;
        case PROP_SIZE:
            new_size = g_value_get_uint (value);
            priv->size = new_size;
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifSnapshot__get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    OnvifSnapshot * self = ONVIF_SNAPSHOT (object);
    OnvifSnapshotPrivate *priv = OnvifSnapshot__get_instance_private (self);
    switch (prop_id){
        case PROP_BUFFER:
            g_value_set_pointer (value, priv->buffer);
            break;
        case PROP_SIZE:
            g_value_set_uint (value, priv->size);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifSnapshot__dispose (GObject *self)
{
    OnvifSnapshotPrivate *priv = OnvifSnapshot__get_instance_private (ONVIF_SNAPSHOT(self));
    if(priv->buffer){
        free(priv->buffer);
        priv->buffer = NULL;
    }
    priv->size = 0;
}

static void
OnvifSnapshot__class_init (OnvifSnapshotClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifSnapshot__dispose;
    object_class->set_property = OnvifSnapshot__set_property;
    object_class->get_property = OnvifSnapshot__get_property;

    obj_properties[PROP_SIZE] =
        g_param_spec_uint ("size",
                            "BufferSize",
                            "uint setting buffer size.",
                            0, G_MAXUINT, 0,
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
    obj_properties[PROP_BUFFER] =
        g_param_spec_pointer ("buffer",
                            "BufferData",
                            "Pointer to snapshot data buffer.",
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
    g_object_class_install_properties (object_class,
                                        N_PROPERTIES,
                                        obj_properties);
}

static void
OnvifSnapshot__init (OnvifSnapshot * self)
{
    OnvifSnapshotPrivate *priv = OnvifSnapshot__get_instance_private (ONVIF_SNAPSHOT(self));
    priv->size = 0;
    priv->buffer = NULL;
}

OnvifSnapshot * OnvifSnapshot__new(size_t size, char * buffer){
    return g_object_new (ONVIF_TYPE_SNAPSHOT, "size", size, "buffer", buffer, NULL);
}

int OnvifSnapshot__get_size(OnvifSnapshot * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_SNAPSHOT (self), 0);
    OnvifSnapshotPrivate *priv = OnvifSnapshot__get_instance_private (ONVIF_SNAPSHOT(self));
    return priv->size;
}

char * OnvifSnapshot__get_buffer(OnvifSnapshot * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_SNAPSHOT (self), NULL);
    OnvifSnapshotPrivate *priv = OnvifSnapshot__get_instance_private (ONVIF_SNAPSHOT(self));
    return priv->buffer;
}