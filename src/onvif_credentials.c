#include "onvif_credentials.h"
#include <stdlib.h>
#include "portable_thread.h"
#include <string.h>

typedef struct _OnvifCredentials {
    char * user;
    char * pass;
    P_MUTEX_TYPE prop_lock;
} OnvifCredentials;

OnvifCredentials * OnvifCredentials__create(){
    OnvifCredentials * self = malloc(sizeof(OnvifCredentials));
    OnvifCredentials__init(self);
    return self;
}

void OnvifCredentials__init(OnvifCredentials * self){
    P_MUTEX_SETUP(self->prop_lock);

    self->user = NULL;
    self->pass = NULL;
}

void OnvifCredentials__destroy(OnvifCredentials * self){
    if(self){
        free(self->user);
        free(self->pass);
        P_MUTEX_CLEANUP(self->prop_lock);
        free(self);
    }
}

void OnvifCredentials__set_username(OnvifCredentials *self, const char * username){
    P_MUTEX_LOCK(self->prop_lock);
    if(!username){
        free(self->user);
        self->user = NULL;
    } else {
        if(!self->user){
            self->user = malloc(strlen(username)+1);
        } else {
            self->user = realloc(self->user,strlen(username)+1);
        }
        strcpy(self->user,username);
    }
    P_MUTEX_UNLOCK(self->prop_lock);
}

char * OnvifCredentials__get_username(OnvifCredentials *self){
    char * ret = NULL;
    P_MUTEX_LOCK(self->prop_lock);
    if(self->user){
        ret = malloc(strlen(self->user)+1);
        strcpy(ret,self->user);
    }
    P_MUTEX_UNLOCK(self->prop_lock);
    return ret;
}

void OnvifCredentials__set_password(OnvifCredentials *self, const char * password){
    P_MUTEX_LOCK(self->prop_lock);
    if(!password){
        free(self->pass);
        self->pass = NULL;
    } else {
        if(!self->pass){
            self->pass = malloc(strlen(password)+1);
        } else {
            self->pass = realloc(self->pass,strlen(password)+1);
        }
        strcpy(self->pass,password);
    }
    P_MUTEX_UNLOCK(self->prop_lock);
}

char * OnvifCredentials__get_password(OnvifCredentials *self){
    char * ret = NULL;
    P_MUTEX_LOCK(self->prop_lock);
    if(self->pass){
        ret = malloc(strlen(self->pass)+1);
        strcpy(ret,self->pass);
    }
    P_MUTEX_UNLOCK(self->prop_lock);
    return ret;
}