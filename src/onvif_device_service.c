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
#include <time.h>

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

time_t OnvifDeviceService__getSystemDateAndTime_callback(struct _tds__GetSystemDateAndTimeResponse * response){
    // Parse out the device time from aSystemDateAndTime.
    // Either UTCDateTime or LocalDateTime or both should be present.
    // From ONVIF 2.0 UTCDateTime is mandatory, but before that it was not.
    // Lets handle both cases so that we can get the LocalTime from old devices
    // or we may fail to authenticate and not be able to e.g. upgrade firmware.
    int is_utc = 0;

    struct tt__DateTime * aDateTime;
    // struct tt__TimeZone * tz = aSystemDateAndTime.TimeZone;
    struct tm tmrdev;
    memset (&tmrdev, 0, sizeof (tmrdev));

    if (response->SystemDateAndTime->UTCDateTime) {
        is_utc = 1;
        aDateTime = response->SystemDateAndTime->UTCDateTime;
    } else {
        //TODO Figure out how to convert remote local time (posix tz parsing)
        aDateTime = response->SystemDateAndTime->LocalDateTime;
    }
    tmrdev.tm_mday = aDateTime->Date->Day;
    tmrdev.tm_mon = aDateTime->Date->Month - 1;
    tmrdev.tm_year = aDateTime->Date->Year - 1900;
    tmrdev.tm_sec = aDateTime->Time->Second;
    tmrdev.tm_min = aDateTime->Time->Minute;
    tmrdev.tm_hour = aDateTime->Time->Hour;
    
    //TODO Handle localtime instead of UTC (prior to ONVIF 2.0)
    if (!is_utc) {
        C_ERROR("camera with Local time isn't supported yet. (ONVIF < 2.0)");
        return time(NULL);
    }
    return timegm(&tmrdev); //Convert tm to time_t
}

time_t OnvifDeviceService__getSystemDateAndTime(OnvifDeviceService * self){
    struct _tds__GetSystemDateAndTime request;
    struct _tds__GetSystemDateAndTimeResponse response;

    time_t ret_val;
    memset (&ret_val, 0, sizeof (ret_val));
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetSystemDateAndTime, OnvifDeviceService__getSystemDateAndTime_callback, ret_val, soap, NULL, &request,  &response);

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

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetCapabilities, OnvifCapabilities__create, capabilities, soap, NULL, &request,  &response);

    return capabilities;
}


OnvifDeviceInformation * OnvifDeviceService__getDeviceInformation(OnvifDeviceService *self){
    struct _tds__GetDeviceInformation request;
    struct _tds__GetDeviceInformationResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    OnvifDeviceInformation *dev_info = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetDeviceInformation, OnvifDeviceInformation__create, dev_info, soap, NULL, &request,  &response);

    return dev_info;
}

OnvifInterfaces * OnvifDeviceService__getNetworkInterfaces(OnvifDeviceService * self) {
    struct _tds__GetNetworkInterfaces req;
    struct _tds__GetNetworkInterfacesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifInterfaces * interfaces = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetNetworkInterfaces, OnvifInterfaces__create, interfaces, soap, NULL, &req,  &resp);

    return interfaces;
}

OnvifScopes * OnvifDeviceService__getScopes(OnvifDeviceService * self) {
    struct _tds__GetScopes req;
    struct _tds__GetScopesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifScopes * scopes = NULL;
    
    ONVIF_INVOKE_SOAP_CALL(self, tds__GetScopes, OnvifScopes__create, scopes, soap, NULL, &req,  &resp);

    return scopes;
}

char * OnvifDeviceService__getHostname_callback(struct _tds__GetHostnameResponse * response){
    char * ret = NULL;
    if(response && response->HostnameInformation){
        ret = malloc(strlen(response->HostnameInformation->Name)+1); 
        strcpy(ret,response->HostnameInformation->Name);
    }
    return ret;
}

char * OnvifDeviceService__getHostname(OnvifDeviceService * self) {
    struct _tds__GetHostname gethostname;
    struct _tds__GetHostnameResponse response;
    memset (&gethostname, 0, sizeof (gethostname));
    memset (&response, 0, sizeof (response));

    char * hostname = NULL;

    ONVIF_INVOKE_SOAP_CALL(self, tds__GetHostname, OnvifDeviceService__getHostname_callback, hostname, soap, NULL, &gethostname,  &response);

    return hostname;
}

char * OnvifDeviceService__get_endpoint(OnvifDeviceService * self){
    return OnvifBaseService__get_endpoint(self->parent);
}