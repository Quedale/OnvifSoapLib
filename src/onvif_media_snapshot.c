#include "onvif_media_snapshot.h"
#include <stdlib.h>
#include <string.h>

typedef struct _OnvifSnapshot {
    char * buffer;
    size_t size;
} OnvifSnapshot;

OnvifSnapshot * OnvifSnapshot__create(size_t size, char * buffer){
    OnvifSnapshot * self = malloc(sizeof(OnvifSnapshot));
    OnvifSnapshot__init(self,size,buffer);
    return self;
}

void OnvifSnapshot__init(OnvifSnapshot * self, size_t size, char * buffer){
    self->size = size;
    self->buffer = malloc(size);
    memcpy(self->buffer,buffer,size);
}

void OnvifSnapshot__destroy(OnvifSnapshot * self){
    if(self){
        free(self->buffer);
        free(self);
    }
}

double OnvifSnapshot__get_size(OnvifSnapshot * self){
    return self->size;
}

char * OnvifSnapshot__get_buffer(OnvifSnapshot * self){
    return self->buffer;
}