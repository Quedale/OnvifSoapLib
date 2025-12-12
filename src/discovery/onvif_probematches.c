#include "onvif_probematches.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "clogger.h"
#include "cstring_utils.h"
#include <ctype.h>

typedef struct {
    int match_count;
    ProbMatch **matches;
} OnvifProbeMatchesPrivate ;

G_DEFINE_TYPE_WITH_PRIVATE (OnvifProbeMatches, OnvifProbeMatches_, G_TYPE_OBJECT)

static void
OnvifProbeMatches__dispose (GObject *object){
    OnvifProbeMatchesPrivate *priv = OnvifProbeMatches__get_instance_private (ONVIF_PROBEMATCHES(object));
    int i;
    for(i=0;i<priv->match_count;i++){
        ProbMatch *match = priv->matches[i];
        ProbMatch__destroy(match);
    }
    if(priv->matches){
        free(priv->matches);
        priv->matches = NULL;
        priv->match_count = 0;
    }
}

static void
OnvifProbeMatches__init(OnvifProbeMatches* self) {
    OnvifProbeMatchesPrivate *priv = OnvifProbeMatches__get_instance_private (self);
    priv->match_count = 0;
    priv->matches = NULL;
}

static void
OnvifProbeMatches__class_init (OnvifProbeMatchesClass *klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifProbeMatches__dispose;
}

OnvifProbeMatches *
OnvifProbeMatches__new() {
    return g_object_new (ONVIF_TYPE_PROBEMATCHES, NULL);
}

void OnvifProbeMatches__insert_match(OnvifProbeMatches * self, ProbMatch* match){
    g_return_if_fail (self != NULL);
    g_return_if_fail (ONVIF_IS_PROBEMATCHES (self));
    OnvifProbeMatchesPrivate *priv = OnvifProbeMatches__get_instance_private (self);
    priv->match_count++;
    if(priv->matches){
        priv->matches = realloc (priv->matches, sizeof (ProbMatch*) * priv->match_count);
    } else {
        priv->matches = malloc(sizeof (ProbMatch*) * priv->match_count);
    }
    priv->matches[priv->match_count-1]=match;
}

int 
OnvifProbeMatches__get_count(OnvifProbeMatches* self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_PROBEMATCHES (self), 0);
    OnvifProbeMatchesPrivate *priv = OnvifProbeMatches__get_instance_private (self);
    return priv->match_count;
}

ProbMatch ** 
OnvifProbeMatches__get_matches(OnvifProbeMatches* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_PROBEMATCHES (self), NULL);
    OnvifProbeMatchesPrivate *priv = OnvifProbeMatches__get_instance_private (self);
    return priv->matches;
}

void ProbMatch__insert_scope(ProbMatch* self, char * scope){
    self->scope_count++;
    if(self->scopes){
        self->scopes = realloc (self->scopes,sizeof (char *) * self->scope_count);
    } else {
        self->scopes = malloc(sizeof (char *) * self->scope_count);
    }
    self->scopes[self->scope_count-1]= malloc(strlen(scope)+1);
    strcpy(self->scopes[self->scope_count-1],scope);
}

void ProbMatch__destroy(ProbMatch* self) {
    C_TRACE("ProbMatch__destroy...");
    if (self) {
        int i;
        for(i=0;i<self->scope_count;i++){
            free(self->scopes[i]);
        }
        free(self->prob_uuid);
        free(self->addr_uuid);
        for(i=0;i<self->addrs_count;i++){
            free(self->addrs[i]);
        }
        if(self->addrs)
            free(self->addrs);
        if(self->types)
            free(self->types);
        if(self->scopes)
            free(self->scopes);
        if(self->service)
            free(self->service);
        free(self);
    }
}

void ProbMatch__init(ProbMatch* self) {
    self->prob_uuid = NULL;
    self->addr_uuid = NULL;
    self->addrs = NULL;
    self->types = NULL;
    self->scope_count = 0;
    self->addrs_count = 0;
    self->scopes = NULL;
    self->service = NULL;
    self->version = -1;
}

ProbMatch* ProbMatch__create() {
    C_TRACE("ProbMatch__create...");
    ProbMatch* result = (ProbMatch*) malloc(sizeof(ProbMatch));
    ProbMatch__init(result);
    return result;
}

void ProbMatch__set_prob_uuid(ProbMatch* self, char * prob_uuid){
    if(!prob_uuid){
        if(self->prob_uuid){
            free(self->prob_uuid);
            self->prob_uuid = NULL;
        }
        return;
    }

    if(!self->prob_uuid){
        self->prob_uuid = malloc(strlen(prob_uuid) + 1);
    } else {
        self->prob_uuid = realloc(self->prob_uuid,strlen(prob_uuid) + 1);
    }
    strcpy(self->prob_uuid,prob_uuid);
}

void ProbMatch__set_addr_uuid(ProbMatch* self, char * addr_uuid){
    if(!self->addr_uuid){
        self->addr_uuid = malloc(strlen(addr_uuid) + 1);
    } else {
        self->addr_uuid = realloc(self->addr_uuid,strlen(addr_uuid) + 1);
    }
    strcpy(self->addr_uuid,addr_uuid);
}

void ProbMatch__add_addr(ProbMatch* self, char * addr){
    self->addrs_count++;
    if(self->addrs){
        self->addrs = realloc (self->addrs, sizeof (char *) * self->addrs_count);
    } else {
        self->addrs = malloc(sizeof (char *) * self->addrs_count);
    }
    self->addrs[self->addrs_count-1] = (char*) malloc(strlen(addr)+1);
    strcpy(self->addrs[self->addrs_count-1],addr);
}

void ProbMatch__set_types(ProbMatch* self, char * types){
    if(!self->types){
        self->types = malloc(strlen(types) + 1);
    } else {
        self->types = realloc(self->types,strlen(types) + 1);
    }
    strcpy(self->types,types);
}

void ProbMatch__set_version(ProbMatch* self, int version){
    self->version = version;
}

static void urldecode2(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a')
                a -= 'a'-'A';
            if (a >= 'A')
                a -= ('A' - 10);
            else
                a -= '0';
            if (b >= 'a')
                b -= 'a'-'A';
            if (b >= 'A')
                b -= ('A' - 10);
            else
                b -= '0';
            *dst++ = 16*a+b;
            src+=3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst++ = '\0';
}

char * ProbMatch__extract_scope(char * key, ProbMatch * match){
    char* ret_val = malloc(1);
    ret_val[0] = '\0';
    const char delimeter[2] = "/";
    const char * onvif_key_del = "onvif://www.onvif.org/";

    char key_w_del[strlen(key)+1+strlen(delimeter)];
    strcpy(key_w_del, key);
    strcat(key_w_del, delimeter);

    int a;
    for (a = 0 ; a < match->scope_count ; ++a) {
        if(cstring_startsWith(onvif_key_del, match->scopes[a])){
            //Drop onvif scope prefix
            char subs[strlen(match->scopes[a])-strlen(onvif_key_del) + 1];
            strncpy(subs,match->scopes[a]+(strlen(onvif_key_del)),strlen(match->scopes[a]) - strlen(onvif_key_del)+1);

            if(cstring_startsWith(key_w_del,subs)){ // Found Scope
                //Extract value
                char sval[strlen(subs)-strlen(key_w_del) + 1];
                strncpy(sval,subs+(strlen(key_w_del)),strlen(subs) - strlen(key_w_del)+1);

                //Decode http string (e.g. %20 = whitespace)
                char output[strlen(sval)+1];
                urldecode2(output, sval);

                if(strlen(ret_val)==0){
                    ret_val = realloc(ret_val, strlen(output) +1);
                    strcpy(ret_val,output);
                } else {
                    ret_val = realloc(ret_val, strlen(ret_val) + strlen(" ") + strlen(output) +1);
                    strcat(ret_val, " ");
                    strcat(ret_val, output);
                }
            }
        }
    }

    return ret_val;
}
