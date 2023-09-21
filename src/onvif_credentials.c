#include "onvif_credentials.h"
#include <stdlib.h>
#include "threads.h"
#include <string.h>

typedef struct _OnvifCredentials {
    char * user;
    char * pass;
    MUTEX_TYPE prop_lock;
} OnvifCredentials;

OnvifCredentials * OnvifCredentials__create(){
    OnvifCredentials * self = malloc(sizeof(OnvifCredentials));
    OnvifCredentials__init(self);
    return self;
}

void OnvifCredentials__init(OnvifCredentials * self){
    self->prop_lock = MUTEX_INITIALIZER;
    MUTEX_SETUP(self->prop_lock);

    self->user = NULL;
    self->pass = NULL;
}

void OnvifCredentials__destroy(OnvifCredentials * self){
    if(self){
        free(self->user);
        free(self->pass);
        MUTEX_CLEANUP(self->prop_lock);
        free(self->prop_lock);
        free(self);
    }
}

void OnvifCredentials__set_username(OnvifCredentials *self, const char * username){
    MUTEX_LOCK(self->prop_lock);
    if(self->user){
        free(self->user);
    }
    if(username){
        self->user = malloc(strlen(username)+1);
        strcpy(self->user,username);
    }
    MUTEX_UNLOCK(self->prop_lock);
}

char * OnvifCredentials__get_username(OnvifCredentials *self){
    char * ret = NULL;
    MUTEX_LOCK(self->prop_lock);
    if(self->user){
        ret = malloc(strlen(self->user)+1);
        strcpy(ret,self->user);
    }
    MUTEX_UNLOCK(self->prop_lock);
    return ret;
}

void OnvifCredentials__set_password(OnvifCredentials *self, const char * password){
    MUTEX_LOCK(self->prop_lock);
    if(self->pass){
        free(self->pass);
    }
    if(password){
        self->pass = malloc(strlen(password)+1);
        strcpy(self->pass,password);
    }
    MUTEX_UNLOCK(self->prop_lock);
}

char * OnvifCredentials__get_password(OnvifCredentials *self){
    char * ret = NULL;
    MUTEX_LOCK(self->prop_lock);
    if(self->pass){
        ret = malloc(strlen(self->pass)+1);
        strcpy(ret,self->pass);
    }
    MUTEX_UNLOCK(self->prop_lock);
    return ret;
}