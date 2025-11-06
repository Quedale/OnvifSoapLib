
#ifndef UDP_DISCOVERER_H_   /* Include guard */
#define UDP_DISCOVERER_H_

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include "onvif_discovery.h"
#include "shard_export.h"

struct UdpDiscoverer {
    void (*found_callback)(DiscoveryEvent *);
};

SHARD_EXPORT struct UdpDiscoverer UdpDiscoverer__create(void (*found_callback)(DiscoveryEvent *)); 
SHARD_EXPORT void UdpDiscoverer__destroy(struct UdpDiscoverer* self); 
SHARD_EXPORT void UdpDiscoverer__start(struct UdpDiscoverer* self, void * user_data, int retry_count, int timeout);

#endif
