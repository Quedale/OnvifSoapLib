#ifndef ONVIF_BASE_SERVICE_LOCAL_H_ 
#define ONVIF_BASE_SERVICE_LOCAL_H_

#include "url_parser.h"
#include "clogger.h"

void OnvifBaseService__set_endpoint(OnvifBaseService * self, char * endpoint);
int OnvifBaseService__http_challenge(OnvifBaseService * self, SoapDef * soap, char* url);
void OnvifBaseService__lock(OnvifBaseService * self);
void OnvifBaseService__unlock(OnvifBaseService * self);
SoapDef * OnvifBaseService__soap_new(OnvifBaseService * self);
void OnvifBaseService__soap_destroy(OnvifBaseService * self, struct soap *soap);
void OnvifBaseService__set_error_code(OnvifBaseService * self, OnvifErrorTypes code);
void OnvifBaseService__handle_soap_error(OnvifBaseService * self, struct soap * soap, int error_code);

#define BUILD_SOAP_FUNC(a)  soap_call___##a 
#define BUILD_SOAP_TYPE(a)  _##a 
#define ONVIF_INVOKE_SOAP_CALL_MSTART(self, callback, soap) \
    OnvifBaseService__lock(self->parent); \
    char * privendpt = OnvifBaseService__get_endpoint(self->parent); \
    char str_func[] = #callback; \
    char * svd_ptr; \
    char * tokenx = strtok_r(str_func, "_", &svd_ptr); \
    tokenx = strtok_r(svd_ptr, "_", &svd_ptr); \
    C_DEBUG("[%s] Invoking %s", privendpt, tokenx); \
    struct soap * soap = OnvifBaseService__soap_new(self->parent); \
    if(!soap){ \
        OnvifBaseService__set_error_code(self->parent,ONVIF_ERROR_CONNECTION); \
        goto prv_exit; \
    } else { \
        OnvifBaseService__set_error_code(self->parent,ONVIF_ERROR_NONE); \
    }

#define ONVIF_INVOKE_SOAP_CALL_MEND(self, callback, vocreator, vo, soap, action, req, resp) \
    int ret = (*BUILD_SOAP_FUNC(callback))(soap, privendpt, action, req, resp); \
    char * new_endpoint = NULL; \
    if(soap->error == 401 && OnvifBaseService__http_challenge(self->parent,soap,privendpt)) \
        ret = (*BUILD_SOAP_FUNC(callback))(soap, privendpt, action, req, resp); \
    if (ret == SOAP_UDP_ERROR || ret == SOAP_TCP_ERROR){ /*Port fallback*/ \
        char * master_port = OnvifDevice__get_port(OnvifBaseService__get_device(self->parent)); \
        new_endpoint = URL__set_port(privendpt, master_port); \
        if(strcmp(new_endpoint,privendpt) != 0){ /* TODO compare port before constructing new URL */ \
            C_WARN("[%s] Attempting to correct URL port to [%s]", privendpt, new_endpoint); \
            ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, action, req, resp); \
            if(soap->error == 401 && OnvifBaseService__http_challenge(self->parent,soap,new_endpoint)) \
                ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, action, req, resp); \
        } \
        free(master_port); \
    } \
    if (ret == SOAP_UDP_ERROR || ret == SOAP_TCP_ERROR){ /* host fallback*/\
        if(new_endpoint) free(new_endpoint); \
        char * master_host = OnvifDevice__get_host(OnvifBaseService__get_device(self->parent)); \
        new_endpoint = URL__set_host(privendpt, master_host); \
        if(strcmp(new_endpoint,privendpt) != 0){ /* TODO compare port before constructing new URL */ \
            C_WARN("[%s] Attempting to correct URL host : to [%s]", privendpt, new_endpoint); \
            ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, action, req, resp); \
            if(soap->error == 401 && OnvifBaseService__http_challenge(self->parent,soap,new_endpoint)) \
                ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, action, req, resp); \
        } \
        free(master_host); \
    } \
    if (ret == SOAP_UDP_ERROR || ret == SOAP_TCP_ERROR){ /* host and port fallback*/\
        if(new_endpoint) free(new_endpoint); \
        char * master_host = OnvifDevice__get_host(OnvifBaseService__get_device(self->parent)); \
        char * master_port = OnvifDevice__get_port(OnvifBaseService__get_device(self->parent)); \
        char * tmp_endpoint = URL__set_host(privendpt, master_host); \
        new_endpoint = URL__set_port(tmp_endpoint, master_port); \
        if(strcmp(new_endpoint,privendpt) != 0){ /* TODO compare port before constructing new URL */ \
            C_WARN("[%s] Attempting to correct root URL to [%s]", privendpt, new_endpoint); \
            ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, action, req, resp); \
            if(soap->error == 401 && OnvifBaseService__http_challenge(self->parent,soap,new_endpoint)) \
                ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, action, req, resp); \
        } \
        free(tmp_endpoint); \
        free(master_host); \
        free(master_port); \
    } \
    if ((ret == SOAP_OK || ret == 401) && new_endpoint) OnvifBaseService__set_endpoint(self->parent,new_endpoint); /* on successful tcp connection, assuming the new url is valid*/ \
    if (ret == SOAP_OK){ \
        vo = (*(vocreator))(resp); \
    } else { \
        OnvifBaseService__handle_soap_error(self->parent,soap,ret); \
    } \
    if (new_endpoint) free(new_endpoint); \
prv_exit: \
    free(privendpt); \
    OnvifBaseService__soap_destroy(self->parent, soap); \
    OnvifBaseService__unlock(self->parent);


 #define ONVIF_INVOKE_SOAP_CALL(self, callback, vocreator, vo, soap, action, req, resp) \
    ONVIF_INVOKE_SOAP_CALL_MSTART(self,callback,soap) \
    ONVIF_INVOKE_SOAP_CALL_MEND(self, callback, vocreator, vo, soap, action, req, resp)

PUSH_WARNING_IGNORE(-1,-Wcomment)
//Alternative implementation allowing request struct init from a callback
// #define ONVIF_INIT_REQ(output, init_callback, type) void (*ptr)(struct type * req, SoapDef * soap) = init_callback; ((ptr) != (NULL) ? (output) : (NULL))
// #define ONVIF_INVOKE_SOAP_CALL(self, callback, reqcreator, vocreator, vo, soap, endpoint, action, req, resp) \
//     ONVIF_INVOKE_SOAP_CALL_MSTART(self,soap) \
//     ONVIF_INIT_REQ(ptr(req,soap),reqcreator, BUILD_SOAP_TYPE(callback)); \
//     ONVIF_INVOKE_SOAP_CALL_MEND(self, callback, vocreator, vo, soap, endpoint, action, req, resp)
POP_WARNING_IGNORE(NULL)
#endif