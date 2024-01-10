#include "onvif_device.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "clogger.h"

int main(void)
{

        C_DEBUG("creating device...\n");
		/*
			The following url isn't compliant.
			I suspect it may be sitting behind NAT.
			The result is that the URL returned by the ONVIF calls all returned an invalid port, forcing a URL correction.
		*/
        OnvifDevice* dev = OnvifDevice__create("http://61.216.97.157:16887/onvif/device_service"); 
        OnvifDevice__set_credentials(dev,"demo", "demo");
		OnvifDevice__authenticate(dev);
        
		char * ip = OnvifDevice__get_ip(dev);
        C_DEBUG("dev : %s\n",ip);
		free(ip);

		OnvifDeviceService * device_service = OnvifDevice__get_device_service(dev);
		OnvifMediaService * media_service = OnvifDevice__get_media_service(dev);
		
        OnvifDeviceInformation * devinfo = OnvifDeviceService__getDeviceInformation(device_service);
        if (devinfo){
			OnvifProfiles * profiles = OnvifMediaService__get_profiles(media_service);
			for (int i = 0; i < OnvifProfiles__get_size(profiles); i++){
				OnvifProfile * profile = OnvifProfiles__get_profile(profiles,i);
				C_DEBUG("Profile name: %s\n", OnvifProfile__get_name(profile));
				C_DEBUG("Profile token: %s\n", OnvifProfile__get_token(profile));

				char * stream_uri = OnvifMediaService__getStreamUri(media_service, i);
				char * snapshot_uri = OnvifMediaService__getSnapshotUri(media_service, i);
				C_DEBUG("StreamUri : %s\n", stream_uri);
				C_DEBUG("SnapshotUri : %s\n",snapshot_uri);

				OnvifSnapshot * snapshot = OnvifMediaService__getSnapshot(media_service, i);
				if (snapshot && OnvifSnapshot__get_size(snapshot) > 0){
					char name[24];
					snprintf(name, sizeof(name), "%d.jpg",i);
					FILE* image = fopen(name, "wb");
					fwrite(OnvifSnapshot__get_buffer(snapshot), OnvifSnapshot__get_size(snapshot), 1, image);
					fclose(image);
				}

				free(stream_uri);
				free(snapshot_uri);
				OnvifSnapshot__destroy(snapshot);
			}
			OnvifProfiles__destroy(profiles);
        }       

		OnvifDeviceInformation__destroy(devinfo);
        OnvifDevice__destroy(dev);
        return 0;

}