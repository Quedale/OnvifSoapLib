#include "onvif_media_profile_local.h"

typedef struct _OnvifProfile {
    char * name;
    char * token;
} OnvifProfile;

typedef struct _OnvifProfiles {
    OnvifProfile ** profiles;
    int count;
} OnvifProfiles;

void OnvifProfile__init(OnvifProfile * self,struct tt__Profile * profile){
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
}

OnvifProfile * OnvifProfile__create(struct tt__Profile * profile){
    OnvifProfile * self = malloc(sizeof(OnvifProfile));
    OnvifProfile__init(self,profile);
    return self;
}

void OnvifProfile__destroy(OnvifProfile* self){
    if(self->name)
        free(self->name);
    if(self->token)
        free(self->token);
    free(self);
}

char * OnvifProfile__get_name(OnvifProfile* self){
    if(!self){return NULL;}
    return self->name;
}

char * OnvifProfile__get_token(OnvifProfile* self){
    if(!self){return NULL;}
    return self->token;
}

void OnvifProfiles__init(OnvifProfiles * self,struct _trt__GetProfilesResponse * resp){
    self->count = 0;
    self->profiles = malloc(0);
    for(int i = 0; i < resp->__sizeProfiles; i++){
        OnvifProfile * profile = OnvifProfile__create(&(resp->Profiles[i]));
        OnvifProfiles__insert(self,profile);
    }
}

OnvifProfiles * OnvifProfiles__create(struct _trt__GetProfilesResponse * resp){
    OnvifProfiles * self = malloc(sizeof(OnvifProfiles));
    OnvifProfiles__init(self,resp);
    return self;
}

int OnvifProfiles__get_size(OnvifProfiles* self){
    return self->count;
}

void OnvifProfiles__destroy(OnvifProfiles* self){
    if(self){
        for (int i = 0; i < self->count; i++){
            OnvifProfile__destroy(self->profiles[i]);
        }
        free(self->profiles);
        self->count = 0;
        free(self);
    }
}

void OnvifProfiles__insert(OnvifProfiles * self,OnvifProfile * profile){
    self->profiles = realloc (self->profiles,sizeof (void *) * (self->count+1));
    self->profiles[self->count]=profile;
    self->count++;
}

OnvifProfile * OnvifProfiles__get_profile(OnvifProfiles * self,int index){
    if(index >= self->count){
        return NULL;
    }
    return self->profiles[index];
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
    return nprofile;
}

SHARD_EXPORT OnvifProfiles * OnvifProfiles__copy(OnvifProfiles * self){
    if(!self){
        return NULL;
    }

    OnvifProfiles * nprofiles = malloc(sizeof(OnvifProfiles));
    nprofiles->count = self->count;
    nprofiles->profiles = malloc(sizeof (void *) * (self->count+1));
    for(int i =0;i<self->count;i++){
        nprofiles->profiles[i] = OnvifProfile__copy(self->profiles[i]);
    }
    return nprofiles;
}