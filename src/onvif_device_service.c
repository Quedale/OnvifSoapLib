#include "onvif_device_service.h"
#include "onvif_device_capabilities_local.h"
#include "onvif_device_info_local.h"
#include "onvif_device_interface_local.h"
#include "onvif_device_scopes_local.h"
#include "onvif_base_service_local.h"
#include "clogger.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct _OnvifDeviceService {
    OnvifBaseService * parent;
} OnvifDeviceService;

OnvifDeviceService * OnvifDeviceService__create(OnvifDevice * device, const char * endpoint, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    OnvifDeviceService * self = malloc(sizeof(OnvifDeviceService));
    OnvifDeviceService__init(self, device, endpoint, error_cb, error_data);
    return self;
}

void OnvifDeviceService__init(OnvifDeviceService * self, OnvifDevice * device, const char * endpoint, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    self->parent = OnvifBaseService__create(device, endpoint, error_cb, error_data);
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

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetCapabilities, OnvifCapabilities__create, capabilities, soap, endpoint, NULL, &gethostname,  &response);

    return capabilities;
}


OnvifDeviceInformation * OnvifDeviceService__getDeviceInformation(OnvifDeviceService *self){
    struct _tds__GetDeviceInformation request;
    struct _tds__GetDeviceInformationResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    OnvifDeviceInformation *dev_info = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetDeviceInformation, OnvifDeviceInformation__create, dev_info, soap, endpoint, NULL, &request,  &response);

    return dev_info;
}

OnvifInterfaces * OnvifDeviceService__getNetworkInterfaces(OnvifDeviceService * self) {
    struct _tds__GetNetworkInterfaces req;
    struct _tds__GetNetworkInterfacesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifInterfaces * interfaces = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetNetworkInterfaces, OnvifInterfaces__create, interfaces, soap, endpoint, NULL, &req,  &resp);

    return interfaces;
}

OnvifScopes * OnvifDeviceService__getScopes(OnvifDeviceService * self) {
    struct _tds__GetScopes req;
    struct _tds__GetScopesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifScopes * scopes = NULL;
    
    ONVIF_INVOKE_SOAP_CALL(self, tds__GetScopes, OnvifScopes__create, scopes, soap, endpoint, NULL, &req,  &resp);

    return scopes;
}

char * OnvifDeviceService__getHostname_callback(struct _tds__GetHostnameResponse * response){
    char * ret = malloc(strlen(response->HostnameInformation->Name)+1); 
    strcpy(ret,response->HostnameInformation->Name);
    return ret;
}

char * OnvifDeviceService__getHostname(OnvifDeviceService * self) {
    struct _tds__GetHostname gethostname;
    struct _tds__GetHostnameResponse response;
    memset (&gethostname, 0, sizeof (gethostname));
    memset (&response, 0, sizeof (response));

    char * hostname = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetHostname, OnvifDeviceService__getHostname_callback, hostname, soap, endpoint, NULL, &gethostname,  &response);

    return hostname;
}

char * OnvifDeviceService__get_endpoint(OnvifDeviceService * self){
    return OnvifBaseService__get_endpoint(self->parent);
}