#include "generated/soapH.h"
#include "client.h"

void OnvifSoapClient__init(OnvifSoapClient* self) {
    self->endpoint= NULL;
    self->soap= soap_new();
    self->lock = malloc(sizeof(pthread_mutex_t));

    pthread_mutex_init(self->lock, NULL);
}

OnvifSoapClient* OnvifSoapClient__create() {
  OnvifSoapClient* result = (OnvifSoapClient*) malloc(sizeof(OnvifSoapClient));
  OnvifSoapClient__init(result);
  return result;
}

int OnvifSoapClient__is_valid(OnvifSoapClient * self){
  if(!self || !self->endpoint || !self->soap || !self->lock){
    return 0;
  }
  return 1;
}

void OnvifSoapClient__destroy(OnvifSoapClient* self) {
  if (self) {
    soap_destroy(self->soap); // delete managed objects
    soap_end(self->soap);     // delete managed data and temporaries 
    soap_done(self->soap);
    soap_free(self->soap); 
    free(self->endpoint);
    pthread_mutex_destroy(self->lock);
    free(self->lock);
    free(self);
  }
}

void OnvifSoapClient__set_endpoint(OnvifSoapClient * self, char * endpoint){
  self->endpoint = endpoint;
}
