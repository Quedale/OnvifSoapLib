#include "onvif_device.h"
#include <stddef.h>
#include <stdio.h>

int main(int argc, char** argv)
{

        printf("creating device...\n");
        OnvifDevice dev = OnvifDevice__create("http://172.31.243.168:8081/onvif/device_service"); 

        printf("dev : %s\n",dev.ip);

        OnvifDeviceInformation * devinfo = OnvifDevice__device_getDeviceInformation(&dev);

        printf("model : %s\n",devinfo->model);
        return 0;

}