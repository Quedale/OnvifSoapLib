
#include "onvif_media_profile_local.h"
#include "clogger.h"

enum {
    PROP_TOKEN = 1,
    PROP_NAME = 2,
    PROP_FIXED = 3,
    PROP_INDEX = 4,
    PROP_AUDIO_SOURCE = 5,
    N_PROPERTIES
};

typedef struct {
    int index;
    char * name;
    char * token;
    gboolean fixed;
    OnvifMediaAudioSourceConfig * audio_source;
} OnvifMediaProfilePrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (OnvifMediaProfile, OnvifMediaProfile_, SOAP_TYPE_OBJECT)

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

void
OnvifMediaProfile__set_audio_source(OnvifMediaProfile * self, OnvifMediaAudioSourceConfig * audio_source){
    g_return_if_fail(self != NULL);
    g_return_if_fail(ONVIF_IS_MEDIA_PROFILE(self));
    OnvifMediaProfilePrivate *priv = OnvifMediaProfile__get_instance_private (self);
    if(priv->audio_source){
        C_WARN("AudioSource configuration being overwritten");
        g_object_unref(priv->audio_source);
    }
    priv->audio_source = audio_source;
}

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
        case PROP_FIXED:
            priv->fixed = g_value_get_boolean(value);
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
        case PROP_FIXED:
            g_value_set_boolean(value, priv->fixed);
            break;
        case PROP_AUDIO_SOURCE:
            g_value_set_object(value, priv->audio_source);
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
    if(priv->audio_source){
        g_object_unref(priv->audio_source);
        priv->audio_source = NULL;
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

    obj_properties[PROP_FIXED] =
        g_param_spec_boolean ("fixed",
                            "FixedProfile",
                            "A value of true signals that the profile cannot be deleted. Default is false.",
                            FALSE,
                            G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

    obj_properties[PROP_AUDIO_SOURCE] =
        g_param_spec_object ("audiosource",
                            "OnvifMediaAudioSourceConfiguration",
                            "Optional configuration of the Audio input.",
                            ONVIF_TYPE_MEDIA_AUDIO_SOURCE_CONFIG  /* default value */,
                            G_PARAM_READABLE);

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