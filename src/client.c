#include "generated/soapH.h"
#include "client.h"
#include "wsse2api.h"

void OnvifSoapClient__init(OnvifSoapClient* self) {
    self->endpoint= NULL;
    self->lock = malloc(sizeof(pthread_mutex_t));

    pthread_mutex_init(self->lock, NULL);
}

OnvifSoapClient* OnvifSoapClient__create() {
  OnvifSoapClient* result = (OnvifSoapClient*) malloc(sizeof(OnvifSoapClient));
  OnvifSoapClient__init(result);
  return result;
}

int OnvifSoapClient__is_valid(OnvifSoapClient * self){
  if(!self || !self->endpoint || !self->lock){
    return 0;
  }
  return 1;
}

void OnvifSoapClient__destroy(OnvifSoapClient* self) {
  if (self) {
    free(self->endpoint);
    pthread_mutex_destroy(self->lock);
    free(self->lock);
    free(self);
  }
}

void OnvifSoapClient__set_endpoint(OnvifSoapClient * self, char * endpoint){
  self->endpoint = endpoint;
}

SoapWrapper * OnvifSoapClient__new_soap(OnvifSoapClient * self){
    SoapWrapper * wrapper = malloc(sizeof(SoapWrapper));
    wrapper->soap= soap_new1(SOAP_XML_CANONICAL | SOAP_C_UTFSTRING); //SOAP_XML_STRICT may cause crash
    wrapper->soap->connect_timeout = wrapper->soap->recv_timeout = wrapper->soap->send_timeout = 10; // 10 sec
    soap_register_plugin(wrapper->soap, soap_wsse);
    return wrapper;
}

void OnvifSoapClient__cleanup(SoapWrapper * wrap){
    soap_wsse_delete_Security(wrap->soap);
    soap_wsse_verify_done(wrap->soap);
    soap_destroy(wrap->soap);
    soap_end(wrap->soap);
    soap_done(wrap->soap);
    soap_free(wrap->soap);
    free(wrap);
}