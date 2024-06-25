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

G_DEFINE_TYPE(OnvifDeviceService, OnvifDeviceService_, ONVIF_TYPE_BASE_SERVICE)

#include "onvif_device_service.h"

static void
OnvifDeviceService__class_init (OnvifDeviceServiceClass * klass)
{
    //Nothing todo here
}

static void
OnvifDeviceService__init (OnvifDeviceService * self)
{
    //Nothing todo here
}

OnvifDeviceService* OnvifDeviceService__new(OnvifDevice * device, const char * endpoint){
    return g_object_new (ONVIF_TYPE_DEVICEMGMT_SERVICE, "device", device, "uri", endpoint, NULL);
}

OnvifDateTime * OnvifDeviceService__getSystemDateAndTime(OnvifDeviceService * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICEMGMT_SERVICE (self), NULL);

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
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICEMGMT_SERVICE (self), NULL);

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
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICEMGMT_SERVICE (self), NULL);

    struct _tds__GetDeviceInformation request;
    struct _tds__GetDeviceInformationResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    OnvifDeviceInformation *dev_info = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetDeviceInformation, OnvifDeviceInformation__new, dev_info, soap, NULL, &request,  &response);

    return dev_info;
}

OnvifDeviceInterfaces * OnvifDeviceService__getNetworkInterfaces(OnvifDeviceService * self) {
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICEMGMT_SERVICE (self), NULL);

    struct _tds__GetNetworkInterfaces req;
    struct _tds__GetNetworkInterfacesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifDeviceInterfaces * interfaces = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetNetworkInterfaces, OnvifDeviceInterfaces__new, interfaces, soap, NULL, &req,  &resp);

    return interfaces;
}

OnvifScopes * OnvifDeviceService__getScopes(OnvifDeviceService * self) {
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICEMGMT_SERVICE (self), NULL);

    struct _tds__GetScopes req;
    struct _tds__GetScopesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifScopes * scopes = NULL;
    
    ONVIF_INVOKE_SOAP_CALL(self, tds__GetScopes, OnvifScopes__new, scopes, soap, NULL, &req,  &resp);

    return scopes;
}

OnvifDeviceHostnameInfo * OnvifDeviceService__getHostname(OnvifDeviceService * self) {
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICEMGMT_SERVICE (self), NULL);

    struct _tds__GetHostname gethostname;
    struct _tds__GetHostnameResponse response;
    memset (&gethostname, 0, sizeof (gethostname));
    memset (&response, 0, sizeof (response));

    OnvifDeviceHostnameInfo * hostname = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetHostname, OnvifDeviceHostnameInfo__new, hostname, soap, NULL, &gethostname,  &response);

    return hostname;
}