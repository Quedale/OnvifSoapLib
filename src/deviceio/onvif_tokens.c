#include "onvif_tokens_local.h"
#include "clogger.h"

typedef struct {
    int count;
    char ** tokens;
} OnvifTokensPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifTokens, OnvifTokens_, SOAP_TYPE_OBJECT)

static void
OnvifTokens__reset (OnvifTokens *self);

static void
OnvifTokens__construct(SoapObject * obj, gpointer ptr){
    struct tmd__GetResponse * response = ptr;
    OnvifTokens * self = ONVIF_TOKENS(obj);
    OnvifTokensPrivate *priv = OnvifTokens__get_instance_private (self);

    OnvifTokens__reset(self);
    if(!response){

        return;
    }

    priv->count = response->__sizeToken;
    priv->tokens = malloc(sizeof(char*)*priv->count);
    for(int i=0;i<response->__sizeToken;i++){
        priv->tokens[i] = malloc(strlen(response->Token[i])+1);
        strcpy(priv->tokens[i],response->Token[i]);
    }
}

static void
OnvifTokens__reset (OnvifTokens *self){
    OnvifTokensPrivate *priv = OnvifTokens__get_instance_private (self);
    if(priv->tokens){
        for(int i=0;i<priv->count;i++){
            free(priv->tokens[i]);
        }
        free(priv->tokens);
        priv->tokens = NULL;
        priv->count = 0;
    }
}

static void
OnvifTokens__dispose (GObject *self)
{
    OnvifTokens__reset(ONVIF_TOKENS(self));

    G_OBJECT_CLASS (OnvifTokens__parent_class)->dispose (self);
}

static void
OnvifTokens__class_init (OnvifTokensClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifTokens__dispose;
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifTokens__construct;
}

static void
OnvifTokens__init (OnvifTokens * self)
{
    OnvifTokensPrivate *priv = OnvifTokens__get_instance_private (ONVIF_TOKENS(self));
    priv->tokens = NULL;
    priv->count = 0;
}

OnvifTokens* OnvifTokens__new(struct tmd__GetResponse * resp){
    return g_object_new (ONVIF_TYPE_TOKENS, "data", resp, NULL);
}

//TODO Implement getters