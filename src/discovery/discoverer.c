#include "discoverer.h"
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif
#include <pthread.h>
#include <stdio.h>
#include "onvif_discovery.h"
#include "clogger.h"

struct DiscoverThreadInput {
    struct UdpDiscoverer * server;
    int retry_count;
    void * user_data;
    void (*callback)(DiscoveryEvent *);
};

void UdpDiscoverer__init(struct UdpDiscoverer* self, void (*found_callback)(DiscoveryEvent *)) {
    self->found_callback = found_callback;
}

struct UdpDiscoverer UdpDiscoverer__create(void (*found_callback)(DiscoveryEvent *)) {
    struct UdpDiscoverer result;
    memset (&result, 0, sizeof (result));
    UdpDiscoverer__init(&result,found_callback);
    return result;
}

void UdpDiscoverer__destroy(struct UdpDiscoverer* self) {
  if (self) {
     free(self);
  }
}

int discovery_event(DiscoveryEvent * event){
    struct DiscoverThreadInput * in = (struct DiscoverThreadInput *) event->data;

    //Remap user_data from thread struct to original user_data
    event->data = in->user_data;

    (in->callback) (event);

    return 0;
}


void UdpDiscoverer__start(struct UdpDiscoverer* self, void *user_data, int retry_count, int timeout) {

    struct DiscoverThreadInput * in = (struct DiscoverThreadInput *) malloc(sizeof(struct DiscoverThreadInput));
    in->server = self;
    in->user_data = user_data;
    in->callback = self->found_callback;
    in->retry_count = retry_count;

    for(int i=0;i<in->retry_count;i++){
        sendProbe(in, timeout, discovery_event);
    }

    free(in);
}
