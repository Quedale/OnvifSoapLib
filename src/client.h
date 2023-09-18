#ifndef SOAP_CLIENT_H_ 
#define SOAP_CLIENT_H_

#include <stdlib.h>
#include <pthread.h>

// Header
typedef struct {
    pthread_mutex_t  * lock;
    char * endpoint;
} OnvifSoapClient;  // forward declared for encapsulation

typedef struct {
    struct soap *soap;
} SoapWrapper;

OnvifSoapClient* OnvifSoapClient__create();  // equivalent to "new Point(x, y)"
void OnvifSoapClient__destroy(OnvifSoapClient* OnvifSoapClient);  // equivalent to "delete point"
int OnvifSoapClient__is_valid(OnvifSoapClient * self);
void OnvifSoapClient__set_endpoint(OnvifSoapClient * self, char * endpoint);
void OnvifSoapClient__cleanup(SoapWrapper *soap);
SoapWrapper * OnvifSoapClient__new_soap(OnvifSoapClient * self);

#endif