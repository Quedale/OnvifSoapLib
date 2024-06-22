#include "onvif_media_profile_local.h"
#include "clogger.h"

typedef struct _OnvifProfile {
    int copy;
    int index;
    char * name;
    char * token;
} OnvifProfile;

enum
{
  PROP_SOAP = 1,
  N_PROPERTIES
};

typedef struct {
    OnvifProfile ** profiles;
    int count;
} OnvifMediaProfilesPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifMediaProfiles, OnvifMediaProfiles_, SOAP_TYPE_OBJECT)
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
OnvifMediaProfiles__set_soap(OnvifMediaProfiles * self, struct _trt__GetProfilesResponse * resp){
    OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (self);
    if(priv->profiles){
        for (int i = 0; i < priv->count; i++){
            OnvifProfile__real_destroy(priv->profiles[i]);
        }
        priv->count = 0;
        priv->profiles = realloc(priv->profiles,0);
    } else {
        priv->count = 0;
        priv->profiles = malloc(0);
    }

    if(!resp){
        return;
    }

    for(int i = 0; i < resp->__sizeProfiles; i++){
        OnvifProfile * profile = OnvifProfile__create(&(resp->Profiles[i]),i);
        OnvifMediaProfiles__insert(self,profile);
    }
}

static void
OnvifMediaProfiles__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    OnvifMediaProfiles * self = ONVIF_MEDIA_PROFILES (object);
    // OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (self);
    switch (prop_id){
        case PROP_SOAP:
            OnvifMediaProfiles__set_soap(self,g_value_get_pointer (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifMediaProfiles__get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    // OnvifMediaProfiles *thread = ONVIF_TYPE_MEDIA_URI (object);
    // OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (thread);
    switch (prop_id){
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifMediaProfiles__dispose (GObject *self)
{
    OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (ONVIF_MEDIA_PROFILES(self));
    for (int i = 0; i < priv->count; i++){
        OnvifProfile__real_destroy(priv->profiles[i]);
    }
    free(priv->profiles);
    priv->count = 0;
}

static void
OnvifMediaProfiles__class_init (OnvifMediaProfilesClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifMediaProfiles__dispose;
    object_class->set_property = OnvifMediaProfiles__set_property;
    object_class->get_property = OnvifMediaProfiles__get_property;
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
OnvifMediaProfiles__init (OnvifMediaProfiles * self)
{
    OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (ONVIF_MEDIA_PROFILES(self));
    priv->count = 0;
    priv->profiles = NULL;
}

OnvifMediaProfiles* OnvifMediaProfiles__new(struct _trt__GetProfilesResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA_PROFILES, "soap", resp, NULL);
}


int OnvifMediaProfiles__get_size(OnvifMediaProfiles * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_MEDIA_PROFILES (self), 0);
    
    OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (ONVIF_MEDIA_PROFILES(self));
    return priv->count;
}

void OnvifMediaProfiles__insert(OnvifMediaProfiles * self,OnvifProfile * profile){
    OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (ONVIF_MEDIA_PROFILES(self));
    priv->profiles = realloc (priv->profiles,sizeof (void *) * (priv->count+1));
    priv->profiles[priv->count]=profile;
    priv->count++;
}

OnvifProfile * OnvifMediaProfiles__get_profile(OnvifMediaProfiles * self,int index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_PROFILES (self), NULL);
    
    OnvifMediaProfilesPrivate *priv = OnvifMediaProfiles__get_instance_private (ONVIF_MEDIA_PROFILES(self));
    g_return_val_if_fail (index < priv->count, NULL);

    return priv->profiles[index];
}


void OnvifProfile__init(OnvifProfile * self,struct tt__Profile * profile, int index){
    if(profile->Name){
        self->name = malloc(strlen(profile->Name)+1);
        strcpy(self->name,profile->Name);
    } else {
        self->name = NULL;
    }

    if(profile->token){
        self->token = malloc(strlen(profile->token)+1);
        strcpy(self->token,profile->token);
    } else {
        self->token = NULL;
    }
    self->index = index;
}

OnvifProfile * OnvifProfile__create(struct tt__Profile * profile, int index){
    OnvifProfile * self = malloc(sizeof(OnvifProfile));
    OnvifProfile__init(self,profile,index);
    self->copy = 0;
    return self;
}

OnvifProfile * OnvifProfile__copy(OnvifProfile * self){
    if(!self){
        return NULL;
    }
    OnvifProfile * nprofile = malloc(sizeof(OnvifProfile));
    nprofile->name = malloc(strlen(self->name)+1);
    strcpy(nprofile->name,self->name);
    nprofile->token = malloc(strlen(self->token)+1);
    strcpy(nprofile->token,self->token);
    nprofile->index = self->index;
    nprofile->copy = 1;
    return nprofile;
}

void OnvifProfile__real_destroy(OnvifProfile* self){
    if(!self)return;
    if(self->name)
        free(self->name);
    if(self->token)
        free(self->token);
    free(self);
}

void OnvifProfile__destroy(OnvifProfile* self){
    g_return_if_fail (self != NULL);
    g_return_if_fail (self->copy == 1);
    OnvifProfile__real_destroy(self);
}

char * OnvifProfile__get_name(OnvifProfile* self){
    if(!self)return NULL;
    return self->name;
}

char * OnvifProfile__get_token(OnvifProfile* self){
    if(!self)return NULL;
    return self->token;
}

int OnvifProfile__get_index(OnvifProfile* self){
    if(!self){C_WARN("Failed to get profile index. NULL pointer. Defaulting to 0."); return 0;}
    return self->index;
}


int OnvifProfile__string_equals(char * str1, char * str2){
    if(str1 == NULL && str2 == NULL){
        return 1;
    }
    if( str1 == NULL || str2 == NULL){
        return 0;
    }
    return strcmp(str1,str2) == 0;
}

int OnvifProfile__equals(OnvifProfile * prof1, OnvifProfile * prof2){
    if(prof1 == prof2){
        return 1;
    }
    if( prof1 == NULL || prof2 == NULL){
        return 0;
    }

    if(!OnvifProfile__string_equals(prof1->name, prof2->name)){
        return 0;
    }
    if(!OnvifProfile__string_equals(prof1->token, prof2->token)){
        return 0;
    }
    return prof1->index == prof2->index;
}