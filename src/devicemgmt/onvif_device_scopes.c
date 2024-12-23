#include "onvif_device_scopes_local.h"
#include "clogger.h"
#include "cstring_utils.h"
#include "../SoapObject_local.h"

typedef struct _OnvifScope {
    char * scope;
    int editable;
} OnvifScope;

typedef struct {
    int count;
    OnvifScope ** scopes;
} OnvifScopesPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifScopes, OnvifScopes_, SOAP_TYPE_OBJECT)

static void
OnvifScopes__reset (OnvifScopes *self);

static void
OnvifScopes__construct(SoapObject * obj, gpointer ptr){
    OnvifScopes * self = ONVIF_DEVICE_SCOPES(obj);
    struct _tds__GetScopesResponse * resp = ptr;
    OnvifScopesPrivate *priv = OnvifScopes__get_instance_private (self);

    OnvifScopes__reset(self);

    if(!resp){
        return;
    }

    int i;
    for(i=0;i<resp->__sizeScopes;i++){
        struct tt__Scope scope = resp->Scopes[i];
        // tt__ScopeDefinition scopedef = scope.ScopeDef;
        xsd__anyURI scopeitem = scope.ScopeItem;
        if(!scopeitem){
            SoapObject__set_fault(obj,SOAP_FAULT_UNEXPECTED);
            OnvifScopes__reset(self);
            return;
        }
        OnvifScope * onvifscope = malloc(sizeof(OnvifScope));
        onvifscope->scope = (char*) malloc(strlen(scopeitem)+1);
        strcpy(onvifscope->scope,scopeitem);

        priv->count++;
        if(priv->scopes){
            priv->scopes = realloc (priv->scopes, sizeof (OnvifScope*) * priv->count);
        } else {
            priv->scopes = malloc(sizeof (OnvifScope*) * priv->count);
        }
        priv->scopes[priv->count-1] = onvifscope;

        // tt__ScopeDefinition__Fixed : tt__ScopeDefinition__Fixed || tt__ScopeDefinition__Configurable
    }
}

static void
OnvifScopes__reset (OnvifScopes *self){
    OnvifScopesPrivate *priv = OnvifScopes__get_instance_private (self);
    if(priv->scopes){
        int i;
        for(i=0;i<priv->count;i++){
            free(priv->scopes[i]->scope);
            free(priv->scopes[i]);
        }
        priv->count = 0;
        if(priv->scopes){
            free(priv->scopes);
            priv->scopes = NULL;
        }
    }
}

static void
OnvifScopes__dispose (GObject *self)
{
    OnvifScopes__reset(ONVIF_DEVICE_SCOPES(self));
    G_OBJECT_CLASS (OnvifScopes__parent_class)->dispose (self);
}

static void
OnvifScopes__class_init (OnvifScopesClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifScopes__dispose;
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifScopes__construct;
}

static void
OnvifScopes__init (OnvifScopes * self)
{
    OnvifScopesPrivate *priv = OnvifScopes__get_instance_private (ONVIF_DEVICE_SCOPES(self));
    priv->scopes = NULL;
    priv->count = 0;
}

OnvifScopes* OnvifScopes__new(struct _tds__GetScopesResponse * resp){
    return g_object_new (ONVIF_TYPE_DEVICE_SCOPES, "data", resp, NULL);
}

/// @brief 
/// @param dst 
/// @param src 
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

char * OnvifScopes__extract_scope(OnvifScopes * self, char * key){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SCOPES (self), 0);
    OnvifScopesPrivate *priv = OnvifScopes__get_instance_private (ONVIF_DEVICE_SCOPES(self));

    char* ret_val = NULL;
    const char delimeter[2] = "/";
    const char * onvif_key_del = "onvif://www.onvif.org/";

    char * key_w_del = malloc(strlen(key)+1+strlen(delimeter));
    strcpy(key_w_del, key);
    strcat(key_w_del, delimeter);

    int a;
    for (a = 0 ; a < priv->count ; ++a) {
        if(cstring_startsWith(onvif_key_del, priv->scopes[a]->scope)){
            //Drop onvif scope prefix
            char * subs = malloc(strlen(priv->scopes[a]->scope)-strlen(onvif_key_del) + 1);
            strncpy(subs,priv->scopes[a]->scope+(strlen(onvif_key_del)),strlen(priv->scopes[a]->scope) - strlen(onvif_key_del)+1);

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