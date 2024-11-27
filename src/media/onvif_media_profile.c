
#include "onvif_media_profile.h"

enum {
    PROP_TOKEN = 1,
    PROP_NAME = 2,
    PROP_INDEX = 3,
    N_PROPERTIES
};

typedef struct {
    int index;
    char * name;
    char * token;
} OnvifMediaProfilePrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (OnvifMediaProfile, OnvifMediaProfile_, G_TYPE_OBJECT)

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
OnvifMediaProfile__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec){
    OnvifMediaProfile * self = ONVIF_MEDIA_PROFILE (object);
    OnvifMediaProfilePrivate *priv = OnvifMediaProfile__get_instance_private (self);
    char * strval;

    switch (prop_id){
        case PROP_TOKEN:
            if(priv->token){
                free(priv->token);
                priv->token = NULL;
            }
            strval = (char*) g_value_get_string (value);
            if(strval){
                priv->token = malloc(strlen(strval)+1);
                strcpy(priv->token,strval);
            }
            break;
        case PROP_NAME:
            if(priv->name){
                free(priv->name);
                priv->name = NULL;
            }
            strval = (char*)g_value_get_string (value);
            if(strval){
                priv->name = malloc(strlen(strval)+1);
                strcpy(priv->name,strval);
            }
            break;
        case PROP_INDEX:
            priv->index = g_value_get_int (value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifMediaProfile__get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec){
    OnvifMediaProfile *self = ONVIF_MEDIA_PROFILE (object);
    OnvifMediaProfilePrivate *priv = OnvifMediaProfile__get_instance_private (self);
    switch (prop_id){
        case PROP_TOKEN:
            g_value_set_string (value, priv->token);
            break;
        case PROP_NAME:
            g_value_set_string (value, priv->name);
            break;
        case PROP_INDEX:
            g_value_set_int (value, priv->index);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifMediaProfile__dispose (GObject *self){
    OnvifMediaProfilePrivate *priv = OnvifMediaProfile__get_instance_private (ONVIF_MEDIA_PROFILE(self));
    if(priv->name){
        free(priv->name);
        priv->name = NULL;
    }
    if(priv->token){
        free(priv->token);
        priv->token = NULL;
    }
    G_OBJECT_CLASS (OnvifMediaProfile__parent_class)->dispose (self);
}

static void
OnvifMediaProfile__class_init (OnvifMediaProfileClass * klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifMediaProfile__dispose;
    object_class->set_property = OnvifMediaProfile__set_property;
    object_class->get_property = OnvifMediaProfile__get_property;

    obj_properties[PROP_TOKEN] =
        g_param_spec_string ("token",
                            "ProfileToken",
                            "Onvif Media Profile token",
                            NULL,
                            G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

    obj_properties[PROP_NAME] =
        g_param_spec_string ("name",
                            "ProfileName",
                            "Onvif Media Profile name",
                            NULL,
                            G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

    obj_properties[PROP_INDEX] =
        g_param_spec_int ("index",
                            "ProfileIndex",
                            "Onvif Media Profile index",
                            -1, G_MAXINT, -1,
                            G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

    g_object_class_install_properties (object_class,
                                        N_PROPERTIES,
                                        obj_properties);
}

static void
OnvifMediaProfile__init (OnvifMediaProfile * self){
    OnvifMediaProfilePrivate *priv = OnvifMediaProfile__get_instance_private (self);
    priv->name = NULL;
    priv->token = NULL;
    priv->index = -1;
}

const char * 
OnvifMediaProfile__get_name(OnvifMediaProfile* self){
    g_return_val_if_fail(self != NULL, NULL);
    g_return_val_if_fail(ONVIF_IS_MEDIA_PROFILE(self), NULL);
    OnvifMediaProfilePrivate *priv = OnvifMediaProfile__get_instance_private (self);
    return priv->name;
}

const char * 
OnvifMediaProfile__get_token(OnvifMediaProfile* self){
    g_return_val_if_fail(self != NULL, NULL);
    g_return_val_if_fail(ONVIF_IS_MEDIA_PROFILE(self), NULL);
    OnvifMediaProfilePrivate *priv = OnvifMediaProfile__get_instance_private (self);
    return priv->token;
}

int 
OnvifMediaProfile__get_index(OnvifMediaProfile* self){
    g_return_val_if_fail(self != NULL, -1);
    g_return_val_if_fail(ONVIF_IS_MEDIA_PROFILE(self), -1);
    OnvifMediaProfilePrivate *priv = OnvifMediaProfile__get_instance_private (self);
    return priv->index;
}