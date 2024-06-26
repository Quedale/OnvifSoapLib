#ifndef ONVIF_TOKENS_H_ 
#define ONVIF_TOKENS_H_

#include "shard_export.h"
#include "SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_TOKENS OnvifTokens__get_type()
G_DECLARE_FINAL_TYPE (OnvifTokens, OnvifTokens_, ONVIF, TOKENS, SoapObject)

struct _OnvifTokens
{
    SoapObject parent_instance;
};


struct _OnvifTokensClass
{
    SoapObjectClass parent_class;
};

//TODO Implement getters

G_END_DECLS

#endif