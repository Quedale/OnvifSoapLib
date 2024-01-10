#ifndef ONVIF_BASE_SERVICE_LOCAL_H_ 
#define ONVIF_BASE_SERVICE_LOCAL_H_

#include "url_parser.h"
#include "clogger.h"

void OnvifBaseService__set_endpoint(OnvifBaseService * self, char * endpoint);

#define BUILD_SOAP_FUNC(a)  soap_call___##a 
#define BUILD_SOAP_TYPE(a)  _##a 
#define ONVIF_INVOKE_SOAP_CALL_MSTART(self, soap) \
    OnvifBaseService__lock(self->parent); \
    char * endpoint = OnvifBaseService__get_endpoint(self->parent); \
    struct soap * soap = OnvifBaseService__soap_new(self->parent); \
    if(!soap){ \
        OnvifBaseService__set_error_code(self->parent,ONVIF_CONNECTION_ERROR); \
        goto exit; \
    } else { \
        OnvifBaseService__set_error_code(self->parent,ONVIF_ERROR_NONE); \
    }

#define ONVIF_INVOKE_SOAP_CALL_MEND(self, callback, vocreator, vo, soap, endpoint, action, req, resp) \
    int ret = (*BUILD_SOAP_FUNC(callback))(soap, endpoint, action, req, resp); \
    char * new_endpoint = NULL; \
    if (ret == SOAP_UDP_ERROR || ret == SOAP_TCP_ERROR){ \
        char * master_port = OnvifDevice__get_port(OnvifBaseService__get_device(self->parent)); \
        new_endpoint = URL__set_port(endpoint, master_port); \
        if(strcmp(new_endpoint,endpoint) != 0){ /* TODO compare port before constructing new URL */ \
            C_WARN("Connection error. Attempting to correct URL : '%s' --> '%s'", endpoint, new_endpoint); \
            ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, action, req, resp); \
        } \
        free(master_port); \
    } \
    if (ret == SOAP_OK){ \
        if (new_endpoint) OnvifBaseService__set_endpoint(self->parent,new_endpoint); \
        vo = (*(vocreator))(resp); \
    } else { \
        OnvifBaseService__handle_soap_error(self->parent,soap,ret); \
    } \
    if (new_endpoint) free(new_endpoint); \
exit: \
    free(endpoint); \
    OnvifBaseService__soap_destroy(soap); \
    OnvifBaseService__unlock(self->parent);


 #define ONVIF_INVOKE_SOAP_CALL(self, callback, vocreator, vo, soap, endpoint, action, req, resp) \
    ONVIF_INVOKE_SOAP_CALL_MSTART(self,soap) \
    ONVIF_INVOKE_SOAP_CALL_MEND(self, callback, vocreator, vo, soap, endpoint, action, req, resp)

PUSH_WARNING_IGNORE(-1,-Wcomment)
//Alternative implementation allowing request struct init from a callback
// #define ONVIF_INIT_REQ(output, init_callback, type) void (*ptr)(struct type * req, SoapDef * soap) = init_callback; ((ptr) != (NULL) ? (output) : (NULL))
// #define ONVIF_INVOKE_SOAP_CALL(self, callback, reqcreator, vocreator, vo, soap, endpoint, action, req, resp) \
//     ONVIF_INVOKE_SOAP_CALL_MSTART(self,soap) \
//     ONVIF_INIT_REQ(ptr(req,soap),reqcreator, BUILD_SOAP_TYPE(callback)); \
//     ONVIF_INVOKE_SOAP_CALL_MEND(self, callback, vocreator, vo, soap, endpoint, action, req, resp)
POP_WARNING_IGNORE(NULL)
#endif