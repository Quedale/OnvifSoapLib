#include "onvif_media_snapshot_uri_local.h"
#include "clogger.h"

enum
{
  PROP_SOAP = 1,
  N_PROPERTIES
};

typedef struct {
    char * uri;
    int invalidAfterConnect;
    int invalidAfterReboot;
    char * timeout;
} OnvifSnapshotUriPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifSnapshotUri, OnvifSnapshotUri_, SOAP_TYPE_OBJECT)
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
OnvifSnapshotUri__set_soap(OnvifSnapshotUri * self, struct _trt__GetSnapshotUriResponse * resp){
    OnvifSnapshotUriPrivate *priv = OnvifSnapshotUri__get_instance_private (self);
    if(!resp){
        if(priv->uri) free(priv->uri);
        priv->uri = NULL;
        priv->invalidAfterConnect = -1; //TODO
        priv->invalidAfterReboot = -1; //TODO
        if(priv->timeout) free(priv->timeout);
        priv->timeout = NULL; //TODO
        return;
    }

    if(resp->MediaUri->Uri){
        if(!priv->uri){
            priv->uri = malloc(strlen(resp->MediaUri->Uri) + 1);
        } else {
            priv->uri = realloc(priv->uri,strlen(resp->MediaUri->Uri) + 1);
        }
        strcpy(priv->uri,resp->MediaUri->Uri);
    } else if(!resp->MediaUri->Uri && priv->uri){
        free(priv->uri);
        priv->uri = NULL;
    }

    priv->invalidAfterConnect = resp->MediaUri->InvalidAfterConnect; //TODO
    priv->invalidAfterReboot = resp->MediaUri->InvalidAfterReboot; //TODO

    if(resp->MediaUri->Timeout){
        if(!priv->timeout){
            priv->timeout = malloc(strlen(resp->MediaUri->Timeout) + 1);
        } else {
            priv->timeout = realloc(priv->timeout,strlen(resp->MediaUri->Timeout) + 1);
        }
        strcpy(priv->timeout,resp->MediaUri->Timeout);
    } else if(!resp->MediaUri->Timeout && priv->timeout){
        free(priv->timeout);
        priv->timeout = NULL;
    }
}

static void
OnvifSnapshotUri__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    OnvifSnapshotUri * self = ONVIF_SNAPSHOTURI (object);
    // OnvifSnapshotUriPrivate *priv = OnvifSnapshotUri__get_instance_private (self);
    switch (prop_id){
        case PROP_SOAP:
            OnvifSnapshotUri__set_soap(self,g_value_get_pointer (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifSnapshotUri__get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    // OnvifSnapshotUri *thread = ONVIF_SNAPSHOTURI (object);
    // OnvifSnapshotUriPrivate *priv = OnvifSnapshotUri__get_instance_private (thread);
    switch (prop_id){
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifSnapshotUri__dispose (GObject *self)
{
    OnvifSnapshotUriPrivate *priv = OnvifSnapshotUri__get_instance_private (ONVIF_SNAPSHOTURI(self));
    if(priv->uri){
        free(priv->uri);
        priv->uri = NULL;
    }

    if(priv->timeout){
        free(priv->timeout);
        priv->timeout = NULL;
    }
}

static void
OnvifSnapshotUri__class_init (OnvifSnapshotUriClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifSnapshotUri__dispose;
    object_class->set_property = OnvifSnapshotUri__set_property;
    object_class->get_property = OnvifSnapshotUri__get_property;
    obj_properties[PROP_SOAP] =
        g_param_spec_pointer ("soap",
                            "SoapMessage",
                            "Pointer to original soap message.",
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);

    g_object_class_install_properties (object_class,
                                        N_PROPERTIES,
                                        obj_properties);
}

static void
OnvifSnapshotUri__init (OnvifSnapshotUri * self)
{
    OnvifSnapshotUriPrivate *priv = OnvifSnapshotUri__get_instance_private (ONVIF_SNAPSHOTURI(self));
    priv->uri = NULL;
    priv->invalidAfterConnect = -1;
    priv->invalidAfterReboot = -1;
    priv->timeout = NULL;

}

char * OnvifSnapshotUri__get_uri(OnvifSnapshotUri* self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_SNAPSHOTURI (self), 0);
    
    OnvifSnapshotUriPrivate *priv = OnvifSnapshotUri__get_instance_private (ONVIF_SNAPSHOTURI(self));
    return priv->uri;
}

OnvifSnapshotUri* OnvifSnapshotUri__new(struct _trt__GetSnapshotUriResponse * resp){
    return g_object_new (ONVIF_TYPE_SNAPSHOTURI, "soap", resp, NULL);
}