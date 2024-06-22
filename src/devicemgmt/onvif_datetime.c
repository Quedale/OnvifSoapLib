#include "onvif_datetime_local.h"
#include "clogger.h"

enum
{
  PROP_SOAP = 1,
  N_PROPERTIES
};

typedef struct {
    time_t datetime;
} OnvifDateTimePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifDateTime, OnvifDateTime_, SOAP_TYPE_OBJECT)
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
OnvifDateTime__set_soap(OnvifDateTime * self, struct _tds__GetSystemDateAndTimeResponse * response){
    OnvifDateTimePrivate *priv = OnvifDateTime__get_instance_private (ONVIF_DATETIME(self));

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
        return;
    }

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
        priv->datetime = time(NULL);
    } else {
        priv->datetime = timegm(&tmrdev); //Convert tm to time_t
    }
}

static void
OnvifDateTime__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    OnvifDateTime * self = ONVIF_DATETIME (object);
    // OnvifDateTimePrivate *priv = OnvifDateTime__get_instance_private (self);
    switch (prop_id){
        case PROP_SOAP:
            OnvifDateTime__set_soap(self,g_value_get_pointer (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifDateTime__get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    // OnvifDateTime *thread = ONVIF_DEVICE_CAPABILITIES (object);
    // OnvifDateTimePrivate *priv = OnvifDateTime__get_instance_private (thread);
    switch (prop_id){
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

// static void
// OnvifDateTime__dispose (GObject *self)
// {
//     OnvifDateTimePrivate *priv = OnvifDateTime__get_instance_private (ONVIF_DATETIME(self));

// }

static void
OnvifDateTime__class_init (OnvifDateTimeClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    // object_class->dispose = OnvifDateTime__dispose;
    object_class->set_property = OnvifDateTime__set_property;
    object_class->get_property = OnvifDateTime__get_property;
    obj_properties[PROP_SOAP] =
        g_param_spec_pointer ("soap",
                            "SoapMessage",
                            "Pointer to original soap message.",
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);

    g_object_class_install_properties (object_class,
                                        N_PROPERTIES,
                                        obj_properties);
}

static void
OnvifDateTime__init (OnvifDateTime * self)
{
    OnvifDateTimePrivate *priv = OnvifDateTime__get_instance_private (ONVIF_DATETIME(self));
    priv->datetime = 0;
}

OnvifDateTime* OnvifDateTime__new(struct _tds__GetSystemDateAndTimeResponse * resp){
    return g_object_new (ONVIF_TYPE_DATETIME, "soap", resp, NULL);
}

time_t * OnvifDateTime__get_datetime(OnvifDateTime * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DATETIME (self), NULL);

    OnvifDateTimePrivate *priv = OnvifDateTime__get_instance_private (ONVIF_DATETIME(self));
    return &priv->datetime;
}