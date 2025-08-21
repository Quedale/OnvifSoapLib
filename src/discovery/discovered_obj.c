
#include "discovered_obj.h"
#include <stdlib.h>


void priv_DiscoveryEvent__destroy(CObject * obj){
    DiscoveryEvent * self = (DiscoveryEvent *) obj;
    if(self->server){
        ProbMatches__destroy(self->server->matches);
        free(self->server);
    }
}

DiscoveryEvent* DiscoveryEvent__create(DiscoveredServer * server, void * data){
    DiscoveryEvent * ret = malloc(sizeof(DiscoveryEvent));
    DiscoveryEvent__init(ret,server,data);
    CObject__set_allocated((CObject*)ret);
    return ret;
}

void DiscoveryEvent__init(DiscoveryEvent* self, DiscoveredServer * server, void * data){
    CObject__init((CObject*)self);
    CObject__set_destroy_callback((CObject*)self,priv_DiscoveryEvent__destroy);
    self->data = data;
    self->server = server;
}
