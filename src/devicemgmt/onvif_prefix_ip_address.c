#include "onvif_prefix_ip_address_local.h"
#include "clogger.h"
#include <glib.h>

typedef struct _OnvifPrefixedIPAddress {
    char * address;
    int prefix;
    gboolean ipv4;
} OnvifPrefixedIPAddress;

OnvifPrefixedIPAddress * 
OnvifPrefixedIPAddress__create(struct tt__PrefixedIPv4Address * addr){
    OnvifPrefixedIPAddress * ret = malloc(sizeof(OnvifPrefixedIPAddress));
    if(addr->Address){
        ret->address = malloc(strlen(addr->Address) + 1);
        strcpy(ret->address,addr->Address);
    } else {
        ret->address = malloc(strlen("No IPv4 Manual Address defined") + 1);
        strcpy(ret->address,"No IPv4 Manual Address defined");
        C_ERROR("No IPv4 Manual Address defined in soap response");
    }

    ret->prefix = addr->PrefixLength;
    return ret;
}

OnvifPrefixedIPAddress * 
OnvifPrefixedIPAddress__create4(struct tt__PrefixedIPv4Address * addr){
    if(!addr){
        return NULL;
    }

    OnvifPrefixedIPAddress * ret = OnvifPrefixedIPAddress__create(addr);
    ret->ipv4 = TRUE;
    return ret;
}

OnvifPrefixedIPAddress * 
OnvifPrefixedIPAddress__create6(struct tt__PrefixedIPv6Address * addr){
    if(!addr){
        return NULL;
    }

    OnvifPrefixedIPAddress * ret = OnvifPrefixedIPAddress__create((struct tt__PrefixedIPv4Address *)addr);
    ret->ipv4 = FALSE;
    return ret;
}

char * 
OnvifPrefixedIPAddress__get_address(OnvifPrefixedIPAddress * self){
    return self->address;
}

int 
OnvifPrefixedIPAddress__get_prefix(OnvifPrefixedIPAddress * self){
    return self->prefix;
}

void 
OnvifPrefixedIPAddress__destroy(OnvifPrefixedIPAddress * self){
    if(self->address)
        free(self->address);
    self->prefix = 0;
    free(self);
}