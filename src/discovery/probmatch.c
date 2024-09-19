#include "probmatch.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "clogger.h"


void ProbMatches__init(ProbMatches* self) {
    self->match_count = 0;
    self->matches = NULL;
}

ProbMatches* ProbMatches__create() {
    C_TRACE("ProbMatches__create...");
    ProbMatches* result = (ProbMatches*) malloc(sizeof(ProbMatches));
    ProbMatches__init(result);
    return result;
}

void ProbMatches__destroy(ProbMatches* self) {
    C_TRACE("ProbMatches__destroy...");
    if (self) {
        int i;
        for(i=0;i<self->match_count;i++){
            ProbMatch *match = self->matches[i];
            ProbMatch__destroy(match);
        }
        if(self->matches){
            free(self->matches);
            self->matches = NULL;
        }
        free(self);
    }
}

void ProbMatches__insert_match(ProbMatches* self, ProbMatch* match){
    self->match_count++;
    if(self->matches){
        self->matches = realloc (self->matches, sizeof (ProbMatch*) * self->match_count);
    } else {
        self->matches = malloc(sizeof (ProbMatch*) * self->match_count);
    }
    self->matches[self->match_count-1]=match;
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
    C_TRACE("ProbMatch__destroy... done");
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