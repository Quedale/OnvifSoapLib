#include "../onvif_base_service_local.h"
#include "onvif_device_capabilities_local.h"
#include "onvif_device_datetime_local.h"
#include "onvif_device_hostnameinfo_local.h"
#include "onvif_device_info_local.h"
#include "onvif_device_interface_local.h"
#include "onvif_device_scopes_local.h"
#include "onvif_device_services_local.h"
#include "onvif_device_service.h"
#include "clogger.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>

G_DEFINE_TYPE(OnvifDeviceService, OnvifDeviceService_, ONVIF_TYPE_BASE_SERVICE)

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
    return g_object_new (ONVIF_TYPE_DEVICE_SERVICE, "device", device, "uri", endpoint, NULL);
}

OnvifDeviceDateTime * OnvifDeviceService__getSystemDateAndTime(OnvifDeviceService * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SERVICE (self), NULL);

    ONVIF_PREPARE_SOAP_CALL(OnvifDeviceDateTime, _tds__GetSystemDateAndTime, _tds__GetSystemDateAndTimeResponse);
    ONVIF_INVOKE_SOAP_CALL(self, tds__GetSystemDateAndTime, OnvifDeviceDateTime__new, ret_val, &request,  &response);

    return ret_val;
}

OnvifCapabilities* OnvifDeviceService__getCapabilities(OnvifDeviceService * self) {
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SERVICE (self), NULL);

    ONVIF_PREPARE_SOAP_CALL(OnvifCapabilities, _tds__GetCapabilities, _tds__GetCapabilitiesResponse);

    enum tt__CapabilityCategory v[] = {tt__CapabilityCategory__All};
    request.__sizeCategory = 1;
    request.Category = v;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetCapabilities, OnvifCapabilities__new, ret_val, &request,  &response);

    return ret_val;
}

OnvifDeviceServices * OnvifDeviceService__getServices(OnvifDeviceService * self, int IncludeCapability) {
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SERVICE (self), NULL);

    ONVIF_PREPARE_SOAP_CALL(OnvifDeviceServices, _tds__GetServices, _tds__GetServicesResponse);

    request.IncludeCapability = IncludeCapability;
    
    ONVIF_INVOKE_SOAP_CALL(self, tds__GetServices, OnvifDeviceServices__new, ret_val, &request,  &response);

    return ret_val;
}

OnvifDeviceInformation * OnvifDeviceService__getDeviceInformation(OnvifDeviceService *self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SERVICE (self), NULL);

    ONVIF_PREPARE_SOAP_CALL(OnvifDeviceInformation, _tds__GetDeviceInformation, _tds__GetDeviceInformationResponse);
    ONVIF_INVOKE_SOAP_CALL(self, tds__GetDeviceInformation, OnvifDeviceInformation__new, ret_val, &request,  &response);

    return ret_val;
}

OnvifDeviceInterfaces * OnvifDeviceService__getNetworkInterfaces(OnvifDeviceService * self) {
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SERVICE (self), NULL);

    ONVIF_PREPARE_SOAP_CALL(OnvifDeviceInterfaces, _tds__GetNetworkInterfaces, _tds__GetNetworkInterfacesResponse);
    ONVIF_INVOKE_SOAP_CALL(self, tds__GetNetworkInterfaces, OnvifDeviceInterfaces__new, ret_val, &request,  &response);

    return ret_val;
}

OnvifScopes * OnvifDeviceService__getScopes(OnvifDeviceService * self) {
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SERVICE (self), NULL);

    ONVIF_PREPARE_SOAP_CALL(OnvifScopes, _tds__GetScopes, _tds__GetScopesResponse);
    ONVIF_INVOKE_SOAP_CALL(self, tds__GetScopes, OnvifScopes__new, ret_val, &request, &response);

    return ret_val;
}

OnvifDeviceHostnameInfo * OnvifDeviceService__getHostname(OnvifDeviceService * self) {
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SERVICE (self), NULL);

    ONVIF_PREPARE_SOAP_CALL(OnvifDeviceHostnameInfo, _tds__GetHostname, _tds__GetHostnameResponse);
    ONVIF_INVOKE_SOAP_CALL(self, tds__GetHostname, OnvifDeviceHostnameInfo__new, ret_val, &request,  &response);

    return ret_val;
}