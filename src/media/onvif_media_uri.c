#include "onvif_media_uri_local.h"
#include "clogger.h"

typedef struct {
    char * uri;
    int invalidAfterConnect;
    int invalidAfterReboot;
    char * timeout;
} OnvifMediaUriPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifMediaUri, OnvifMediaUri_, SOAP_TYPE_OBJECT)

static void
OnvifMediaUri__construct(SoapObject * obj, gpointer ptr){
    OnvifMediaUri * self = ONVIF_MEDIA_URI(obj);
    struct tt__MediaUri * media_uri = ptr;
    OnvifMediaUriPrivate *priv = OnvifMediaUri__get_instance_private (self);
    if(!media_uri){
        if(priv->uri) free(priv->uri);
        priv->uri = NULL;
        priv->invalidAfterConnect = -1; //TODO
        priv->invalidAfterReboot = -1; //TODO
        if(priv->timeout) free(priv->timeout);
        priv->timeout = NULL; //TODO
        return;
    }

    if(media_uri->Uri){
        if(!priv->uri){
            priv->uri = malloc(strlen(media_uri->Uri) + 1);
        } else {
            priv->uri = realloc(priv->uri,strlen(media_uri->Uri) + 1);
        }
        strcpy(priv->uri,media_uri->Uri);
    } else if(!media_uri->Uri && priv->uri){
        free(priv->uri);
        priv->uri = NULL;
    }

    priv->invalidAfterConnect = media_uri->InvalidAfterConnect; //TODO
    priv->invalidAfterReboot = media_uri->InvalidAfterReboot; //TODO

    if(media_uri->Timeout){
        if(!priv->timeout){
            priv->timeout = malloc(strlen(media_uri->Timeout) + 1);
        } else {
            priv->timeout = realloc(priv->timeout,strlen(media_uri->Timeout) + 1);
        }
        strcpy(priv->timeout,media_uri->Timeout);
    } else if(!media_uri->Timeout && priv->timeout){
        free(priv->timeout);
        priv->timeout = NULL;
    }
}

static void
OnvifMediaUri__dispose (GObject *self)
{
    OnvifMediaUriPrivate *priv = OnvifMediaUri__get_instance_private (ONVIF_MEDIA_URI(self));
    if(priv->uri){
        free(priv->uri);
        priv->uri = NULL;
    }

    if(priv->timeout){
        free(priv->timeout);
        priv->timeout = NULL;
    }
    G_OBJECT_CLASS (OnvifMediaUri__parent_class)->dispose (self);
}

static void
OnvifMediaUri__class_init (OnvifMediaUriClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    
    object_class->dispose = OnvifMediaUri__dispose;
    soapobj_class->construct = OnvifMediaUri__construct;
}

static void
OnvifMediaUri__init (OnvifMediaUri * self)
{
    OnvifMediaUriPrivate *priv = OnvifMediaUri__get_instance_private (ONVIF_MEDIA_URI(self));
    priv->uri = NULL;
    priv->invalidAfterConnect = -1;
    priv->invalidAfterReboot = -1;
    priv->timeout = NULL;

}

char * OnvifMediaUri__get_uri(OnvifMediaUri* self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_URI (self), 0);
    
    OnvifMediaUriPrivate *priv = OnvifMediaUri__get_instance_private (ONVIF_MEDIA_URI(self));
    return priv->uri;
}

OnvifMediaUri* OnvifMediaUri__new(struct tt__MediaUri * resp){
    return g_object_new (ONVIF_TYPE_MEDIA_URI, "data", resp, NULL);
}

OnvifMediaUri * OnvifSnapshotUri__new(struct _trt__GetSnapshotUriResponse * resp){
    if(!resp){
        return g_object_new (ONVIF_TYPE_MEDIA_URI, "data", NULL, NULL);
    } else {
        return g_object_new (ONVIF_TYPE_MEDIA_URI, "data", resp->MediaUri, NULL);
    }
}

OnvifMediaUri * OnvifStreamUri__new(struct _trt__GetStreamUriResponse * resp){
    if(!resp){
        return g_object_new (ONVIF_TYPE_MEDIA_URI, NULL);
    } else {
        return g_object_new (ONVIF_TYPE_MEDIA_URI, "data", resp->MediaUri, NULL);
    }
}