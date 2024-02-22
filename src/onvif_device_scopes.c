#include "onvif_device_scopes_local.h"
#include "clogger.h"
#include "cstring_utils.h"

typedef struct _OnvifScope {
    char * scope;
    int editable;
} OnvifScope;

typedef struct _OnvifScopes {
    int count;
    OnvifScope ** scopes;
} OnvifScopes;


void urldecode(char *dst, const char *src)
{
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

OnvifScopes * OnvifScopes__create(struct _tds__GetScopesResponse * resp){
    OnvifScopes * self = malloc(sizeof(OnvifScopes));
    OnvifScopes__init(self,resp);
    return self;
}

void OnvifScopes__init(OnvifScopes * self,struct _tds__GetScopesResponse * resp){
    self->scopes = malloc(0);
    self->count = 0;

    int i;
    for(i=0;i<resp->__sizeScopes;i++){
        struct tt__Scope scope = resp->Scopes[i];
        // tt__ScopeDefinition scopedef = scope.ScopeDef;
        xsd__anyURI scopeitem = scope.ScopeItem;

        OnvifScope * onvifscope = malloc(sizeof(OnvifScope));
        onvifscope->scope = (char*) malloc(strlen(scopeitem)+1);
        strcpy(onvifscope->scope,scopeitem);

        self->count++;
        self->scopes = realloc (self->scopes, sizeof (OnvifScope*) * self->count);
        self->scopes[self->count-1] = onvifscope;

        // tt__ScopeDefinition__Fixed : tt__ScopeDefinition__Fixed || tt__ScopeDefinition__Configurable
    }
}

char * OnvifScopes__extract_scope(OnvifScopes * self, char * key){
    C_DEBUG("OnvifScopes__extract_scope %s\n", key);
    if(!self){
        return NULL;
    }
    char* ret_val = NULL;
    const char delimeter[2] = "/";
    const char * onvif_key_del = "onvif://www.onvif.org/";

    char * key_w_del = malloc(strlen(key)+1+strlen(delimeter));
    strcpy(key_w_del, key);
    strcat(key_w_del, delimeter);

    int a;
    for (a = 0 ; a < self->count ; ++a) {
        if(cstring_startsWith(onvif_key_del, self->scopes[a]->scope)){
            //Drop onvif scope prefix
            char * subs = malloc(strlen(self->scopes[a]->scope)-strlen(onvif_key_del) + 1);
            strncpy(subs,self->scopes[a]->scope+(strlen(onvif_key_del)),strlen(self->scopes[a]->scope) - strlen(onvif_key_del)+1);

            if(cstring_startsWith(key_w_del,subs)){ // Found Scope
                //Extract value
                char * sval = malloc(strlen(subs)-strlen(key_w_del) + 1);
                strncpy(sval,subs+(strlen(key_w_del)),strlen(subs) - strlen(key_w_del)+1);

                //Decode http string (e.g. %20 = whitespace)
                char output[strlen(sval)+1];
                urldecode(output, sval);

                if(ret_val == NULL){
                    ret_val = malloc(strlen(output)+1);
                    memcpy(ret_val,output,strlen(output)+1);
                } else {
                    ret_val = realloc(ret_val, strlen(ret_val) + strlen(output) + strlen(" ") +1);
                    strcat(ret_val, " ");
                    strcat(ret_val, output);
                }
                free(sval);
            }
            free(subs);
        }
    }
    free(key_w_del);
  return ret_val;
}

void OnvifScopes__destroy(OnvifScopes * self){
    if(self){
        int i;
        for(i=0;i<self->count;i++){
            free(self->scopes[i]->scope);
            free(self->scopes[i]);
        }
        self->count = 0;
        free(self->scopes);
        free(self);
    }
}