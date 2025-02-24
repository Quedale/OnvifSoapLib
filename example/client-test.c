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

void print_media_audio_source_configs(OnvifMediaService * media_service, int profile_index){
	OnvifMediaAudioSourceConfigs * audio_sources = OnvifMediaService__getAudioSourceConfigurations(media_service,profile_index);
	SoapFault * fault = SoapObject__get_fault(SOAP_OBJECT(audio_sources));
    switch(*fault){
        case SOAP_FAULT_NONE:
			if(OnvifMediaAudioSourceConfigs__get_size(audio_sources) <= 0) C_DEBUG("Device doesn't have a microphone");
			for(int i=0;i<OnvifMediaAudioSourceConfigs__get_size(audio_sources);i++){
				OnvifMediaAudioSourceConfig * audio_source = OnvifMediaAudioSourceConfigs__get_config(audio_sources,i);
				C_DEBUG("----------AudioSourceConfig------------");
				C_DEBUG("Token : %s",OnvifMediaAudioSourceConfig__get_token(audio_source));
				C_DEBUG("Name : %s",OnvifMediaAudioSourceConfig__get_name(audio_source));
				C_DEBUG("UseCount : %d",OnvifMediaAudioSourceConfig__get_use_count(audio_source));
				C_DEBUG("SourceToken : %s",OnvifMediaAudioSourceConfig__get_source_token(audio_source));
			}
            break;
        case SOAP_FAULT_CONNECTION_ERROR:
        case SOAP_FAULT_NOT_VALID:
        case SOAP_FAULT_UNAUTHORIZED:
        case SOAP_FAULT_ACTION_NOT_SUPPORTED:
        case SOAP_FAULT_UNEXPECTED:
        default:
            C_ERROR("Failed to retrieve AudioSourceConfig");
            break;
    }
	g_object_unref(audio_sources);
}

void print_media_audio_output_configs(OnvifMediaService * media_service, int profile_index){
	OnvifMediaAudioOutputConfigs * audio_outputs = OnvifMediaService__getAudioOutputConfigurations(media_service,profile_index);
	SoapFault * fault = SoapObject__get_fault(SOAP_OBJECT(audio_outputs));
    switch(*fault){
        case SOAP_FAULT_NONE:
			if(OnvifMediaAudioOutputConfigs__get_size(audio_outputs) <= 0) C_DEBUG("Device doesn't have a speaker");
			for(int i=0;i<OnvifMediaAudioOutputConfigs__get_size(audio_outputs);i++){
				OnvifMediaAudioOutputConfig * audio_output = OnvifMediaAudioOutputConfigs__get_config(audio_outputs,i);
				C_DEBUG("----------AudioOutputConfig------------");
				C_DEBUG("Token : %s",OnvifMediaAudioOutputConfig__get_token(audio_output));
				C_DEBUG("Name : %s",OnvifMediaAudioOutputConfig__get_name(audio_output));
				C_DEBUG("UseCount : %d",OnvifMediaAudioOutputConfig__get_use_count(audio_output));
				C_DEBUG("OutputToken : %s",OnvifMediaAudioOutputConfig__get_output_token(audio_output));
			}
            break;
        case SOAP_FAULT_CONNECTION_ERROR:
        case SOAP_FAULT_NOT_VALID:
        case SOAP_FAULT_UNAUTHORIZED:
        case SOAP_FAULT_ACTION_NOT_SUPPORTED:
        case SOAP_FAULT_UNEXPECTED:
        default:
            C_ERROR("Failed to retrieve Device AudioOutputConfig");
            break;
    }
	g_object_unref(audio_outputs);
}

void print_media_audio_output_config_options(OnvifMediaService * media_service, int profile_index){
	OnvifMediaAudioOutputConfigOptions * audio_src_config_opts = OnvifMediaService__getAudioOutputConfigurationOptions(media_service,profile_index);
	SoapFault * fault = SoapObject__get_fault(SOAP_OBJECT(audio_src_config_opts));
    switch(*fault){
        case SOAP_FAULT_NONE:
			C_DEBUG("----------AudioOutputConfigOption------------");
			int count = OnvifMediaAudioOutputConfigOptions__ouput_tokens_available_count(audio_src_config_opts);
			for(int i=0;i<count;i++){
				char * out_token_available = OnvifMediaAudioOutputConfigOptions__get_ouput_token_available(audio_src_config_opts,i);
				C_DEBUG("\tOutputTokenAvailable %i: %s",i,out_token_available);
			}
			count = OnvifMediaAudioOutputConfigOptions__get_send_primacy_options_count(audio_src_config_opts);
			for(int i=0;i<count;i++){
				char * send_primacy_opt = OnvifMediaAudioOutputConfigOptions__get_send_primacy_option(audio_src_config_opts,i);
				C_DEBUG("\tSendPrimacyOption %i: %s",i,send_primacy_opt);
			}
			C_DEBUG("\t Minimum/Maximum : %d/%d",OnvifMediaAudioOutputConfigOptions__get_min_level(audio_src_config_opts),OnvifMediaAudioOutputConfigOptions__get_max_level(audio_src_config_opts));
            break;
        case SOAP_FAULT_CONNECTION_ERROR:
        case SOAP_FAULT_NOT_VALID:
        case SOAP_FAULT_UNAUTHORIZED:
        case SOAP_FAULT_ACTION_NOT_SUPPORTED:
        case SOAP_FAULT_UNEXPECTED:
        default:
            C_ERROR("Failed to retrieve Device AudioOutputConfigOptions");
            break;
    }
	g_object_unref(audio_src_config_opts);
}

void loop_media_profiles(OnvifMediaProfiles * profiles, OnvifMediaService * media_service, OnvifDevice * dev){
	for (int i = 0; i < OnvifMediaProfiles__get_size(profiles); i++){
		OnvifMediaProfile * profile = OnvifMediaProfiles__get_profile(profiles,i);
		C_DEBUG("Media Profile name: %s\n", OnvifMediaProfile__get_name(profile));
		C_DEBUG("Media Profile token: %s\n", OnvifMediaProfile__get_token(profile));

		//TODO Filter below calls only if supported
		print_media_audio_source_configs(media_service,i);
		print_media_audio_output_configs(media_service,i);
		print_media_audio_output_config_options(media_service,i);
		
		OnvifUri * stream_uri = OnvifMediaService__getStreamUri(media_service, i);
		SoapFault * fault = SoapObject__get_fault(SOAP_OBJECT(stream_uri));
		const char * uri;
		switch(*fault){
			case SOAP_FAULT_NONE:
				uri = OnvifUri__get_uri(stream_uri);
				if(!uri){
					ONVIF_MEDIA_ERROR("[%s] No StreamURI provided...",dev);
				} else {
					ONVIF_MEDIA_INFO("[%s] StreamURI : %s",dev, uri);
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
		OnvifUri * snapshot_uri = OnvifMediaService__getSnapshotUri(media_service, i);
		fault = SoapObject__get_fault(SOAP_OBJECT(snapshot_uri));
		switch(*fault){
			case SOAP_FAULT_NONE:
				if(!OnvifMediaServiceCapabilities__get_snapshot_uri(media_caps)){
					C_WARN("GetSnapshotUri suprisingly works!!");
				}
				char * snap_uri = OnvifUri__get_uri(snapshot_uri);
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
		g_object_unref(media_caps);
		g_object_unref(snapshot_uri);

		OnvifMediaSnapshot * snapshot = OnvifMediaService__getSnapshot(media_service, i);
		fault = SoapObject__get_fault(SOAP_OBJECT(snapshot));
		switch(*fault){
			case SOAP_FAULT_NONE:
				if(OnvifMediaSnapshot__get_size(snapshot) > 0){
					char name[24];
					snprintf(name, sizeof(name), "%d.jpg",i);
					FILE* image = fopen(name, "wb");
					fwrite(OnvifMediaSnapshot__get_buffer(snapshot), OnvifMediaSnapshot__get_size(snapshot), 1, image);
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

void print_scopes(OnvifDeviceService * device_service){
	char * name, * hardware, * location;

	OnvifScopes * scopes = OnvifDeviceService__getScopes(device_service);
	SoapFault * fault = SoapObject__get_fault(SOAP_OBJECT(scopes));
	switch(*fault){
		case SOAP_FAULT_NONE:
			name = OnvifScopes__extract_scope(scopes,"name");
			hardware = OnvifScopes__extract_scope(scopes,"hardware");
			location = OnvifScopes__extract_scope(scopes,"location");
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
}

void print_device_capabilites(OnvifDeviceService * device_service){
	OnvifDeviceCapabilities * device_caps;
	OnvifSystemCapabilities * sys_caps;
	int major, minor;

	OnvifCapabilities* caps = OnvifDeviceService__getCapabilities(device_service);
	SoapFault * fault = SoapObject__get_fault(SOAP_OBJECT(caps));
    switch(*fault){
        case SOAP_FAULT_NONE:
            device_caps = OnvifCapabilities__get_device(caps);
			if(device_caps){
				sys_caps = OnvifDeviceCapabilities__get_system(device_caps);
				if(sys_caps){
					major = OnvifSystemCapabilities__get_majorVersion(sys_caps);
					minor = OnvifSystemCapabilities__get_minorVersion(sys_caps);
					C_DEBUG("ONVIF Version : %d.%02d",major,minor);
				} else {
					C_WARN("No SystemCapabilities defined");
				}
			} else {
				C_WARN("No DeviceCapabilities defined");
			}
            break;
        case SOAP_FAULT_CONNECTION_ERROR:
        case SOAP_FAULT_NOT_VALID:
        case SOAP_FAULT_UNAUTHORIZED:
        case SOAP_FAULT_ACTION_NOT_SUPPORTED:
        case SOAP_FAULT_UNEXPECTED:
        default:
            C_ERROR("Failed to retrieve Device Capabilities");
            break;
    }
	g_object_unref(caps);
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
	OnvifDevice* dev = OnvifDevice__new(url); 
	OnvifDevice__set_credentials(dev,arguments.user, arguments.pass);
	OnvifDevice__authenticate(dev);
	
	//TODO Support context-aware error handling instead of global pointer
	if(!OnvifDevice__is_authenticated(dev)){
		C_ERROR("Device encountered an authentication error.");
		g_object_unref(dev);
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
	

	OnvifDeviceInterfaces * interfaces = OnvifDeviceService__getNetworkInterfaces(device_service);
	fault = SoapObject__get_fault(SOAP_OBJECT(interfaces));
	switch(*fault){
		case SOAP_FAULT_NONE:
				C_INFO("count : %d",OnvifDeviceInterfaces__get_count(interfaces));
				for(int a=0;a<OnvifDeviceInterfaces__get_count(interfaces);a++){
					OnvifDeviceInterface * inet = OnvifDeviceInterfaces__get_interface(interfaces,a);
					OnvifIPv4Configuration * v4 = OnvifDeviceInterface__get_ipv4(inet);
					C_INFO("name : %s",OnvifDeviceInterface__get_name(inet));
					C_INFO("mtu : %d",OnvifDeviceInterface__get_mtu(inet));
					if(v4){
						C_INFO("-------------------------------------------");
						C_INFO("enabled : %d",OnvifIPv4Configuration__is_enabled(v4));
						C_INFO("manual count : %d",OnvifIPv4Configuration__get_manual_count(v4));
						for(int i=0;i<OnvifIPv4Configuration__get_manual_count(v4);i++){
							OnvifPrefixedIPAddress * addr = OnvifIPv4Configuration__get_manual(v4, i);
							C_INFO("ipv4 : %s/%d",OnvifPrefixedIPAddress__get_address(addr), OnvifPrefixedIPAddress__get_prefix(addr));
						}
					} else {
						C_INFO("No IPv4 Configured");
					}
				}
			break;
		case SOAP_FAULT_ACTION_NOT_SUPPORTED:
		case SOAP_FAULT_CONNECTION_ERROR:
		case SOAP_FAULT_NOT_VALID:
		case SOAP_FAULT_UNAUTHORIZED:
		case SOAP_FAULT_UNEXPECTED:
		default:
			C_ERROR("Failed to retrieve inet %d",*fault);
			break;
	}
	g_object_unref(interfaces);
	
	print_scopes(device_service);

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

	OnvifMediaProfiles * profiles = OnvifMediaService__get_profiles(ONVIF_MEDIA_SERVICE(media_service));
	fault = SoapObject__get_fault(SOAP_OBJECT(profiles));
	switch(*fault){
		case SOAP_FAULT_NONE:
			loop_media_profiles(profiles,media_service, dev);
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

	print_device_capabilites(device_service);
	
	g_object_unref(dev);
	return 0;

}