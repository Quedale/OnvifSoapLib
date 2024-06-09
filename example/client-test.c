#include "onvif_device.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "clogger.h"
#include <argp.h>

const char *argp_program_version = "0.0";
const char *argp_program_bug_address = "<your@email.address>";
static char doc[] = "Your program description.";
static struct argp_option options[] = { 
    { "ip", 'i', "IP", 0, "IP of the ONVIF device to connect to. (Default: 61.216.97.157)"},
    { "port", 'p', "PORT", 0, "Port of the ONVIF device to connect to (Default: 16887)"},
	{ "protocol", 't', "PROTO", 0, "HTTP or HTTPS (Default: HTTP)"},
	{ "username", 'u', "USER", 0, "WS-Security username (Default: demo)"},
	{ "password", 'w', "PASS", 0, "WS-Security password (Default: demo)"},
    { 0 } 
};

struct arguments {
    char *protocol;
    char *ip;
    char *port;
	char * user;
	char * pass;
};

//main arguments processing function
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
    case 't': arguments->protocol = arg; break;
    case 'i': arguments->ip = arg; break;
    case 'p': arguments->port = arg; break;
	case 'u': arguments->user = arg; break;
	case 'w': arguments->pass = arg; break;
    case ARGP_KEY_ARG: return 0;
    default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

//Initialize argp context
static struct argp argp = { options, parse_opt, NULL, doc, 0, 0, 0 };

int main(int argc, char *argv[])
{

	struct arguments arguments;
    arguments.protocol = "http";
    arguments.ip = "61.216.97.157";
    arguments.port = "16887";
	arguments.user = "demo";
	arguments.pass = "demo";

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

	char url[1024];
	sprintf(url, "%s://%s:%s/onvif/device_service",arguments.protocol,arguments.ip,arguments.port);
	
	C_DEBUG("creating device... [%s]\n", url);
	/*
		The following url isn't compliant.
		I suspect it may be sitting behind NAT.
		The result is that the URL returned by the ONVIF calls all returned an invalid port, forcing a URL correction.
	*/
	OnvifDevice* dev = OnvifDevice__create(url); 
	OnvifDevice__set_credentials(dev,arguments.user, arguments.pass);
	OnvifDevice__authenticate(dev);
	
	//TODO Support context-aware error handling instead of global pointer
	if(OnvifDevice__get_last_error(dev) != ONVIF_ERROR_NONE){
		C_ERROR("Device encountered an authentication error.");
		OnvifDevice__destroy(dev);
		return 1;
	}
	
	char * host = OnvifDevice__get_host(dev);
	C_DEBUG("dev : %s\n",host);
	free(host);

	OnvifDeviceService * device_service = OnvifDevice__get_device_service(dev);

	char * hostname = OnvifDeviceService__getHostname(device_service);
	C_DEBUG("Device hostname : %s",hostname);
	free(hostname);
	
	OnvifScopes * scopes = OnvifDeviceService__getScopes(device_service);
	char * name = OnvifScopes__extract_scope(scopes,"name");
	char * hardware = OnvifScopes__extract_scope(scopes,"hardware");
	char * location = OnvifScopes__extract_scope(scopes,"location");
	C_DEBUG("OnvifScope name : %s",name);
	C_DEBUG("OnvifScope hardware : %s",hardware);
	C_DEBUG("OnvifScope location : %s",location);
	free(name);
	free(hardware);
	free(location);
	OnvifScopes__destroy(scopes);

	OnvifMediaService * media_service = OnvifDevice__get_media_service(dev);
	
	OnvifDeviceInformation * devinfo = OnvifDeviceService__getDeviceInformation(device_service);
	if (devinfo){
		OnvifProfiles * profiles = OnvifMediaService__get_profiles(media_service);
		for (int i = 0; i < OnvifProfiles__get_size(profiles); i++){
			OnvifProfile * profile = OnvifProfiles__get_profile(profiles,i);
			C_DEBUG("Profile name: %s\n", OnvifProfile__get_name(profile));
			C_DEBUG("Profile token: %s\n", OnvifProfile__get_token(profile));

			char * stream_uri = OnvifMediaService__getStreamUri(media_service, i);
			C_DEBUG("StreamUri : %s\n", stream_uri);
			free(stream_uri);

			OnvifMediaServiceCapabilities * media_caps = OnvifMediaService__getServiceCapabilities(media_service);
            if(!OnvifMediaServiceCapabilities__get_snapshot_uri(media_caps)){
				C_INFO("SnapshotUri feature not supported. Trying anyway...");
			}
			OnvifSnapshotUri * snapshot_uri = OnvifMediaService__getSnapshotUri(media_service, i);
			char * snap_uri = OnvifSnapshotUri__get_uri(snapshot_uri);

			C_DEBUG("SnapshotUri : %s\n",snap_uri);

			OnvifSnapshot * snapshot = OnvifMediaService__getSnapshot(media_service, i);
			SoapFault * fault = SoapObject__get_fault(SOAP_OBJECT(snapshot));
			switch(*fault){
				case SOAP_FAULT_NONE:
					if(!OnvifMediaServiceCapabilities__get_snapshot_uri(media_caps)){
						C_WARN("SnapshotUri suprisingly works!!");
					}
					if(OnvifSnapshot__get_size(snapshot) > 0){
						char name[24];
						snprintf(name, sizeof(name), "%d.jpg",i);
						FILE* image = fopen(name, "wb");
						fwrite(OnvifSnapshot__get_buffer(snapshot), OnvifSnapshot__get_size(snapshot), 1, image);
						fclose(image);
						C_DEBUG("Successfully retrieved snapshot. Saved at './%d.jpg'",i);
					} else {
						C_ERROR("Snapshot return is an empty buffer");
					}
					break;
				case SOAP_FAULT_ACTION_NOT_SUPPORTED:
					C_WARN("SnapshotUri not supported.");
					break;
				case SOAP_FAULT_CONNECTION_ERROR:
				case SOAP_FAULT_NOT_VALID:
				case SOAP_FAULT_UNAUTHORIZED:
				case SOAP_FAULT_UNEXPECTED:
				default:
					C_ERROR("Failed to retrieve snapshot.");
					break;
			}

			g_object_unref(snapshot_uri);
			g_object_unref(snapshot);
		}
		OnvifProfiles__destroy(profiles);
	}      

	OnvifDeviceInformation__destroy(devinfo);
	OnvifDevice__destroy(dev);
	return 0;

}