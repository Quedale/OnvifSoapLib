#ifndef onvif_discovery_h__
#define onvif_discovery_h__

#include "probmatch.h" 
#include "discovered_obj.h"
#include "shard_export.h"

SHARD_EXPORT void sendProbe(void * data, int timeout, int (*cc)(DiscoveryEvent *));

SHARD_EXPORT char * onvif_extract_scope(char * key, ProbMatch * match);

#endif
