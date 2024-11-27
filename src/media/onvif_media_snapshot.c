#include "onvif_media_snapshot.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    char * buffer;
    size_t size;
} OnvifMediaSnapshotPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifMediaSnapshot, OnvifMediaSnapshot_, SOAP_TYPE_OBJECT)

static void
OnvifMediaSnapshot__construct(SoapObject * obj, gpointer ptr){
    OnvifMediaSnapshotPrivate * data = ptr;
    OnvifMediaSnapshot * self = ONVIF_MEDIA_SNAPSHOT(obj);
    OnvifMediaSnapshotPrivate *priv = OnvifMediaSnapshot__get_instance_private (self);

    priv->size = data->size;

    if(!priv->buffer)
        priv->buffer = malloc(priv->size);
    else
        priv->buffer = realloc(priv->buffer,priv->size);
    memcpy(priv->buffer,data->buffer,priv->size);
}

static void
OnvifMediaSnapshot__dispose (GObject *self)
{
    OnvifMediaSnapshotPrivate *priv = OnvifMediaSnapshot__get_instance_private (ONVIF_MEDIA_SNAPSHOT(self));
    if(priv->buffer){
        free(priv->buffer);
        priv->buffer = NULL;
    }
    priv->size = 0;
    G_OBJECT_CLASS (OnvifMediaSnapshot__parent_class)->dispose (self);
}

static void
OnvifMediaSnapshot__class_init (OnvifMediaSnapshotClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifMediaSnapshot__dispose;
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMediaSnapshot__construct;
}

static void
OnvifMediaSnapshot__init (OnvifMediaSnapshot * self)
{
    OnvifMediaSnapshotPrivate *priv = OnvifMediaSnapshot__get_instance_private (ONVIF_MEDIA_SNAPSHOT(self));
    priv->size = 0;
    priv->buffer = NULL;
}

OnvifMediaSnapshot * OnvifMediaSnapshot__new(size_t size, char * buffer){
    OnvifMediaSnapshotPrivate priv = { buffer, size };
    return g_object_new (ONVIF_TYPE_MEDIA_SNAPSHOT, "data", &priv, NULL);
}

int OnvifMediaSnapshot__get_size(OnvifMediaSnapshot * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_SNAPSHOT (self), 0);
    OnvifMediaSnapshotPrivate *priv = OnvifMediaSnapshot__get_instance_private (ONVIF_MEDIA_SNAPSHOT(self));
    return priv->size;
}

char * OnvifMediaSnapshot__get_buffer(OnvifMediaSnapshot * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_SNAPSHOT (self), NULL);
    OnvifMediaSnapshotPrivate *priv = OnvifMediaSnapshot__get_instance_private (ONVIF_MEDIA_SNAPSHOT(self));
    return priv->buffer;
}