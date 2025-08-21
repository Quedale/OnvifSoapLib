#ifndef discovered_obj_h__
#define discovered_obj_h__

#include "probmatch.h"
#include "cobject.h"

typedef struct {
    char *msg_uuid; // SOAP-ENV:Envelope/SOAP-ENV:Header/wsa:MessageID
    char *relate_uuid; // SOAP-ENV:Envelope/SOAP-ENV:Header/wsa:RelatesTo
    ProbMatches *matches; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/
} DiscoveredServer;

typedef struct {
    CObject parent;
    DiscoveredServer * server;
    void * data;

} DiscoveryEvent;

DiscoveryEvent* DiscoveryEvent__create(DiscoveredServer * server, void * data); 
void DiscoveryEvent__init(DiscoveryEvent* self, DiscoveredServer * server, void * data); 

#endif
