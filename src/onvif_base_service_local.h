#ifndef ONVIF_BASE_SERVICE_LOCAL_H_ 
#define ONVIF_BASE_SERVICE_LOCAL_H_

#include "onvif_base_service.h"
#include "url_parser.h"
#include "clogger.h"
#include "httpda.h"
#include "plugin/logging.h"

#define FAULT_UNAUTHORIZED "\"http://www.onvif.org/ver10/error\":NotAuthorized"
#define FAULT_ACTIONNOTSUPPORTED "\"http://www.onvif.org/ver10/error\":ActionNotSupported"

OnvifBaseService * OnvifBaseService__new(OnvifDevice * device, const char * endpoint);
void OnvifBaseService__set_endpoint(OnvifBaseService * self, char * endpoint);
int OnvifBaseService__http_challenge(OnvifBaseService * self, SoapDef * soap, char* url);
void OnvifBaseService__lock(OnvifBaseService * self);
void OnvifBaseService__unlock(OnvifBaseService * self);
SoapDef * OnvifBaseService__soap_new(OnvifBaseService * self);
void OnvifBaseService__soap_destroy(OnvifBaseService * self, struct soap *soap);
int OnvifBaseService__ssl_verify_callback(int ok, X509_STORE_CTX *store);

#define STR_CONCAT(a,b) a##b
#define TOSTRMAC(A) #A

#define xsd__boolean_to_bool(a, b, c) \
    if(!a){ \
        C_WARN(TOSTRMAC(STR_CONCAT(c," is NULL"))); \
        b = FALSE; \
    } else { \
        switch(*a){ \
            case xsd__boolean__false_: \
                b = FALSE; \
                break; \
            case xsd__boolean__true_: \
                b = TRUE; \
                break; \
            default: \
                b = FALSE; \
                C_WARN(TOSTRMAC(STR_CONCAT(c," default to FALSE"))); \
        } \
    }

#define BUILD_SOAP_FUNC(a)  soap_call___##a 
#define BUILD_SOAP_TYPE(a)  _##a 
#define ONVIF_INVOKE_SOAP_CALL_MSTART(self, callback, vocreator, vo) \
    OnvifBaseService__lock(ONVIF_BASE_SERVICE(self)); \
    char * privendpt = OnvifBaseService__get_endpoint(ONVIF_BASE_SERVICE(self)); \
    char str_func[] = #callback; \
    char * svd_ptr; \
    char * tokenx = strtok_r(str_func, "_", &svd_ptr); \
    tokenx = strtok_r(svd_ptr, "_", &svd_ptr); \
    C_DEBUG("[%s] Invoking %s", privendpt, tokenx); \
    struct soap * soap = OnvifBaseService__soap_new(ONVIF_BASE_SERVICE(self)); \
    if(!soap){ \
        vo = (*(vocreator))(NULL); \
        SoapObject__set_fault(SOAP_OBJECT(vo),SOAP_FAULT_CONNECTION_ERROR); \
        goto prv_exit; \
    }

#define ONVIF_INVOKE_SOAP_CALL_MEND(self, callback, vocreator, vo, req, resp) \
    int ret; \
    int redirect_count = 0; \
retry: \
    ret = (*BUILD_SOAP_FUNC(callback))(soap, privendpt, NULL, req, resp); \
    /* Untested, need a camera setup with HTTPS on top of soap */ \
    if(redirect_count < 10 && soap->error >= 301 && soap->error <= 308){ \
        redirect_count++; \
        /*302 = contruct new uri */ \
        if(soap->error == 302){ \
            ParsedURL * purl = ParsedURL__create(privendpt); \
            ParsedURL * npurl = ParsedURL__create(soap->endpoint); \
            ParsedURL__set_host(purl,ParsedURL__get_host(npurl)); \
            ParsedURL__set_port(purl,ParsedURL__get_port(npurl)); \
            ParsedURL__set_protocol(purl,ParsedURL__get_protocol(npurl)); \
            free(privendpt); \
            privendpt = ParsedURL__toString(purl); \
            ParsedURL__destroy(purl); \
            ParsedURL__destroy(npurl); \
        } else { \
            free(privendpt); \
            privendpt = malloc(strlen(soap->endpoint)+1); \
            strcpy(privendpt,soap->endpoint); \
        } \
        C_WARN("Redirecting to : %s [%d]",privendpt, soap->error); \
        goto retry; \
    } \
    char * new_endpoint = NULL; \
    if(soap->error == 401 && OnvifBaseService__http_challenge(ONVIF_BASE_SERVICE(self),soap,privendpt)) \
        ret = (*BUILD_SOAP_FUNC(callback))(soap, privendpt, NULL, req, resp); \
    if (ret == SOAP_UDP_ERROR || ret == SOAP_TCP_ERROR){ /*Port fallback*/ \
        char * master_port = OnvifDevice__get_port(OnvifBaseService__get_device(ONVIF_BASE_SERVICE(self))); \
        new_endpoint = URL__set_port(privendpt, master_port); \
        if(strcmp(new_endpoint,privendpt) != 0){ /* TODO compare port before constructing new URL */ \
            C_WARN("[%s] Attempting to correct URL port to [%s]", privendpt, new_endpoint); \
            ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, NULL, req, resp); \
            if(soap->error == 401 && OnvifBaseService__http_challenge(ONVIF_BASE_SERVICE(self),soap,new_endpoint)) \
                ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, NULL, req, resp); \
        } \
        free(master_port); \
    } \
    if (ret == SOAP_UDP_ERROR || ret == SOAP_TCP_ERROR){ /* host fallback*/\
        if(new_endpoint) free(new_endpoint); \
        char * master_host = OnvifDevice__get_host(OnvifBaseService__get_device(ONVIF_BASE_SERVICE(self))); \
        new_endpoint = URL__set_host(privendpt, master_host); \
        if(strcmp(new_endpoint,privendpt) != 0){ /* TODO compare port before constructing new URL */ \
            C_WARN("[%s] Attempting to correct URL host : to [%s]", privendpt, new_endpoint); \
            ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, NULL, req, resp); \
            if(soap->error == 401 && OnvifBaseService__http_challenge(ONVIF_BASE_SERVICE(self),soap,new_endpoint)) \
                ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, NULL, req, resp); \
        } \
        free(master_host); \
    } \
    if (ret == SOAP_UDP_ERROR || ret == SOAP_TCP_ERROR){ /* host and port fallback*/\
        if(new_endpoint) free(new_endpoint); \
        char * master_host = OnvifDevice__get_host(OnvifBaseService__get_device(ONVIF_BASE_SERVICE(self))); \
        char * master_port = OnvifDevice__get_port(OnvifBaseService__get_device(ONVIF_BASE_SERVICE(self))); \
        char * tmp_endpoint = URL__set_host(privendpt, master_host); \
        new_endpoint = URL__set_port(tmp_endpoint, master_port); \
        if(strcmp(new_endpoint,privendpt) != 0){ /* TODO compare port before constructing new URL */ \
            C_WARN("[%s] Attempting to correct root URL to [%s]", privendpt, new_endpoint); \
            ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, NULL, req, resp); \
            if(soap->error == 401 && OnvifBaseService__http_challenge(ONVIF_BASE_SERVICE(self),soap,new_endpoint)) \
                ret = (*BUILD_SOAP_FUNC(callback))(soap, new_endpoint, NULL, req, resp); \
        } \
        free(tmp_endpoint); \
        free(master_host); \
        free(master_port); \
    } \
    if ((ret == SOAP_OK || ret == 401) && new_endpoint) OnvifBaseService__set_endpoint(ONVIF_BASE_SERVICE(self),new_endpoint); /* on successful tcp connection, assuming the new url is valid*/ \
    if (ret == SOAP_OK){ \
        vo = (*(vocreator))(resp); \
    } else { \
        char * endpt = OnvifBaseService__get_endpoint(ONVIF_BASE_SERVICE(self)); \
        vo = (*(vocreator))(NULL); \
        if (soap->error == SOAP_UDP_ERROR || \
            soap->error == SOAP_TCP_ERROR || \
            soap->error == SOAP_HTTP_ERROR || \
            soap->error == SOAP_SSL_ERROR || \
            soap->error == SOAP_ZLIB_ERROR){ \
            C_ERROR("[%s] CONNECTION ERROR\n",endpt); \
            SoapObject__set_fault(SOAP_OBJECT(vo),SOAP_FAULT_CONNECTION_ERROR); \
        } else if(soap->error == SOAP_NO_TAG){ \
            C_ERROR("[%s] Server didn't return a soap message.",endpt); \
            SoapObject__set_fault(SOAP_OBJECT(vo),SOAP_FAULT_NOT_VALID); \
        } else if(soap->error == 400 || \
                    soap->error == 403 || /* Forbidden (soap not authorized returns a 200) */ \
                    soap->error == 404){ /* Not found */ \
            C_ERROR("[%s] Server returned 400 bad request",endpt); \
            SoapObject__set_fault(SOAP_OBJECT(vo),SOAP_FAULT_NOT_VALID); \
        } else { \
            const char * fault_code = soap_fault_subcode(soap); \
            if (soap->error == 401 || (fault_code && !strcmp(fault_code,FAULT_UNAUTHORIZED))) { \
                C_WARN("[%s] Not Authorized Error\n",endpt); \
                SoapObject__set_fault(SOAP_OBJECT(vo),SOAP_FAULT_UNAUTHORIZED); \
            } else if (fault_code && !strcmp(fault_code,FAULT_ACTIONNOTSUPPORTED)) { \
                C_WARN("[%s] Action Not Supported Soap Error\n",endpt); \
                SoapObject__set_fault(SOAP_OBJECT(vo),SOAP_FAULT_ACTION_NOT_SUPPORTED); \
            } else { /* Mostly SOAP_FAULT */ \
                C_ERROR("[%s] Unhandled ERROR %i [%s]\n",endpt,soap->error, fault_code); \
                soap_print_fault(soap, stderr); \
                SoapObject__set_fault(SOAP_OBJECT(vo),SOAP_FAULT_UNEXPECTED); \
            } \
        } \
        free(endpt); \
    } \
    if (new_endpoint) free(new_endpoint); \
prv_exit: \
    free(privendpt); \
    OnvifBaseService__soap_destroy(ONVIF_BASE_SERVICE(self), soap); \
    OnvifBaseService__unlock(ONVIF_BASE_SERVICE(self));

 #define ONVIF_INVOKE_SOAP_CALL(self, callback, vocreator, vo, req, resp) \
    ONVIF_INVOKE_SOAP_CALL_MSTART(self,callback, vocreator, vo) \
    ONVIF_INVOKE_SOAP_CALL_MEND(self, callback, vocreator, vo, req, resp)

#define ONVIF_PREPARE_SOAP_CALL(ret_type, req_type, resp_type) \
    struct req_type request; \
    struct resp_type response; \
    memset (&request, 0, sizeof (request)); \
    memset (&response, 0, sizeof (response)); \
    ret_type * ret_val = NULL;

#endif