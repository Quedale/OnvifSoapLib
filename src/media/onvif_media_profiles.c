
#include "onvif_media_profiles.h"

typedef struct {
    OnvifMediaProfile ** profiles;
    int count;
} OnvifMediaProfilesPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (OnvifMediaProfiles, OnvifMediaProfiles_, SOAP_TYPE_OBJECT)

static void
OnvifMediaProfiles__dispose (GObject *self){
    OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (ONVIF_MEDIA_PROFILES(self));
    if(priv->profiles){
        for (int i = 0; i < priv->count; i++){
            g_object_unref(priv->profiles[i]);
        }
        priv->count = 0;
        free(priv->profiles);
        priv->profiles = NULL;
    }
    G_OBJECT_CLASS (OnvifMediaProfiles__parent_class)->dispose (self);
}

static void
OnvifMediaProfiles__class_init (OnvifMediaProfilesClass * klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifMediaProfiles__dispose;
}

static void
OnvifMediaProfiles__init (OnvifMediaProfiles * self){
    OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (self);
    priv->count = 0;
    priv->profiles = NULL;
}

int 
OnvifMediaProfiles__get_size(OnvifMediaProfiles * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_PROFILES (self), 0);
    
    OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (ONVIF_MEDIA_PROFILES(self));
    return priv->count;
}

void 
OnvifMediaProfiles__insert(OnvifMediaProfiles * self,OnvifMediaProfile * profile){
    OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (ONVIF_MEDIA_PROFILES(self));
    priv->profiles = realloc (priv->profiles,sizeof (void *) * (priv->count+1));
    priv->profiles[priv->count]=profile;
    priv->count++;
}

OnvifMediaProfile * 
OnvifMediaProfiles__get_profile(OnvifMediaProfiles * self,int index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_PROFILES (self), NULL);
    
    OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (ONVIF_MEDIA_PROFILES(self));
    g_return_val_if_fail (index < priv->count, NULL);
    g_return_val_if_fail (index >= 0, NULL);

    return priv->profiles[index];
}