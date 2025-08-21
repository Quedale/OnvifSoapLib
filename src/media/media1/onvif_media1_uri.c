#include "onvif_media1_uri_local.h"
#include "../onvif_uri_local.h"

typedef struct {
    int invalidAfterConnect;
    int invalidAfterReboot;
    char * timeout;
} OnvifMedia1UriPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifMedia1Uri, OnvifMedia1Uri_, ONVIF_TYPE_URI)

static void
OnvifMedia1Uri__construct(SoapObject * obj, gpointer ptr){
    OnvifMedia1Uri * self = ONVIF_MEDIA1_URI(obj);
    struct tt__MediaUri * media_uri = ptr;
    OnvifMedia1UriPrivate *priv = OnvifMedia1Uri__get_instance_private (self);
    if(!media_uri){
        OnvifUri__set_uri(ONVIF_URI(self),NULL);
        priv->invalidAfterConnect = -1;
        priv->invalidAfterReboot = -1;
        if(priv->timeout) free(priv->timeout);
        priv->timeout = NULL;
        return;
    }

    OnvifUri__set_uri(ONVIF_URI(self),media_uri->Uri);

    priv->invalidAfterConnect = media_uri->InvalidAfterConnect;
    priv->invalidAfterReboot = media_uri->InvalidAfterReboot;

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
OnvifMedia1Uri__dispose (GObject *self){
    OnvifMedia1UriPrivate *priv = OnvifMedia1Uri__get_instance_private (ONVIF_MEDIA1_URI(self));

    if(priv->timeout){
        free(priv->timeout);
        priv->timeout = NULL;
    }
    G_OBJECT_CLASS (OnvifMedia1Uri__parent_class)->dispose (self);
}

static void
OnvifMedia1Uri__class_init (OnvifMedia1UriClass * klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    
    object_class->dispose = OnvifMedia1Uri__dispose;
    soapobj_class->construct = OnvifMedia1Uri__construct;
}

static void
OnvifMedia1Uri__init (OnvifMedia1Uri * self){
    OnvifMedia1UriPrivate *priv = OnvifMedia1Uri__get_instance_private (ONVIF_MEDIA1_URI(self));
    priv->invalidAfterConnect = -1;
    priv->invalidAfterReboot = -1;
    priv->timeout = NULL;

}

OnvifMedia1Uri * 
OnvifMedia1SnapshotUri__new(struct _trt__GetSnapshotUriResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA1_URI, "data", (!resp) ? NULL : resp->MediaUri, NULL);
}

OnvifMedia1Uri * 
OnvifMedia1StreamUri__new(struct _trt__GetStreamUriResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA1_URI, "data", (!resp) ? NULL : resp->MediaUri, NULL);
}
