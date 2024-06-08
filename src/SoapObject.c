#include "SoapObject.h"

typedef struct {
  SoapFault * fault;
} SoapObjectPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (SoapObject, SoapObject_, G_TYPE_OBJECT)

GType
SoapFault__get_type (void)
{
        static gsize g_define_type_id__volatile = 0;

        if (g_once_init_enter(&g_define_type_id__volatile)) {
                static const GEnumValue values[] = {
                        { SOAP_FAULT_NONE,                  "No Fault",                         "None"},
                        { SOAP_FAULT_UNAUTHORIZED,          "Soap Fault Unauthorized",          "Unauthorized"},
                        { SOAP_FAULT_ACTION_NOT_SUPPORTED,  "Soap Fault Action Not Supported",  "ActionNotSupported"},
                        { 0,                        NULL,                       NULL}
                };
                GType g_define_type_id = g_enum_register_static(g_intern_static_string("SoapFault"), values);
                g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
        }

        return g_define_type_id__volatile;
}


static void
SoapObject__class_init (SoapObjectClass *klass)
{
}

static void
SoapObject__init (SoapObject *self)
{
    SoapObjectPrivate *priv = SoapObject__get_instance_private (self);
    priv->fault = SOAP_FAULT_NONE;

}

SoapObject * SoapObject__new (void){
    return g_object_new (SOAP_TYPE_OBJECT, NULL);
}

SoapFault * SoapObject__get_fault(SoapObject * self){
    g_return_val_if_fail (self != NULL,NULL);
    g_return_val_if_fail (SOAP_IS_OBJECT (self),NULL);
    SoapObjectPrivate *priv = SoapObject__get_instance_private (self);
    return priv->fault;
}