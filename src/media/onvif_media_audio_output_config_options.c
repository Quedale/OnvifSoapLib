
#include "onvif_media_audio_output_config_options_local.h"

typedef struct {
    char ** ouput_tokens_available;
    int ouput_tokens_available_count;
    char ** send_primacy_options;
    int send_primacy_count;
    int min_level;
    int max_level;
} OnvifMediaAudioOutputConfigOptionsPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (OnvifMediaAudioOutputConfigOptions, OnvifMediaAudioOutputConfigOptions_, SOAP_TYPE_OBJECT)

void
OnvifMediaAudioOutputConfigOptions__set_option(OnvifMediaAudioOutputConfigOptions * self, struct tt__AudioOutputConfigurationOptions * data){
    g_return_if_fail (self != NULL);
    g_return_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS (self));
    OnvifMediaAudioOutputConfigOptionsPrivate *priv = OnvifMediaAudioOutputConfigOptions__get_instance_private (self);

    if(!data){
        return;
    }

    priv->ouput_tokens_available_count = data->__sizeOutputTokensAvailable;
    if(data->__sizeOutputTokensAvailable) priv->ouput_tokens_available = malloc(sizeof(char*) * data->__sizeOutputTokensAvailable);
    for(int i=0;i<data->__sizeOutputTokensAvailable;i++){
        priv->ouput_tokens_available[i] = malloc(strlen(data->OutputTokensAvailable[i])+1);
        strcpy(priv->ouput_tokens_available[i],data->OutputTokensAvailable[i]);
    }

    priv->send_primacy_count = data->__sizeSendPrimacyOptions;
    if(data->__sizeSendPrimacyOptions) priv->send_primacy_options = malloc(sizeof(char*) * data->__sizeSendPrimacyOptions);
    for(int i=0;i<data->__sizeSendPrimacyOptions;i++){
        priv->send_primacy_options[i] = malloc(strlen(data->SendPrimacyOptions[i])+1);
        strcpy(priv->send_primacy_options[i],data->SendPrimacyOptions[i]);
    }

    if(data->OutputLevelRange){
        priv->min_level = data->OutputLevelRange->Min;
        priv->max_level = data->OutputLevelRange->Max;
    }
}

static void
OnvifMediaAudioOutputConfigOptions__dispose (GObject *self){
    OnvifMediaAudioOutputConfigOptionsPrivate *priv = OnvifMediaAudioOutputConfigOptions__get_instance_private (ONVIF_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS(self));
    if(priv->ouput_tokens_available){
        for(int i=0;i<priv->ouput_tokens_available_count;i++){
            free(priv->ouput_tokens_available[i]);
            priv->ouput_tokens_available[i] = NULL;
        }
        free(priv->ouput_tokens_available);
        priv->ouput_tokens_available = NULL;
        priv->ouput_tokens_available_count = 0;
    }

    if(priv->send_primacy_options){
        for(int i=0;i<priv->send_primacy_count;i++){
            free(priv->send_primacy_options[i]);
            priv->send_primacy_options[i] = NULL;
        }
        free(priv->send_primacy_options);
        priv->send_primacy_options = NULL;
        priv->send_primacy_count = 0;
    }
    priv->min_level = 0;
    priv->max_level = 0;
    G_OBJECT_CLASS (OnvifMediaAudioOutputConfigOptions__parent_class)->dispose (self);
}

static void
OnvifMediaAudioOutputConfigOptions__class_init (OnvifMediaAudioOutputConfigOptionsClass * klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifMediaAudioOutputConfigOptions__dispose;
}

static void
OnvifMediaAudioOutputConfigOptions__init (OnvifMediaAudioOutputConfigOptions * self){
    OnvifMediaAudioOutputConfigOptionsPrivate *priv = OnvifMediaAudioOutputConfigOptions__get_instance_private (self);
    priv->ouput_tokens_available = NULL;
    priv->ouput_tokens_available_count = 0;
    priv->send_primacy_options = NULL;
    priv->send_primacy_count = 0;
    priv->min_level = 0;
    priv->max_level = 0;
}

char * 
OnvifMediaAudioOutputConfigOptions__get_ouput_token_available(OnvifMediaAudioOutputConfigOptions * self, int index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS (self), NULL);
    OnvifMediaAudioOutputConfigOptionsPrivate *priv = OnvifMediaAudioOutputConfigOptions__get_instance_private (self);
    g_return_val_if_fail (index < priv->ouput_tokens_available_count, NULL);
    return priv->ouput_tokens_available[index];
}

int 
OnvifMediaAudioOutputConfigOptions__ouput_tokens_available_count(OnvifMediaAudioOutputConfigOptions * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS (self), 0);
    OnvifMediaAudioOutputConfigOptionsPrivate *priv = OnvifMediaAudioOutputConfigOptions__get_instance_private (self);
    return priv->ouput_tokens_available_count;
}

char * 
OnvifMediaAudioOutputConfigOptions__get_send_primacy_option(OnvifMediaAudioOutputConfigOptions * self, int index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS (self), NULL);
    OnvifMediaAudioOutputConfigOptionsPrivate *priv = OnvifMediaAudioOutputConfigOptions__get_instance_private (self);
    g_return_val_if_fail (index < priv->send_primacy_count, NULL);
    return priv->send_primacy_options[index];
}

int 
OnvifMediaAudioOutputConfigOptions__get_send_primacy_options_count(OnvifMediaAudioOutputConfigOptions * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS (self), 0);
    OnvifMediaAudioOutputConfigOptionsPrivate *priv = OnvifMediaAudioOutputConfigOptions__get_instance_private (self);
    return priv->send_primacy_count;
}

int 
OnvifMediaAudioOutputConfigOptions__get_min_level(OnvifMediaAudioOutputConfigOptions * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS (self), 0);
    OnvifMediaAudioOutputConfigOptionsPrivate *priv = OnvifMediaAudioOutputConfigOptions__get_instance_private (self);
    return priv->min_level;
}

int 
OnvifMediaAudioOutputConfigOptions__get_max_level(OnvifMediaAudioOutputConfigOptions * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIG_OPTIONS (self), 0);
    OnvifMediaAudioOutputConfigOptionsPrivate *priv = OnvifMediaAudioOutputConfigOptions__get_instance_private (self);
    return priv->max_level;
}
