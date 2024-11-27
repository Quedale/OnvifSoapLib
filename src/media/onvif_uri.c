
#include "onvif_uri_local.h"

typedef struct {
    char * uri;
} OnvifUriPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (OnvifUri, OnvifUri_, SOAP_TYPE_OBJECT)

static void
OnvifUri__dispose (GObject *self){
    OnvifUriPrivate *priv = OnvifUri__get_instance_private (ONVIF_URI(self));
    if(priv->uri){
        free(priv->uri);
        priv->uri = NULL;
    }
    G_OBJECT_CLASS (OnvifUri__parent_class)->dispose (self);
}

static void
OnvifUri__class_init (OnvifUriClass * klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifUri__dispose;
}

static void
OnvifUri__init (OnvifUri * self){
    OnvifUriPrivate *priv = OnvifUri__get_instance_private (self);
    priv->uri = NULL;
}

char* 
OnvifUri__get_uri(OnvifUri * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_URI (self), 0);
    
    OnvifUriPrivate *priv = OnvifUri__get_instance_private (ONVIF_URI(self));
    return priv->uri;
}

void 
OnvifUri__set_uri(OnvifUri* self, char * uri){
    g_return_if_fail (self != NULL);
    g_return_if_fail (ONVIF_IS_URI (self));
    OnvifUriPrivate *priv = OnvifUri__get_instance_private (ONVIF_URI(self));
    if(!uri && priv->uri){
        free(priv->uri);
        priv->uri = NULL;
    }

    if(!uri) return;

    priv->uri = malloc(strlen(uri)+1);
    strcpy(priv->uri,uri);
}