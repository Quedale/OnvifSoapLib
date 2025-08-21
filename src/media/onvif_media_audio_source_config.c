#include "onvif_media_audio_source_config_local.h"

typedef struct {
    char * token;
    char * name;
    int use_count;
    char * source_token;
} OnvifMediaAudioSourceConfigPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (OnvifMediaAudioSourceConfig, OnvifMediaAudioSourceConfig_, SOAP_TYPE_OBJECT)

char * 
OnvifMediaAudioSourceConfig__get_token(OnvifMediaAudioSourceConfig * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_SOURCE_CONFIG (self), NULL);
    OnvifMediaAudioSourceConfigPrivate *priv = OnvifMediaAudioSourceConfig__get_instance_private (self);
    return priv->token;
}

char * 
OnvifMediaAudioSourceConfig__get_name(OnvifMediaAudioSourceConfig * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_SOURCE_CONFIG (self), NULL);
    OnvifMediaAudioSourceConfigPrivate *priv = OnvifMediaAudioSourceConfig__get_instance_private (self);
    return priv->name;
}

int 
OnvifMediaAudioSourceConfig__get_use_count(OnvifMediaAudioSourceConfig * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_SOURCE_CONFIG (self), 0);
    OnvifMediaAudioSourceConfigPrivate *priv = OnvifMediaAudioSourceConfig__get_instance_private (self);
    return priv->use_count;
}

char * 
OnvifMediaAudioSourceConfig__get_source_token(OnvifMediaAudioSourceConfig * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_SOURCE_CONFIG (self), NULL);
    OnvifMediaAudioSourceConfigPrivate *priv = OnvifMediaAudioSourceConfig__get_instance_private (self);
    return priv->source_token;
}

static void
OnvifMediaAudioSourceConfig__construct(SoapObject * obj, gpointer ptr){
    OnvifMediaAudioSourceConfig * self = ONVIF_MEDIA_AUDIO_SOURCE_CONFIG(obj);
    struct tt__AudioSourceConfiguration * resp = ptr;
    OnvifMediaAudioSourceConfigPrivate *priv = OnvifMediaAudioSourceConfig__get_instance_private (self);

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

    if(priv->source_token){
        free(priv->source_token);
        priv->source_token = NULL;
    }
    if(resp->SourceToken){
        priv->source_token = malloc(strlen(resp->SourceToken)+1);
        strcpy(priv->source_token,resp->SourceToken);
    }
}

static void
OnvifMediaAudioSourceConfig__dispose(GObject * object){
    OnvifMediaAudioSourceConfigPrivate *priv = OnvifMediaAudioSourceConfig__get_instance_private (ONVIF_MEDIA_AUDIO_SOURCE_CONFIG(object));
    if(priv->token){
        free(priv->token);
        priv->token = NULL;
    }

    if(priv->name){
        free(priv->name);
        priv->name = NULL;
    }
    priv->use_count = 0;
    if(priv->source_token){
        free(priv->source_token);
        priv->source_token = NULL;
    }
    G_OBJECT_CLASS (OnvifMediaAudioSourceConfig__parent_class)->dispose (object);
}

static void
OnvifMediaAudioSourceConfig__class_init (OnvifMediaAudioSourceConfigClass * klass){
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = OnvifMediaAudioSourceConfig__dispose;
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMediaAudioSourceConfig__construct;
}

static void
OnvifMediaAudioSourceConfig__init (OnvifMediaAudioSourceConfig * self){
    OnvifMediaAudioSourceConfigPrivate *priv = OnvifMediaAudioSourceConfig__get_instance_private (self);
    priv->token = NULL;
    priv->name = NULL;
    priv->source_token = NULL;
    priv->use_count = -1;
}


OnvifMediaAudioSourceConfig * 
OnvifMediaAudioSourceConfig__new(struct tt__AudioSourceConfiguration * resp){
    return g_object_new (ONVIF_TYPE_MEDIA_AUDIO_SOURCE_CONFIG, "data", resp, NULL);
}
