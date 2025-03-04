#ifndef XSD_SIMPLE_TYPE_CONVERTER_H 
#define XSD_SIMPLE_TYPE_CONVERTER_H

#include <glib-object.h>

#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

gboolean xsd__boolean_to_bool(enum xsd__boolean * val, gboolean default_val);

#endif