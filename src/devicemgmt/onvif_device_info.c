#include <stdlib.h>
#include <string.h>
#include "onvif_device_info_local.h"

typedef struct _OnvifDeviceInformation {
    char * manufacturer;
    char * model;
    char * firmwareVersion;
    char * serialNumber;
    char * hardwareId;
} OnvifDeviceInformation;

OnvifDeviceInformation * OnvifDeviceInformation__create(struct _tds__GetDeviceInformationResponse * info){
    OnvifDeviceInformation * self = malloc(sizeof(OnvifDeviceInformation));
    OnvifDeviceInformation__init(self,info);
    return self;
}

void OnvifDeviceInformation__init(OnvifDeviceInformation *self, struct _tds__GetDeviceInformationResponse * info){
    if(info && info->FirmwareVersion){
        self->firmwareVersion = (char*) malloc(strlen(info->FirmwareVersion)+1);
        strcpy(self->firmwareVersion,info->FirmwareVersion);
    } else {
        self->firmwareVersion = NULL;
    }
    
    if(info && info->HardwareId){
        self->hardwareId = (char*) malloc(strlen(info->HardwareId)+1);
        strcpy(self->hardwareId,info->HardwareId);
    } else {
        self->hardwareId = NULL;
    }

    if(info && info->Manufacturer){
        self->manufacturer = (char*) malloc(strlen(info->Manufacturer)+1);
        strcpy(self->manufacturer,info->Manufacturer);
    } else {
        self->manufacturer = NULL;
    }

    if(info && info->Model){
        self->model = (char*) malloc(strlen(info->Model)+1);
        strcpy(self->model,info->Model);
    } else {
        self->model = NULL;
    }

    if(info && info->SerialNumber){
        self->serialNumber = (char*) malloc(strlen(info->SerialNumber)+1);
        strcpy(self->serialNumber,info->SerialNumber);
    } else {
        self->serialNumber = NULL;
    }
}

char * OnvifDeviceInformation__get_manufacturer(OnvifDeviceInformation *self){
    return self->manufacturer;
}

char * OnvifDeviceInformation__get_model(OnvifDeviceInformation *self){
    return self->model;
}

char * OnvifDeviceInformation__get_firmwareVersion(OnvifDeviceInformation *self){
    return self->firmwareVersion;
}

char * OnvifDeviceInformation__get_serialNumber(OnvifDeviceInformation *self){
    return self->serialNumber;
}

char * OnvifDeviceInformation__get_hardwareId(OnvifDeviceInformation *self){
    return self->hardwareId;
}

SHARD_EXPORT void OnvifDeviceInformation__destroy(OnvifDeviceInformation *self){
    if(self){
        if(self->manufacturer)
            free(self->manufacturer);
        if(self->model)
            free(self->model);
        if(self->firmwareVersion)
            free(self->firmwareVersion);
        if(self->serialNumber)
            free(self->serialNumber);
        if(self->hardwareId)
            free(self->hardwareId);
        free(self);
    }
}