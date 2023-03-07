#ifndef SOAP_CLIENT_H_ 
#define SOAP_CLIENT_H_

#include <stdlib.h>




// Header
typedef struct {
    char * endpoint;
    struct soap *soap;
    void * priv_ptr;
} OnvifSoapClient;  // forward declared for encapsulation


OnvifSoapClient* OnvifSoapClient__create(char * endpoint,char * user, char * password);  // equivalent to "new Point(x, y)"
void OnvifSoapClient__destroy(OnvifSoapClient* OnvifSoapClient);  // equivalent to "delete point"

#endif