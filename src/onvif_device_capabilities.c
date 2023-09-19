#include "onvif_device_capabilities_local.h"

typedef struct _OnvifMedia {
    char *xaddr;
    //TODO StreamingCapabilities
    //TODO Extension
} OnvifMedia;

typedef struct _OnvifCapabilities {
    OnvifMedia *media;
} OnvifCapabilities;

OnvifCapabilities * OnvifCapabilities__create(struct _tds__GetCapabilitiesResponse * response){
    OnvifCapabilities * self = malloc(sizeof(OnvifCapabilities));
    OnvifCapabilities__init(self, response);
    return self;
}

void OnvifCapabilities__init(OnvifCapabilities * self, struct _tds__GetCapabilitiesResponse * response){
    OnvifMedia* media =  (OnvifMedia *) malloc(sizeof(OnvifMedia));
    media->xaddr = malloc(strlen(response->Capabilities->Media->XAddr)+1);
    strcpy(media->xaddr,response->Capabilities->Media->XAddr);
    self->media = media;
}

void OnvifCapabilities__destroy(OnvifCapabilities * self){
    if(self){
        free(self->media->xaddr);
        free(self->media);
        free(self);
    }
}

OnvifMedia * OnvifCapabilities__get_media(OnvifCapabilities * self){
    return self->media;
}

char * OnvifMedia__get_address(OnvifMedia * self){
    return self->xaddr;
}