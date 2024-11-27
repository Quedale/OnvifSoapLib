#include "onvif_media1_profile_local.h"

// enum {
//     // PROP_TOKEN = 1,
//     // PROP_NAME = 2,
//     // PROP_INDEX = 3,
//     N_PROPERTIES
// };

typedef struct {
    // int index;
    // char * name;
    // char * token;
    char * placeholder;
} OnvifMedia1ProfilePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifMedia1Profile, OnvifMedia1Profile_, ONVIF_TYPE_MEDIA_PROFILE)

// static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
OnvifMedia1Profile__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec){
    // OnvifMedia1Profile * self = ONVIF_MEDIA1_PROFILE (object);
    // OnvifMedia1ProfilePrivate *priv = OnvifMedia1Profile__get_instance_private (self);
    // char * strval;

    switch (prop_id){
        // case PROP_TOKEN:
        //     if(priv->token){
        //         free(priv->token);
        //         priv->token = NULL;
        //     }
        //     strval = (char*) g_value_get_string (value);
        //     if(strval){
        //         priv->token = malloc(strlen(strval)+1);
        //         strcpy(priv->token,strval);
        //     }
        //     break;
        // case PROP_NAME:
        //     if(priv->name){
        //         free(priv->name);
        //         priv->name = NULL;
        //     }
        //     strval = (char*)g_value_get_string (value);
        //     if(strval){
        //         priv->name = malloc(strlen(strval)+1);
        //         strcpy(priv->name,strval);
        //     }
        //     break;
        // case PROP_INDEX:
        //     priv->index = g_value_get_int (value);
        //     break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifMedia1Profile__get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec){
    // OnvifMedia1Profile *self = ONVIF_MEDIA1_PROFILE (object);
    // OnvifMedia1ProfilePrivate *priv = OnvifMedia1Profile__get_instance_private (self);
    switch (prop_id){
        // case PROP_TOKEN:
        //     g_value_set_string (value, priv->token);
        //     break;
        // case PROP_NAME:
        //     g_value_set_string (value, priv->name);
        //     break;
        // case PROP_INDEX:
        //     g_value_set_int (value, priv->index);
        //     break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifMedia1Profile__dispose (GObject *self){
    // OnvifMedia1ProfilePrivate *priv = OnvifMedia1Profile__get_instance_private (ONVIF_MEDIA1_PROFILE(self));
    G_OBJECT_CLASS (OnvifMedia1Profile__parent_class)->dispose (self);
}

static void
OnvifMedia1Profile__class_init (OnvifMedia1ProfileClass * klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifMedia1Profile__dispose;
    object_class->set_property = OnvifMedia1Profile__set_property;
    object_class->get_property = OnvifMedia1Profile__get_property;

    // obj_properties[PROP_TOKEN] =
    //     g_param_spec_string ("token",
    //                         "ProfileToken",
    //                         "Onvif Media Profile token",
    //                         NULL,
    //                         G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

    // obj_properties[PROP_NAME] =
    //     g_param_spec_string ("name",
    //                         "ProfileName",
    //                         "Onvif Media Profile name",
    //                         NULL,
    //                         G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

    // obj_properties[PROP_INDEX] =
    //     g_param_spec_int ("index",
    //                         "ProfileIndex",
    //                         "Onvif Media Profile index",
    //                         -1, G_MAXINT, -1,
    //                         G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

    // g_object_class_install_properties (object_class,
    //                                     N_PROPERTIES,
    //                                     obj_properties);
}

static void
OnvifMedia1Profile__init (OnvifMedia1Profile * self){
    // OnvifMedia1ProfilePrivate *priv = OnvifMedia1Profile__get_instance_private (self);
    // priv->name = NULL;
    // priv->token = NULL;
    // priv->index = -1;
}

OnvifMedia1Profile * 
OnvifMedia1Profile__new(struct tt__Profile * profile, int index){
    return g_object_new(ONVIF_TYPE_MEDIA1_PROFILE,
                        "token",profile->token,
                        "name",profile->Name,
                        "index",index,
                        NULL);
}