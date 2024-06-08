#ifndef SOAP_OBJECT_H 
#define SOAP_OBJECT_H

#include <glib-object.h>

/*
 * Potentially, include other headers on which this header depends.
 */

G_BEGIN_DECLS

/*
 * Type declaration.
 */
#define SOAP_TYPE_OBJECT SoapObject__get_type()
G_DECLARE_FINAL_TYPE (SoapObject, SoapObject_, SOAP, OBJECT, GObject)


typedef enum {
    SOAP_FAULT_NONE         = 0,
    SOAP_FAULT_UNAUTHORIZED         = 1,
    SOAP_FAULT_ACTION_NOT_SUPPORTED = 1
} SoapFault;

#ifndef g_enum_to_nick
#define g_enum_to_nick(type,val) (g_enum_get_value(g_type_class_ref (type),val)->value_nick)
#endif

GType SoapFault__get_type (void) G_GNUC_CONST;
#define SOAP_TYPE_FAULT (SoapFault__get_type())


struct _SoapObject
{
  GObject parent_instance;

  /* Other members, including private data. */
};


struct _SoapObjectClass
{
  GObjectClass parent_class;

};

/*
 * Method definitions.
 */
SoapObject * SoapObject__new (void);
SoapFault * SoapObject__get_fault(SoapObject * self);

G_END_DECLS

#endif