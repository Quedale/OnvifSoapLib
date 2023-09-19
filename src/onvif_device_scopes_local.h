#ifndef ONVIF_SCOPES_LOCAL_H_ 
#define ONVIF_SCOPES_LOCAL_H_

#include "onvif_device_scopes.h"
#include "generated/soapH.h"

OnvifScopes * OnvifScopes__create(struct _tds__GetScopesResponse * resp);
void OnvifScopes__init(OnvifScopes * self,struct _tds__GetScopesResponse * resp);

#endif