#include "onvif_device_datetime_local.h"
#include "clogger.h"
#include "../SoapObject_local.h"

typedef struct {
    time_t datetime;
} OnvifDeviceDateTimePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifDeviceDateTime, OnvifDeviceDateTime_, SOAP_TYPE_OBJECT)

static void
OnvifDeviceDateTime__construct(SoapObject * obj, gpointer ptr){
    struct _tds__GetSystemDateAndTimeResponse * response = ptr;
    OnvifDeviceDateTimePrivate *priv = OnvifDeviceDateTime__get_instance_private (ONVIF_DEVICE_DATETIME(obj));

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

    if(!response){
        priv->datetime = -1;
        C_ERROR("Invalid GetSystemDateTime response.");
        SoapObject__set_fault(obj,SOAP_FAULT_UNEXPECTED);
        return;
    }

    if (response->SystemDateAndTime->UTCDateTime) {
        is_utc = 1;
        aDateTime = response->SystemDateAndTime->UTCDateTime;
    } else {
        //TODO Figure out how to convert remote local time (posix tz parsing)
        aDateTime = response->SystemDateAndTime->LocalDateTime;
    }

    if(!aDateTime){
        C_ERROR("Invalid GetSystemDateTime response.");
        SoapObject__set_fault(obj,SOAP_FAULT_UNEXPECTED);
        return;
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
        priv->datetime = time(NULL);
    } else {
        priv->datetime = timegm(&tmrdev); //Convert tm to time_t
    }

    if(priv->datetime < 0){
        C_ERROR("Invalid GetSystemDateTime time value.");
        SoapObject__set_fault(obj,SOAP_FAULT_UNEXPECTED);
    }
}


static void
OnvifDeviceDateTime__class_init (OnvifDeviceDateTimeClass * klass)
{
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifDeviceDateTime__construct;
}

static void
OnvifDeviceDateTime__init (OnvifDeviceDateTime * self)
{
    OnvifDeviceDateTimePrivate *priv = OnvifDeviceDateTime__get_instance_private (ONVIF_DEVICE_DATETIME(self));
    priv->datetime = 0;
}

OnvifDeviceDateTime* OnvifDeviceDateTime__new(struct _tds__GetSystemDateAndTimeResponse * resp){
    return g_object_new (ONVIF_TYPE_DEVICE_DATETIME, "data", resp, NULL);
}

time_t * OnvifDeviceDateTime__get_datetime(OnvifDeviceDateTime * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_DATETIME (self), NULL);

    OnvifDeviceDateTimePrivate *priv = OnvifDeviceDateTime__get_instance_private (ONVIF_DEVICE_DATETIME(self));
    return &priv->datetime;
}
