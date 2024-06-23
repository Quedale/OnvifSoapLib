#include "onvif_device_service.h"
#include "onvif_device_capabilities_local.h"
#include "onvif_device_hostnameinfo_local.h"
#include "onvif_device_info_local.h"
#include "onvif_device_interface_local.h"
#include "onvif_device_scopes_local.h"
#include "onvif_base_service_local.h"
#include "onvif_datetime_local.h"
#include "clogger.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>

typedef struct _OnvifDeviceService {
    OnvifBaseService * parent;
} OnvifDeviceService;

OnvifDeviceService * OnvifDeviceService__create(OnvifDevice * device, const char * endpoint){
    OnvifDeviceService * self = malloc(sizeof(OnvifDeviceService));
    OnvifDeviceService__init(self, device, endpoint);
    return self;
}

void OnvifDeviceService__init(OnvifDeviceService * self, OnvifDevice * device, const char * endpoint){
    self->parent = OnvifBaseService__create(device, endpoint);
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

OnvifDateTime * OnvifDeviceService__getSystemDateAndTime(OnvifDeviceService * self){
    struct _tds__GetSystemDateAndTime request;
    struct _tds__GetSystemDateAndTimeResponse response;

    OnvifDateTime * ret_val;
    memset (&ret_val, 0, sizeof (ret_val));
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetSystemDateAndTime, OnvifDateTime__new, ret_val, soap, NULL, &request,  &response);

    return ret_val;
}

OnvifCapabilities* OnvifDeviceService__getCapabilities(OnvifDeviceService * self) {
    struct _tds__GetCapabilities request;
    struct _tds__GetCapabilitiesResponse response;
    OnvifCapabilities *capabilities = NULL;

    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    enum tt__CapabilityCategory v[] = {tt__CapabilityCategory__All};
    request.__sizeCategory = 1;
    request.Category = v;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetCapabilities, OnvifCapabilities__new, capabilities, soap, NULL, &request,  &response);

    return capabilities;
}


OnvifDeviceInformation * OnvifDeviceService__getDeviceInformation(OnvifDeviceService *self){
    struct _tds__GetDeviceInformation request;
    struct _tds__GetDeviceInformationResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    OnvifDeviceInformation *dev_info = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetDeviceInformation, OnvifDeviceInformation__new, dev_info, soap, NULL, &request,  &response);

    return dev_info;
}

OnvifDeviceInterfaces * OnvifDeviceService__getNetworkInterfaces(OnvifDeviceService * self) {
    struct _tds__GetNetworkInterfaces req;
    struct _tds__GetNetworkInterfacesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifDeviceInterfaces * interfaces = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetNetworkInterfaces, OnvifDeviceInterfaces__new, interfaces, soap, NULL, &req,  &resp);

    return interfaces;
}

OnvifScopes * OnvifDeviceService__getScopes(OnvifDeviceService * self) {
    struct _tds__GetScopes req;
    struct _tds__GetScopesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifScopes * scopes = NULL;
    
    ONVIF_INVOKE_SOAP_CALL(self, tds__GetScopes, OnvifScopes__new, scopes, soap, NULL, &req,  &resp);

    return scopes;
}

OnvifDeviceHostnameInfo * OnvifDeviceService__getHostname(OnvifDeviceService * self) {
    struct _tds__GetHostname gethostname;
    struct _tds__GetHostnameResponse response;
    memset (&gethostname, 0, sizeof (gethostname));
    memset (&response, 0, sizeof (response));

    OnvifDeviceHostnameInfo * hostname = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetHostname, OnvifDeviceHostnameInfo__new, hostname, soap, NULL, &gethostname,  &response);

    return hostname;
}

char * OnvifDeviceService__get_endpoint(OnvifDeviceService * self){
    return OnvifBaseService__get_endpoint(self->parent);
}