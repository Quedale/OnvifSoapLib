#include "discovery/onvif_discovery.h"
#include "discovery/discoverer.h"
#include <stddef.h>
#include "clogger.h"

void probe_callback(DiscoveryEvent * e){
    C_DEBUG("prob callback...\n");
    DiscoveredServer * server = e->server;

    C_DEBUG("MSG UUID : %s\n", server->msg_uuid);
    C_DEBUG("PROBE UUID : %s\n", server->relate_uuid);

    int i;
    for(i=0;i<server->matches->match_count;i++){
        ProbMatch * match = server->matches->matches[i];
        for(int a=0;a<match->addrs_count;a++){
            C_DEBUG("\tAddress : %s\n",match->addrs[a]);
        }
        C_DEBUG("\tAddress UUID : %s\n",match->addr_uuid);
        C_DEBUG("\tAddress PROBE UUID : %s\n",match->prob_uuid);
        C_DEBUG("\tservice : %s\n",match->service);
        C_DEBUG("\tTypes : %s\n",match->types);
        C_DEBUG("\tVersion : %i\n",match->version);
        int a;
        for(a=0;a<match->scope_count;a++){
            char * scope = match->scopes[a];
            C_DEBUG("\t\tScope : %s\n",scope);
        }
    }
}

int main(int argc, char** argv)
{


        C_DEBUG("sending probex...\n");

        struct UdpDiscoverer udsc = UdpDiscoverer__create(probe_callback);
        UdpDiscoverer__start(&udsc, NULL, 1, 2);

        C_DEBUG("sending probe done...\n");
        return 0;

}