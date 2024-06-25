#include "onvif_media_snapshot.h"
#include <stdlib.h>
#include <string.h>
#include "clogger.h"


typedef struct {
    char * buffer;
    size_t size;
} OnvifSnapshotPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifSnapshot, OnvifSnapshot_, SOAP_TYPE_OBJECT)

static void
OnvifSnapshot__construct(SoapObject * obj, gpointer ptr){
    OnvifSnapshotPrivate * data = ptr;
    OnvifSnapshot * self = ONVIF_SNAPSHOT(obj);
    OnvifSnapshotPrivate *priv = OnvifSnapshot__get_instance_private (self);

    priv->size = data->size;

    if(!priv->buffer)
        priv->buffer = malloc(priv->size);
    else
        priv->buffer = realloc(priv->buffer,priv->size);
    memcpy(priv->buffer,data->buffer,priv->size);
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
    G_OBJECT_CLASS (OnvifSnapshot__parent_class)->dispose (self);
}

static void
OnvifSnapshot__class_init (OnvifSnapshotClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifSnapshot__dispose;
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifSnapshot__construct;
}

static void
OnvifSnapshot__init (OnvifSnapshot * self)
{
    OnvifSnapshotPrivate *priv = OnvifSnapshot__get_instance_private (ONVIF_SNAPSHOT(self));
    priv->size = 0;
    priv->buffer = NULL;
}

OnvifSnapshot * OnvifSnapshot__new(size_t size, char * buffer){
    OnvifSnapshotPrivate priv = { buffer, size };
    return g_object_new (ONVIF_TYPE_SNAPSHOT, "soap", &priv, NULL);
}

int OnvifSnapshot__get_size(OnvifSnapshot * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_SNAPSHOT (self), 0);
    OnvifSnapshotPrivate *priv = OnvifSnapshot__get_instance_private (ONVIF_SNAPSHOT(self));
    return priv->size;
}

char * OnvifSnapshot__get_buffer(OnvifSnapshot * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_SNAPSHOT (self), NULL);
    OnvifSnapshotPrivate *priv = OnvifSnapshot__get_instance_private (ONVIF_SNAPSHOT(self));
    return priv->buffer;
}