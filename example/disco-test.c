#include "discovery/onvif_discoverer.h"
// #include "discovery/discoverer.h"
#include <stddef.h>
#include "clogger.h"

void _dicovery_found_server_cb (OnvifDiscoverer * discoverer, OnvifProbeMatches * probematches) {
    C_DEBUG("prob callback...\n");
        int match_count = OnvifProbeMatches__get_count(probematches);
    int i;
    for(i=0;i<match_count;i++){
        ProbMatch * match = OnvifProbeMatches__get_matches(probematches)[i];
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

        OnvifDiscoverer * discoverer = OnvifDiscoverer__new(1,2);
        g_signal_connect(discoverer,"probe-match",G_CALLBACK(_dicovery_found_server_cb),NULL);
        OnvifDiscoverer__start(discoverer);
        
        C_DEBUG("sending probe done...\n");
        return 0;

}