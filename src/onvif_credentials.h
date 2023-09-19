#ifndef ONVIF_CRED_H_ 
#define ONVIF_CRED_H_

typedef struct _OnvifCredentials OnvifCredentials;

OnvifCredentials * OnvifCredentials__create();
void OnvifCredentials__init(OnvifCredentials * self);
void OnvifCredentials__destroy(OnvifCredentials * self);
void OnvifCredentials__set_username(OnvifCredentials *self, const char * username);
char * OnvifCredentials__get_username(OnvifCredentials *self);
void OnvifCredentials__set_password(OnvifCredentials *self, const char * password);
char * OnvifCredentials__get_password(OnvifCredentials *self);

#endif