#include "generated/soapH.h"
#include "client.h"

struct _SoapCred {
    char * user;
    char * pass;
};

void OnvifSoapClient__init(OnvifSoapClient* self, char * endpoint, char * user, char * pass) {
    self->endpoint=endpoint;
    self->soap= soap_new();
    struct _SoapCred * priv = malloc(sizeof(struct _SoapCred));
    priv->pass = pass;
    priv->user = user;
    self->private = priv;
}

OnvifSoapClient* OnvifSoapClient__create(char * endpoint, char * user, char * password) {
  printf("OnvifSoapClient__create [%s]\n",endpoint);
  OnvifSoapClient* result = (OnvifSoapClient*) malloc(sizeof(OnvifSoapClient));
  OnvifSoapClient__init(result, endpoint, user, password);
  return result;
}

void OnvifSoapClient__destroy(OnvifSoapClient* self) {
  if (self) {
    printf("OnvifSoapClient__destroy [%s]\n",self->endpoint);
    soap_destroy(self->soap); // delete managed objects
    soap_end(self->soap);     // delete managed data and temporaries 
    soap_done(self->soap);
    soap_free(self->soap); 
    free(self->private);
    free(self);
  }
}