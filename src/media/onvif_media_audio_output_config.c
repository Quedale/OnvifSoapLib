#include "onvif_media_audio_output_config_local.h"

typedef struct {
    char * token;
    char * name;
    int use_count;
    char * output_token;
} OnvifMediaAudioOutputConfigPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (OnvifMediaAudioOutputConfig, OnvifMediaAudioOutputConfig_, SOAP_TYPE_OBJECT)

char * 
OnvifMediaAudioOutputConfig__get_token(OnvifMediaAudioOutputConfig * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIG (self), NULL);
    OnvifMediaAudioOutputConfigPrivate *priv = OnvifMediaAudioOutputConfig__get_instance_private (self);
    return priv->token;
}

char * 
OnvifMediaAudioOutputConfig__get_name(OnvifMediaAudioOutputConfig * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIG (self), NULL);
    OnvifMediaAudioOutputConfigPrivate *priv = OnvifMediaAudioOutputConfig__get_instance_private (self);
    return priv->name;
}

int 
OnvifMediaAudioOutputConfig__get_use_count(OnvifMediaAudioOutputConfig * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIG (self), 0);
    OnvifMediaAudioOutputConfigPrivate *priv = OnvifMediaAudioOutputConfig__get_instance_private (self);
    return priv->use_count;
}

char * 
OnvifMediaAudioOutputConfig__get_output_token(OnvifMediaAudioOutputConfig * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIG (self), NULL);
    OnvifMediaAudioOutputConfigPrivate *priv = OnvifMediaAudioOutputConfig__get_instance_private (self);
    return priv->output_token;
}

static void
OnvifMediaAudioOutputConfig__construct(SoapObject * obj, gpointer ptr){
    OnvifMediaAudioOutputConfig * self = ONVIF_MEDIA_AUDIO_OUTPUT_CONFIG(obj);
    struct tt__AudioOutputConfiguration * resp = ptr;
    OnvifMediaAudioOutputConfigPrivate *priv = OnvifMediaAudioOutputConfig__get_instance_private (self);

    if(!resp){
        return;
    }

    if(priv->token){
        free(priv->token);
        priv->token = NULL;
    }
    if(resp->token){
        priv->token = malloc(strlen(resp->token)+1);
        strcpy(priv->token,resp->token);
    }

    if(priv->name){
        free(priv->name);
        priv->name = NULL;
    }
    if(resp->Name){
        priv->name = malloc(strlen(resp->Name)+1);
        strcpy(priv->name,resp->Name);
    }

    priv->use_count = resp->UseCount;

    if(priv->output_token){
        free(priv->output_token);
        priv->output_token = NULL;
    }
    if(resp->OutputToken){
        priv->output_token = malloc(strlen(resp->OutputToken)+1);
        strcpy(priv->output_token,resp->OutputToken);
    }
}

static void
OnvifMediaAudioOutputConfig__dispose(GObject * object){
    OnvifMediaAudioOutputConfigPrivate *priv = OnvifMediaAudioOutputConfig__get_instance_private (ONVIF_MEDIA_AUDIO_OUTPUT_CONFIG(object));
    if(priv->token){
        free(priv->token);
        priv->token = NULL;
    }
    if(priv->name){
        free(priv->name);
        priv->name = NULL;
    }

    priv->use_count = 0;

    if(priv->output_token){
        free(priv->output_token);
        priv->output_token = NULL;
    }
    G_OBJECT_CLASS (OnvifMediaAudioOutputConfig__parent_class)->dispose (object);
}

static void
OnvifMediaAudioOutputConfig__class_init (OnvifMediaAudioOutputConfigClass * klass){
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = OnvifMediaAudioOutputConfig__dispose;
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMediaAudioOutputConfig__construct;
}

static void
OnvifMediaAudioOutputConfig__init (OnvifMediaAudioOutputConfig * self){
    OnvifMediaAudioOutputConfigPrivate *priv = OnvifMediaAudioOutputConfig__get_instance_private (self);
    priv->token = NULL;
    priv->name = NULL;
    priv->output_token = NULL;
    priv->use_count = -1;
}


OnvifMediaAudioOutputConfig * 
OnvifMediaAudioOutputConfig__new(struct tt__AudioOutputConfiguration * resp){
    return g_object_new (ONVIF_TYPE_MEDIA_AUDIO_OUTPUT_CONFIG, "data", resp, NULL);
}