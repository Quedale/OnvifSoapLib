#ifndef ONVIF_LIST_H_ 
#define ONVIF_LIST_H_

#include "onvif_device.h"
#include <stdlib.h>


typedef struct {
  int device_count;
  OnvifDevice **devices;
} OnvifDeviceList;

__attribute__ ((visibility("default"))) 
extern OnvifDeviceList* OnvifDeviceList__create(); 

__attribute__ ((visibility("default"))) 
extern void OnvifDeviceList__destroy(OnvifDeviceList* onvifDeviceList);
__attribute__ ((visibility("default")))  
extern void OnvifDeviceList__insert_element(OnvifDeviceList* self, OnvifDevice * record, int index);
__attribute__ ((visibility("default"))) 
extern void OnvifDeviceList__remove_element(OnvifDeviceList* self, int index);
__attribute__ ((visibility("default"))) 
extern void OnvifDeviceList__clear(OnvifDeviceList* self);

#endif