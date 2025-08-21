#ifndef onvif_discovery_h__
#define onvif_discovery_h__

#include "probmatch.h" 
#include "discovered_obj.h"

__attribute__ ((visibility("default"))) 
extern void sendProbe(void * data, int timeout, int (*cc)(DiscoveryEvent *));

__attribute__ ((visibility("default"))) 
extern char * onvif_extract_scope(char * key, ProbMatch * match);

#endif
