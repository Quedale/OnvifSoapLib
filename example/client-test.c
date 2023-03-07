#include "onvif_device.h"
#include <stddef.h>
#include <stdio.h>


int main(int argc, char** argv)
{

        printf("creating device...\n");
        OnvifDevice* dev = OnvifDevice__create("http://192.168.0.121/onvif/device_service"); 
        OnvifDevice_set_credentials(dev,"admin", "Admin123");
        OnvifDevice_authenticate(dev);
        
        // printf("dev : %s\n",dev->ip);

        OnvifDeviceInformation * devinfo = OnvifDevice__device_getDeviceInformation(dev);
        if (devinfo){
			OnvifDevice_get_profiles(dev);
			for (int i = 0; i < dev->sizeSrofiles; i++){
				printf("Profile name: %s\n", dev->profiles[i].name);
				printf("Profile token: %s\n", dev->profiles[i].token);

				printf("StreamUri : %s\n",OnvifDevice__media_getStreamUri(dev, i));
				printf("SnapshotUri : %s\n",OnvifDevice__media_getSnapshotUri(dev, i));

				struct chunk * image_buf = OnvifDevice__media_getSnapshot(dev, i);
				if (image_buf->size){
					char name[24];
					snprintf(name, sizeof(name), "%d.jpg",i);
					FILE* image = fopen(name, "wb");
					fwrite(image_buf->buffer, image_buf->size, 1, image);
					fclose(image);
					free(image_buf->buffer);
					free(image_buf);
				}
			}
        }       

        OnvifDevice__destroy(dev);
        return 0;

}