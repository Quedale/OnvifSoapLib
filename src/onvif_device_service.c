#include "onvif_device_service.h"
#include "onvif_device_capabilities_local.h"
#include "onvif_device_info_local.h"
#include "onvif_device_interface_local.h"
#include "onvif_device_scopes_local.h"
#include "clogger.h"
#include "generated/soapH.h"
#include "wsse2api.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct _OnvifDeviceService {
    OnvifBaseService * parent;
} OnvifDeviceService;

OnvifDeviceService * OnvifDeviceService__create(const char * endpoint, OnvifCredentials * credentials, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    OnvifDeviceService * self = malloc(sizeof(OnvifDeviceService));
    OnvifDeviceService__init(self,endpoint, credentials, error_cb, error_data);
    return self;
}

void OnvifDeviceService__init(OnvifDeviceService * self,const char * endpoint, OnvifCredentials * credentials, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    self->parent = OnvifBaseService__create(endpoint, credentials, error_cb, error_data);
}

void OnvifDeviceService__destroy(OnvifDeviceService * self){
    if(self){
        OnvifBaseService__destroy(self->parent);
        free(self);
    }
}

OnvifBaseService * OnvifDeviceService__get_parent(OnvifDeviceService * self){
    return self->parent;
}

OnvifCapabilities* OnvifDeviceService__getCapabilities(OnvifDeviceService * self) {
    C_DEBUG("OnvifDeviceService__getCapabilities\n");
    struct _tds__GetCapabilities gethostname;
    struct _tds__GetCapabilitiesResponse response;
    OnvifCapabilities *capabilities = NULL;

    memset (&gethostname, 0, sizeof (gethostname));
    memset (&response, 0, sizeof (response));

    OnvifBaseService__lock(self->parent);
    char * endpoint = OnvifBaseService__get_endpoint(self->parent);
    SoapDef * soap  = OnvifBaseService__soap_new(self->parent);

    if(!soap){
        OnvifBaseService__set_error_code(self->parent,ONVIF_CONNECTION_ERROR);
        goto exit;
    } else {
        OnvifBaseService__set_error_code(self->parent,ONVIF_ERROR_NONE);
    }

    int soapret = soap_call___tds__GetCapabilities(soap, endpoint, NULL, &gethostname,  &response);
    if (soapret == SOAP_OK){
        capabilities = OnvifCapabilities__create(&response);
    } else {
        OnvifBaseService__handle_soap_error(self->parent,soap,soapret);
    }

exit:
    free(endpoint);
    OnvifBaseService__soap_destroy(soap);  
    OnvifBaseService__unlock(self->parent);
    return capabilities;
}


OnvifDeviceInformation * OnvifDeviceService__getDeviceInformation(OnvifDeviceService *self){
    struct _tds__GetDeviceInformation request;
    struct _tds__GetDeviceInformationResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    OnvifDeviceInformation *ret = NULL;

    OnvifBaseService__lock(self->parent);
    char * endpoint = OnvifBaseService__get_endpoint(self->parent);
    SoapDef * soap  = OnvifBaseService__soap_new(self->parent);
    if(!soap){
        OnvifBaseService__set_error_code(self->parent, ONVIF_CONNECTION_ERROR);
        goto exit;
    } else {
        OnvifBaseService__set_error_code(self->parent, ONVIF_ERROR_NONE);
    }

    int soapret = soap_call___tds__GetDeviceInformation(soap, endpoint, "", &request,  &response);
    if (soapret == SOAP_OK){
        ret = OnvifDeviceInformation__create(&response);
    } else {
        OnvifBaseService__handle_soap_error(self->parent,soap,soapret);
    }

exit:
    free(endpoint);
    OnvifBaseService__soap_destroy(soap);
    OnvifBaseService__unlock(self->parent);
    return ret;
}

OnvifInterfaces * OnvifDeviceService__getNetworkInterfaces(OnvifDeviceService * self) {
    struct _tds__GetNetworkInterfaces req;
    struct _tds__GetNetworkInterfacesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifInterfaces * interfaces = NULL;

    OnvifBaseService__lock(self->parent);
    char * endpoint = OnvifBaseService__get_endpoint(self->parent);

    SoapDef * soap  = OnvifBaseService__soap_new(self->parent);
    if(!soap){
        OnvifBaseService__set_error_code(self->parent, ONVIF_CONNECTION_ERROR);
        goto exit;
    } else {
        OnvifBaseService__set_error_code(self->parent, ONVIF_ERROR_NONE);
    }

    int soapret = soap_call___tds__GetNetworkInterfaces(soap, endpoint, "", &req,  &resp);
    if (soapret == SOAP_OK){
        interfaces = OnvifInterfaces__create(&resp);
    } else {
        OnvifBaseService__handle_soap_error(self->parent,soap,soapret);
    }

exit:
    free(endpoint);
    OnvifBaseService__soap_destroy(soap);
    OnvifBaseService__unlock(self->parent);
    return interfaces;
}

OnvifScopes * OnvifDeviceService__getScopes(OnvifDeviceService * self) {
    struct _tds__GetScopes req;
    struct _tds__GetScopesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifScopes * scopes = NULL;

    OnvifBaseService__lock(self->parent);
    char * endpoint = OnvifBaseService__get_endpoint(self->parent);
    SoapDef * soap  = OnvifBaseService__soap_new(self->parent);
    if(!soap){
        OnvifBaseService__set_error_code(self->parent, ONVIF_CONNECTION_ERROR);
        goto exit;
    } else {
        OnvifBaseService__set_error_code(self->parent, ONVIF_ERROR_NONE);
    }
    
    int soapret = soap_call___tds__GetScopes(soap, endpoint, "", &req,  &resp);
    if (soapret == SOAP_OK){
        scopes = OnvifScopes__create(&resp);
    } else {
        OnvifBaseService__handle_soap_error(self->parent,soap,soapret);
    }

exit:
    free(endpoint);
    OnvifBaseService__soap_destroy(soap);
    OnvifBaseService__unlock(self->parent);
    return scopes;
}

char * OnvifDeviceService__getHostname(OnvifDeviceService * self) {
    struct _tds__GetHostname gethostname;
    struct _tds__GetHostnameResponse response;
    memset (&gethostname, 0, sizeof (gethostname));
    memset (&response, 0, sizeof (response));

    char * ret = NULL;

    OnvifBaseService__lock(self->parent);
    char * endpoint = OnvifBaseService__get_endpoint(self->parent);
    SoapDef * soap  = OnvifBaseService__soap_new(self->parent);
    if(!soap){
        OnvifBaseService__set_error_code(self->parent, ONVIF_CONNECTION_ERROR);
        goto exit;
    } else {
        OnvifBaseService__set_error_code(self->parent, ONVIF_ERROR_NONE);
    }

    int soapret = soap_call___tds__GetHostname(soap, endpoint, "", &gethostname,  &response);
    if (soapret == SOAP_OK){
        ret = malloc(strlen(response.HostnameInformation->Name)+1); 
        strcpy(ret,response.HostnameInformation->Name);
    } else {
        OnvifBaseService__handle_soap_error(self->parent,soap,soapret);
    }

exit:
    free(endpoint);
    OnvifBaseService__soap_destroy(soap);
    OnvifBaseService__unlock(self->parent);
    return ret;
}

char * OnvifDeviceService__get_endpoint(OnvifDeviceService * self){
    return OnvifBaseService__get_endpoint(self->parent);
}