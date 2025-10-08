#ifndef ONVIF_PREFIX_IP_ADDRESS_H_ 
#define ONVIF_PREFIX_IP_ADDRESS_H_

typedef struct _OnvifPrefixedIPAddress OnvifPrefixedIPAddress;

char * OnvifPrefixedIPAddress__get_address(OnvifPrefixedIPAddress * self);
int OnvifPrefixedIPAddress__get_prefix(OnvifPrefixedIPAddress * self);
void OnvifPrefixedIPAddress__destroy(OnvifPrefixedIPAddress * self);

#endif