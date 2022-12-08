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
    
}

OnvifSoapClient* OnvifSoapClient__create(char * endpoint, char * user, char * password) {
   OnvifSoapClient* result = (OnvifSoapClient*) malloc(sizeof(OnvifSoapClient));
   OnvifSoapClient__init(result, endpoint, user, password);
   return result;
}

void OnvifSoapClient__reset(OnvifSoapClient* self) {
}

void OnvifSoapClient__destroy(OnvifSoapClient* OnvifSoapClient) {
  if (OnvifSoapClient) {
     OnvifSoapClient__reset(OnvifSoapClient);
     free(OnvifSoapClient->private);
     free(OnvifSoapClient);
  }
}