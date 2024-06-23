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

void loop_profiles(OnvifMediaProfiles * profiles, OnvifMediaService * media_service, OnvifDevice * dev){
	for (int i = 0; i < OnvifMediaProfiles__get_size(profiles); i++){
		OnvifProfile * profile = OnvifMediaProfiles__get_profile(profiles,i);
		C_DEBUG("Profile name: %s\n", OnvifProfile__get_name(profile));
		C_DEBUG("Profile token: %s\n", OnvifProfile__get_token(profile));

		OnvifMediaUri * stream_uri = OnvifMediaService__getStreamUri(media_service, i);
		SoapFault * fault = SoapObject__get_fault(SOAP_OBJECT(stream_uri));
		char * uri;
		switch(*fault){
			case SOAP_FAULT_NONE:
				uri = OnvifMediaUri__get_uri(stream_uri);
				if(!uri){
					ONVIF_MEDIA_ERROR("[%s] No StreamURI provided...",dev);
				} else {
					ONVIF_MEDIA_ERROR("[%s] StreamURI : %s",dev, uri);
				}
				break;
			case SOAP_FAULT_ACTION_NOT_SUPPORTED:
			case SOAP_FAULT_CONNECTION_ERROR:
			case SOAP_FAULT_NOT_VALID:
			case SOAP_FAULT_UNAUTHORIZED:
			case SOAP_FAULT_UNEXPECTED:
			default:
				ONVIF_MEDIA_ERROR("[%s] Failed to retrieve StreamURI...",dev);
				break;
		}

		g_object_unref(stream_uri);

		OnvifMediaServiceCapabilities * media_caps = OnvifMediaService__getServiceCapabilities(media_service);
		if(!OnvifMediaServiceCapabilities__get_snapshot_uri(media_caps)){
			C_INFO("SnapshotUri feature not supported. Trying anyway...");
		}
		OnvifMediaUri * snapshot_uri = OnvifMediaService__getSnapshotUri(media_service, i);
		fault = SoapObject__get_fault(SOAP_OBJECT(snapshot_uri));
		switch(*fault){
			case SOAP_FAULT_NONE:
				if(!OnvifMediaServiceCapabilities__get_snapshot_uri(media_caps)){
					C_WARN("GetSnapshotUri suprisingly works!!");
				}
				char * snap_uri = OnvifMediaUri__get_uri(snapshot_uri);
				if(snap_uri){
					C_DEBUG("SnapshotUri : %s\n",snap_uri);
				} else {
					C_DEBUG("No SnapshotUri provided.\n");
					g_object_unref(snapshot_uri);
					continue;
				}
				break;
			case SOAP_FAULT_ACTION_NOT_SUPPORTED:
				C_WARN("SnapshotUri not supported.");
				g_object_unref(snapshot_uri);
				continue;
			case SOAP_FAULT_CONNECTION_ERROR:
			case SOAP_FAULT_NOT_VALID:
			case SOAP_FAULT_UNAUTHORIZED:
			case SOAP_FAULT_UNEXPECTED:
			default:
				C_ERROR("Failed to retrieve snapshot.");
				g_object_unref(snapshot_uri);
				continue;
		}
		g_object_unref(snapshot_uri);

		OnvifSnapshot * snapshot = OnvifMediaService__getSnapshot(media_service, i);
		fault = SoapObject__get_fault(SOAP_OBJECT(snapshot));
		switch(*fault){
			case SOAP_FAULT_NONE:
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
		g_object_unref(snapshot);
	}
}
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
	if(!OnvifDevice__is_authenticated(dev)){
		C_ERROR("Device encountered an authentication error.");
		OnvifDevice__destroy(dev);
		return 1;
	}
	
	char * host = OnvifDevice__get_host(dev);
	C_DEBUG("dev : %s\n",host);
	free(host);

	OnvifDeviceService * device_service = OnvifDevice__get_device_service(dev);

	OnvifDeviceHostnameInfo * hostname = OnvifDeviceService__getHostname(device_service);
	SoapFault * fault = SoapObject__get_fault(SOAP_OBJECT(hostname));
	switch(*fault){
		case SOAP_FAULT_NONE:
			C_DEBUG("Device hostname : %s",OnvifDeviceHostnameInfo__get_name(hostname));
			break;
		case SOAP_FAULT_ACTION_NOT_SUPPORTED:
		case SOAP_FAULT_CONNECTION_ERROR:
		case SOAP_FAULT_NOT_VALID:
		case SOAP_FAULT_UNAUTHORIZED:
		case SOAP_FAULT_UNEXPECTED:
		default:
			C_ERROR("Failed to retrieve hostname %d",*fault);
			break;
	}
	g_object_unref(hostname);
	
	OnvifScopes * scopes = OnvifDeviceService__getScopes(device_service);
	fault = SoapObject__get_fault(SOAP_OBJECT(scopes));
	switch(*fault){
		case SOAP_FAULT_NONE:
			char * name = OnvifScopes__extract_scope(scopes,"name");
			char * hardware = OnvifScopes__extract_scope(scopes,"hardware");
			char * location = OnvifScopes__extract_scope(scopes,"location");
			C_DEBUG("OnvifScope name : %s",name);
			C_DEBUG("OnvifScope hardware : %s",hardware);
			C_DEBUG("OnvifScope location : %s",location);
			free(name);
			free(hardware);
			free(location);
			break;
		case SOAP_FAULT_ACTION_NOT_SUPPORTED:
		case SOAP_FAULT_CONNECTION_ERROR:
		case SOAP_FAULT_NOT_VALID:
		case SOAP_FAULT_UNAUTHORIZED:
		case SOAP_FAULT_UNEXPECTED:
		default:
			C_ERROR("Failed to retrieve Device Scopes.");
			break;
	}
	g_object_unref(scopes);

	OnvifMediaService * media_service = OnvifDevice__get_media_service(dev);
	
	OnvifDeviceInformation * devinfo = OnvifDeviceService__getDeviceInformation(device_service);
	fault = SoapObject__get_fault(SOAP_OBJECT(devinfo));
	switch(*fault){
		case SOAP_FAULT_NONE:
			C_DEBUG("Device Manufacturer : %s", OnvifDeviceInformation__get_manufacturer(devinfo));
			C_DEBUG("Device Model : %s", OnvifDeviceInformation__get_model(devinfo));
			C_DEBUG("Device FirmwareVersion : %s", OnvifDeviceInformation__get_firmwareVersion(devinfo));
			C_DEBUG("Device SerialNumber : %s", OnvifDeviceInformation__get_serialNumber(devinfo));
			C_DEBUG("Device HardwareId : %s", OnvifDeviceInformation__get_hardwareId(devinfo));
			break;
		case SOAP_FAULT_ACTION_NOT_SUPPORTED:
		case SOAP_FAULT_CONNECTION_ERROR:
		case SOAP_FAULT_NOT_VALID:
		case SOAP_FAULT_UNAUTHORIZED:
		case SOAP_FAULT_UNEXPECTED:
		default:
			C_ERROR("Failed to retrieve DeviceInformation.");
			break;
	}
	g_object_unref(devinfo);

	OnvifMediaProfiles * profiles = OnvifMediaService__get_profiles(media_service);
	fault = SoapObject__get_fault(SOAP_OBJECT(profiles));
	switch(*fault){
		case SOAP_FAULT_NONE:
			loop_profiles(profiles,media_service, dev);
			break;
		case SOAP_FAULT_ACTION_NOT_SUPPORTED:
		case SOAP_FAULT_CONNECTION_ERROR:
		case SOAP_FAULT_NOT_VALID:
		case SOAP_FAULT_UNAUTHORIZED:
		case SOAP_FAULT_UNEXPECTED:
		default:
			C_ERROR("Failed to retrieve MediaProfiles.");
			break;
	}
	g_object_unref(profiles);

	OnvifDevice__destroy(dev);
	return 0;

}