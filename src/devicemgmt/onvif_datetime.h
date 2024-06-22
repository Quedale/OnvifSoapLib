#ifndef ONVIF_DATETIME_H_ 
#define ONVIF_DATETIME_H_

#include "shard_export.h"
#include "SoapObject.h"

G_BEGIN_DECLS

#define ONVIF_TYPE_DATETIME OnvifDateTime__get_type()
G_DECLARE_FINAL_TYPE (OnvifDateTime, OnvifDateTime_, ONVIF, DATETIME, SoapObject)

struct _OnvifDateTime
{
    SoapObject parent_instance;
};


struct _OnvifDateTimeClass
{
    SoapObjectClass parent_class;
};

SHARD_EXPORT time_t * OnvifDateTime__get_datetime(OnvifDateTime * self);

G_END_DECLS

#endif