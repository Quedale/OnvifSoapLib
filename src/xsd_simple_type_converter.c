#include "xsd_simple_type_converter.h"

gboolean
xsd__boolean_to_bool(enum xsd__boolean * val, gboolean default_val){
    if(!val){ 
        return default_val;
    } else { 
        switch(*val){ 
            case xsd__boolean__false_: 
                return FALSE; 
            case xsd__boolean__true_: 
                return TRUE;
            default: 
                return default_val;
        } 
    }
}
