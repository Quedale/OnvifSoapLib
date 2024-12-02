
#include "onvif_media_audio_output_configs_local.h"

typedef struct {
    OnvifMediaAudioOutputConfig ** configs;
    int count;
} OnvifMediaAudioOutputConfigsPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (OnvifMediaAudioOutputConfigs, OnvifMediaAudioOutputConfigs_, SOAP_TYPE_OBJECT)

static void
OnvifMediaAudioOutputConfigs__dispose (GObject *self){
    OnvifMediaAudioOutputConfigsPrivate *priv = OnvifMediaAudioOutputConfigs__get_instance_private (ONVIF_MEDIA_AUDIO_OUTPUT_CONFIGS(self));
    if(priv->configs){
        for (int i = 0; i < priv->count; i++){
            g_object_unref(priv->configs[i]);
        }
        priv->count = 0;
        free(priv->configs);
        priv->configs = NULL;
    }
    G_OBJECT_CLASS (OnvifMediaAudioOutputConfigs__parent_class)->dispose (self);
}

static void
OnvifMediaAudioOutputConfigs__class_init (OnvifMediaAudioOutputConfigsClass * klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifMediaAudioOutputConfigs__dispose;
}

static void
OnvifMediaAudioOutputConfigs__init (OnvifMediaAudioOutputConfigs * self){
    OnvifMediaAudioOutputConfigsPrivate *priv = OnvifMediaAudioOutputConfigs__get_instance_private (self);
    priv->count = 0;
    priv->configs = NULL;
}

int 
OnvifMediaAudioOutputConfigs__get_size(OnvifMediaAudioOutputConfigs * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIGS (self), 0);
    
    OnvifMediaAudioOutputConfigsPrivate *priv = OnvifMediaAudioOutputConfigs__get_instance_private (self);
    return priv->count;
}

void 
OnvifMediaAudioOutputConfigs__insert(OnvifMediaAudioOutputConfigs * self,OnvifMediaAudioOutputConfig * config){
    OnvifMediaAudioOutputConfigsPrivate *priv = OnvifMediaAudioOutputConfigs__get_instance_private (self);
    priv->configs = realloc (priv->configs,sizeof (void *) * (priv->count+1));
    priv->configs[priv->count]=config;
    priv->count++;
}

OnvifMediaAudioOutputConfig * 
OnvifMediaAudioOutputConfigs__get_config(OnvifMediaAudioOutputConfigs * self,int index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_AUDIO_OUTPUT_CONFIGS (self), NULL);
    
    OnvifMediaAudioOutputConfigsPrivate *priv = OnvifMediaAudioOutputConfigs__get_instance_private (self);
    g_return_val_if_fail (index < priv->count, NULL);
    g_return_val_if_fail (index >= 0, NULL);

    return priv->configs[index];
}