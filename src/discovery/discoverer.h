
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


struct UdpDiscoverer {
    void (*found_callback)(DiscoveryEvent *);
};

__attribute__ ((visibility("default"))) 
extern struct UdpDiscoverer UdpDiscoverer__create(void (*found_callback)(DiscoveryEvent *)); 
__attribute__ ((visibility("default"))) 
extern void UdpDiscoverer__destroy(struct UdpDiscoverer* self); 
__attribute__ ((visibility("default"))) 
extern void UdpDiscoverer__start(struct UdpDiscoverer* self, void * user_data, int retry_count, int timeout);

#endif