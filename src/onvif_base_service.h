#ifndef ONVIF_BASE_SERVICE_H_ 
#define ONVIF_BASE_SERVICE_H_

typedef struct _OnvifBaseService OnvifBaseService;
typedef struct soap SoapDef;

typedef enum {
    ONVIF_ERROR_NOT_SET = -1,
    ONVIF_ERROR_NONE = 0,
    ONVIF_ERROR_CONNECTION = 1,
    ONVIF_ERROR_SOAP = 2,
    ONVIF_ERROR_NOT_AUTHORIZED = 3,
    ONVIF_ERROR_NOT_VALID = 4
} OnvifErrorTypes;

#include "portable_thread.h"
#include "shard_export.h"
#include "onvif_credentials.h"
#include "onvif_device.h"

OnvifBaseService * OnvifBaseService__create(OnvifDevice * device, const char * endpoint, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data);
void OnvifBaseService__init(OnvifBaseService * self, OnvifDevice * device, const char * endpoint, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data);
void OnvifBaseService__destroy(OnvifBaseService * self);
SHARD_EXPORT char * OnvifBaseService__get_endpoint(OnvifBaseService * self);
SHARD_EXPORT OnvifDevice * OnvifBaseService__get_device(OnvifBaseService * self);


#endif