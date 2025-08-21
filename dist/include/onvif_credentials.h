#ifndef ONVIF_CRED_H_ 
#define ONVIF_CRED_H_

#include "shard_export.h"

typedef struct _OnvifCredentials OnvifCredentials;

SHARD_EXPORT OnvifCredentials * OnvifCredentials__create(void);
SHARD_EXPORT void OnvifCredentials__init(OnvifCredentials * self);
SHARD_EXPORT void OnvifCredentials__destroy(OnvifCredentials * self);
SHARD_EXPORT void OnvifCredentials__set_username(OnvifCredentials *self, const char * username);
SHARD_EXPORT char * OnvifCredentials__get_username(OnvifCredentials *self);
SHARD_EXPORT void OnvifCredentials__set_password(OnvifCredentials *self, const char * password);
SHARD_EXPORT char * OnvifCredentials__get_password(OnvifCredentials *self);

#endif
