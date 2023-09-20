#include "onvif_device_interface_local.h"

typedef struct _OnvifInterface {
    char * token; ///< Required attribute.
    int enabled; ///< Required element.

    //Info ///< Optional element.
    int has_info;
    char * name;
    char * mac;
    int mtu;

    //IPv4 ///< Optional element.
    //__sizeManual
    int ipv4_enabled;
    int ipv4_dhcp; // DHCP
    int ipv4_manual_count;
    char ** ipv4_manual; // Manual
    char * ipv4_link_local; // LinkLocal
    char * ipv4_from_dhcp; // FromDHCP

    //TODO Support IPv6

} OnvifInterface;

typedef struct _OnvifInterfaces {
    int count;
    OnvifInterface ** interfaces;
} OnvifInterfaces;

OnvifInterfaces * OnvifInterfaces__create(struct _tds__GetNetworkInterfacesResponse * intefaces){
    OnvifInterfaces * self = malloc(sizeof(OnvifInterfaces));
    OnvifInterfaces__init(self,intefaces);
    return self;
}

void OnvifInterfaces__init(OnvifInterfaces * self, struct _tds__GetNetworkInterfacesResponse * resp){  
    self->interfaces = malloc(0);
    self->count = 0;

    int i;
    for(i=0;i<resp->__sizeNetworkInterfaces;i++){
        struct tt__NetworkInterface interface = resp->NetworkInterfaces[i];
        OnvifInterface * onvifinterface = malloc(sizeof(OnvifInterface));
        onvifinterface->enabled = interface.Enabled;
        onvifinterface->token = (char*) malloc(strlen(interface.token)+1);
        strcpy(onvifinterface->token,interface.token);
        
        if(interface.Info){
            onvifinterface->has_info = 1;
            onvifinterface->name = (char*) malloc(strlen(interface.Info->Name)+1);
            strcpy(onvifinterface->name,interface.Info->Name);
            onvifinterface->mac = (char*) malloc(strlen(interface.Info->HwAddress)+1);
            strcpy(onvifinterface->mac,interface.Info->HwAddress);

            onvifinterface->mtu = interface.Info->MTU[0];
        } else {
            onvifinterface->has_info = 0;
        }
        
        //struct tt__IPv4NetworkInterface*     IPv4
        if(interface.IPv4){
            onvifinterface->ipv4_enabled = interface.IPv4->Enabled;
            onvifinterface->ipv4_dhcp = interface.IPv4->Config->DHCP;
            onvifinterface->ipv4_manual_count = interface.IPv4->Config->__sizeManual;
            onvifinterface->ipv4_manual = NULL;
            onvifinterface->ipv4_link_local = NULL;
            onvifinterface->ipv4_from_dhcp = NULL;

            //Manually configured IPs
            if(interface.IPv4->Config->__sizeManual > 0){
                struct tt__PrefixedIPv4Address * manuals = interface.IPv4->Config->Manual;
                for(int a=0;a<onvifinterface->ipv4_manual_count;a++){
                    struct tt__PrefixedIPv4Address manual = manuals[a];
                    onvifinterface->ipv4_manual = realloc(onvifinterface->ipv4_manual,sizeof(char *) * onvifinterface->ipv4_manual_count);
                    onvifinterface->ipv4_manual[onvifinterface->ipv4_manual_count-1] = malloc(strlen(manual.Address) + 1);
                    strcpy(onvifinterface->ipv4_manual[onvifinterface->ipv4_manual_count-1],manual.Address);
                }
            }

            //Link local address.
            if(interface.IPv4->Config->LinkLocal){
                onvifinterface->ipv4_link_local = malloc(strlen(interface.IPv4->Config->LinkLocal->Address)+1);
                strcpy(onvifinterface->ipv4_link_local,interface.IPv4->Config->LinkLocal->Address);
            }

            //DHCP IP
            if(interface.IPv4->Config->FromDHCP){
                onvifinterface->ipv4_from_dhcp = malloc(strlen(interface.IPv4->Config->FromDHCP->Address)+1);
                strcpy(onvifinterface->ipv4_from_dhcp,interface.IPv4->Config->FromDHCP->Address);
            }
        } else {
            onvifinterface->ipv4_enabled = 0;
        }

        //TODO IPv6

        self->count++;
        self->interfaces = realloc (self->interfaces, sizeof (OnvifInterface *) * self->count);
        self->interfaces[self->count-1] = onvifinterface;
    }
}

int OnvifInterfaces__get_count(OnvifInterfaces * self){
    return self->count;
}

OnvifInterface * OnvifInterfaces__get_interface(OnvifInterfaces * self, int index){
    if(index >= self->count){
        return NULL;
    }
    return self->interfaces[index];
}

void OnvifInterfaces__destroy(OnvifInterfaces * self){
    if(self){
        for(int i=0;i<self->count;i++){
            free(self->interfaces[i]->token);
            if(self->interfaces[i]->name)
                free(self->interfaces[i]->name);
            if(self->interfaces[i]->mac)
                free(self->interfaces[i]->mac);
            if(self->interfaces[i]->ipv4_manual){
                for(int a=0;a<self->interfaces[i]->ipv4_manual_count;a++){
                    free(self->interfaces[i]->ipv4_manual[a]);
                }
                free(self->interfaces[i]->ipv4_manual);
            }
            if(self->interfaces[i]->ipv4_link_local)
                free(self->interfaces[i]->ipv4_link_local);
            if(self->interfaces[i]->ipv4_from_dhcp)
                free(self->interfaces[i]->ipv4_from_dhcp);
            
            free(self->interfaces[i]);
        }

        free(self);
    }
}


char * OnvifInterface__get_token(OnvifInterface * self){
    return self->token;
}

int OnvifInterface__get_enabled(OnvifInterface * self){
    return self->enabled;
}

int OnvifInterface__has_info(OnvifInterface * self){
    return self->has_info;
}

char * OnvifInterface__get_name(OnvifInterface * self){
    return self->name;
}

char * OnvifInterface__get_mac(OnvifInterface * self){
    return self->mac;
}

int OnvifInterface__get_mtu(OnvifInterface * self){
    return self->mtu;
}

int OnvifInterface__is_ipv4_enabled(OnvifInterface * self){
    return self->ipv4_enabled;
}

int OnvifInterface__get_ipv4_dhcp(OnvifInterface * self){
    return self->ipv4_dhcp;
}

int OnvifInterface__get_ipv4_manual_count(OnvifInterface * self){
    return self->ipv4_manual_count;
}

char ** OnvifInterface__get_ipv4_manual(OnvifInterface * self){
    return self->ipv4_manual;
}

char * OnvifInterface__get_ipv4_link_local(OnvifInterface * self){
    return self->ipv4_link_local;
}

char * OnvifInterface__get_ipv4_from_dhcp(OnvifInterface * self){
    return self->ipv4_from_dhcp;
}