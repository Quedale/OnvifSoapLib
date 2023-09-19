#ifndef ONVIF_BASE_SERVICE_H_ 
#define ONVIF_BASE_SERVICE_H_

#include "shard_export.h"
#include "onvif_credentials.h"

typedef struct _OnvifBaseService OnvifBaseService;
typedef struct soap SoapDef;

typedef enum {
    ONVIF_NOT_SET = -1,
    ONVIF_ERROR_NONE = 0,
    ONVIF_CONNECTION_ERROR = 1,
    ONVIF_SOAP_ERROR = 2,
    ONVIF_NOT_AUTHORIZED = 3
} OnvifErrorTypes;

OnvifBaseService * OnvifBaseService__create(const char * endpoint, OnvifCredentials * credentials, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data);
void OnvifBaseService__init(OnvifBaseService * self,const char * endpoint, OnvifCredentials * credentials, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data);
void OnvifBaseService__destroy(OnvifBaseService * self);
void OnvifBaseService__lock(OnvifBaseService * self);
void OnvifBaseService__unlock(OnvifBaseService * self);
SHARD_EXPORT char * OnvifBaseService__get_endpoint(OnvifBaseService * self);
OnvifCredentials * OnvifBaseService__get_credentials(OnvifBaseService * self);
SoapDef * OnvifBaseService__soap_new(OnvifBaseService * self);
void OnvifBaseService__soap_destroy(struct soap *soap);
void OnvifBaseService__set_error_code(OnvifBaseService * self, OnvifErrorTypes code);
void OnvifBaseService__handle_soap_error(OnvifBaseService * self, struct soap * soap, int error_code);

#endif