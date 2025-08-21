#include "SoapObject_local.h"

enum
{
  PROP_DATA = 1,
  N_PROPERTIES
};

typedef struct {
  SoapFault fault;
} SoapObjectPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (SoapObject, SoapObject_, G_TYPE_OBJECT)
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

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
SoapObject__set_object(SoapObject * obj, gpointer ptr){
    if(SOAP_OBJECT_GET_CLASS(obj)->construct){
        SOAP_OBJECT_GET_CLASS(obj)->construct(obj,ptr);
    }
}

static void
SoapObject__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    SoapObject * self = SOAP_OBJECT (object);
    switch (prop_id){
        case PROP_DATA:
            SoapObject__set_object(self,g_value_get_pointer (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
SoapObject__get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    switch (prop_id){
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}


static void
SoapObject__class_init (SoapObjectClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = SoapObject__set_property;
    object_class->get_property = SoapObject__get_property;
    klass->construct = NULL;

    obj_properties[PROP_DATA] =
        g_param_spec_pointer ("data",
                            "Soap Response data",
                            "Pointer to original soap response.",
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);

    g_object_class_install_properties (object_class,
                                        N_PROPERTIES,
                                        obj_properties);
}

static void
SoapObject__init (SoapObject *self)
{
    SoapObjectPrivate *priv = SoapObject__get_instance_private (self);
    priv->fault = SOAP_FAULT_NONE;

}

void SoapObject__set_fault(SoapObject * self, SoapFault fault){
    g_return_if_fail (self != NULL);
    g_return_if_fail (SOAP_IS_OBJECT (self));
    SoapObjectPrivate *priv = SoapObject__get_instance_private (self);
    priv->fault = fault;
}

SoapFault * SoapObject__get_fault(SoapObject * self){
    g_return_val_if_fail (self != NULL,NULL);
    g_return_val_if_fail (SOAP_IS_OBJECT (self),NULL);
    SoapObjectPrivate *priv = SoapObject__get_instance_private (self);
    return &priv->fault;
}
