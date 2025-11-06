#ifndef SOAP_OBJECT_H 
#define SOAP_OBJECT_H

#include <glib-object.h>
#include "shard_export.h"

/*
 * Potentially, include other headers on which this header depends.
 */

G_BEGIN_DECLS

/*
 * Type declaration.
 */
#define SOAP_TYPE_OBJECT SoapObject__get_type()
G_DECLARE_DERIVABLE_TYPE (SoapObject, SoapObject_, SOAP, OBJECT, GObject)


typedef enum {
    SOAP_FAULT_NONE         = 0,
    SOAP_FAULT_CONNECTION_ERROR = 1,
    SOAP_FAULT_NOT_VALID = 2,
    SOAP_FAULT_UNAUTHORIZED         = 3,
    SOAP_FAULT_ACTION_NOT_SUPPORTED = 4,
    SOAP_FAULT_UNEXPECTED = 5
} SoapFault;

#ifndef g_enum_to_nick
#define g_enum_to_nick(type,val) (g_enum_get_value(g_type_class_ref (type),val)->value_nick)
#endif

GType SoapFault__get_type (void) G_GNUC_CONST;
#define SOAP_TYPE_FAULT (SoapFault__get_type())


struct _SoapObjectClass {
  GObjectClass parent_class;
  void (* construct) (SoapObject  * self, gpointer ptr);

};

/*
 * Method definitions.
 */
SHARD_EXPORT SoapFault * SoapObject__get_fault(SoapObject * self);

G_END_DECLS

#endif
