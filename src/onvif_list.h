#ifndef ONVIF_LIST_H_ 
#define ONVIF_LIST_H_

#include "onvif_device.h"
#include <stdlib.h>


typedef struct {
  int device_count;
  OnvifDevice **devices;
} OnvifDeviceList;

SHARD_EXPORT OnvifDeviceList* OnvifDeviceList__create(); 

SHARD_EXPORT void OnvifDeviceList__destroy(OnvifDeviceList* onvifDeviceList);
SHARD_EXPORT void OnvifDeviceList__insert_element(OnvifDeviceList* self, OnvifDevice * record, int index);
SHARD_EXPORT void OnvifDeviceList__remove_element(OnvifDeviceList* self, int index);
SHARD_EXPORT void OnvifDeviceList__clear(OnvifDeviceList* self);

#endif