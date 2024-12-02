
#include "onvif_media_audio_source_configs_local.h"

typedef struct {
    OnvifMediaAudioSourceConfig ** configs;
    int count;
} OnvifMediaAudioSourceConfigsPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (OnvifMediaAudioSourceConfigs, OnvifMediaAudioSourceConfigs_, SOAP_TYPE_OBJECT)

static void
OnvifMediaAudioSourceConfigs__dispose (GObject *self){
    OnvifMediaAudioSourceConfigsPrivate *priv = OnvifMediaAudioSourceConfigs__get_instance_private (ONVIF_MEDIA_AUDIO_SOURCE_CONFIGS(self));
    if(priv->configs){
        for (int i = 0; i < priv->count; i++){
            g_object_unref(priv->configs[i]);
        }
        priv->count = 0;
        free(priv->configs);
        priv->configs = NULL;
    }
    G_OBJECT_CLASS (OnvifMediaAudioSourceConfigs__parent_class)->dispose (self);
}

static void
OnvifMediaAudioSourceConfigs__class_init (OnvifMediaAudioSourceConfigsClass * klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifMediaAudioSourceConfigs__dispose;
}

static void
OnvifMediaAudioSourceConfigs__init (OnvifMediaAudioSourceConfigs * self){
    OnvifMediaAudioSourceConfigsPrivate *priv = OnvifMediaAudioSourceConfigs__get_instance_private (self);
    priv->count = 0;
    priv->configs = NULL;
}

int 
OnvifMediaAudioSourceConfigs__get_size(OnvifMediaAudioSourceConfigs * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_SOURCE_CONFIGS (self), 0);
    
    OnvifMediaAudioSourceConfigsPrivate *priv = OnvifMediaAudioSourceConfigs__get_instance_private (self);
    return priv->count;
}

void 
OnvifMediaAudioSourceConfigs__insert(OnvifMediaAudioSourceConfigs * self,OnvifMediaAudioSourceConfig * config){
    OnvifMediaAudioSourceConfigsPrivate *priv = OnvifMediaAudioSourceConfigs__get_instance_private (self);
    priv->configs = realloc (priv->configs,sizeof (void *) * (priv->count+1));
    priv->configs[priv->count]=config;
    priv->count++;
}

OnvifMediaAudioSourceConfig * 
OnvifMediaAudioSourceConfigs__get_config(OnvifMediaAudioSourceConfigs * self,int index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_SOURCE_CONFIGS (self), NULL);
    
    OnvifMediaAudioSourceConfigsPrivate *priv = OnvifMediaAudioSourceConfigs__get_instance_private (self);
    g_return_val_if_fail (index < priv->count, NULL);
    g_return_val_if_fail (index >= 0, NULL);

    return priv->configs[index];
}