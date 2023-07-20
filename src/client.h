#ifndef SOAP_CLIENT_H_ 
#define SOAP_CLIENT_H_

#include <stdlib.h>




// Header
typedef struct {
    pthread_mutex_t  * lock;
    char * endpoint;
    struct soap *soap;
} OnvifSoapClient;  // forward declared for encapsulation


OnvifSoapClient* OnvifSoapClient__create();  // equivalent to "new Point(x, y)"
void OnvifSoapClient__destroy(OnvifSoapClient* OnvifSoapClient);  // equivalent to "delete point"
int OnvifSoapClient__is_valid(OnvifSoapClient * self);
void OnvifSoapClient__set_endpoint(OnvifSoapClient * self, char * endpoint);
void OnvifSoapClient__set_credentials(OnvifSoapClient * self, char * user, char * password);

#endif