#ifndef ONVIF_SCOPES_H_ 
#define ONVIF_SCOPES_H_

#include "shard_export.h"

typedef struct _OnvifScope  OnvifScope;
typedef struct _OnvifScopes  OnvifScopes;

SHARD_EXPORT void OnvifScopes__destroy(OnvifScopes * scopes);
SHARD_EXPORT char * OnvifScopes__extract_scope(OnvifScopes * scopes, char * key);

#endif