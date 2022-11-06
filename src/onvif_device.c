#include "onvif_device.h"
#include "client.h"
#include "generated/soapH.h"
#include "generated/DeviceBinding.nsmap"
#include "wsseapi.h"
#include <netdb.h>
#include "wsddapi.h"

struct _OnvifCred {
    char * user;
    char * pass;
};

OnvifCapabilities* OnvifDevice__device_getCapabilities(OnvifDevice* self) {
    struct _tds__GetCapabilities gethostname;
    struct _tds__GetCapabilitiesResponse response;

    memset (&gethostname, 0, sizeof (gethostname));
    memset (&response, 0, sizeof (response));
    OnvifCapabilities* capabilities;
    

    if (soap_wsse_add_Timestamp(self->device_soap->soap, "Time", 10)
        || soap_wsse_add_UsernameTokenDigest(self->device_soap->soap, "Auth", "admin", "admin2")){
        printf("Unable to set wsse creds...\n");
        //TODO Error handling
        return "Error";
    }

    if (soap_call___tds__GetCapabilities(self->device_soap->soap, self->device_soap->endpoint, "", &gethostname,  &response) == SOAP_OK){
        OnvifMedia* media =  (OnvifMedia *) malloc(sizeof(OnvifMedia));
        media->xaddr = response.Capabilities->Media->XAddr;
        capabilities->media = media;
        self->authorized = 1;

        return capabilities; 
    } else {
        if(soap_fault_string(self->device_soap->soap) == "Sender not Authorized"){
            self->authorized = 0;
        } else if(soap_fault_detail(self->device_soap->soap) == "\"http://www.onvif.org/ver10/error\":NotAuthorized"){
            self->authorized = 0;
        } else {
            //TODO Set error...
            soap_print_fault(self->device_soap->soap, stderr);
        }
    }

    return NULL;
}

OnvifSoapClient * OnvifDevice__device_getMediaSoap(OnvifDevice* self){
    printf("OnvifDevice__device_getMediaSoap \n");
    OnvifCapabilities* capabilities = OnvifDevice__device_getCapabilities(self);
    if(capabilities){
        return OnvifSoapClient__create(capabilities->media->xaddr);
    }
}

OnvifDeviceInformation * OnvifDevice__device_getDeviceInformation(OnvifDevice *self){
    printf("OnvifDevice__device_getDeviceInformation \n");
    struct _tds__GetDeviceInformation request;
    struct _tds__GetDeviceInformationResponse response;
    OnvifDeviceInformation *ret = (OnvifDeviceInformation *) malloc(sizeof(OnvifDeviceInformation));

    if (soap_wsse_add_Timestamp(self->device_soap->soap, "Time", 10)
        || soap_wsse_add_UsernameTokenDigest(self->device_soap->soap, "Auth", "admin", "admin")){
        printf("Unable to set wsse creds...\n");
        //TODO Error handling
        return "Error";
    }

    if (soap_call___tds__GetDeviceInformation(self->device_soap->soap, self->device_soap->endpoint, "", &request,  &response) == SOAP_OK){
        ret->firmwareVersion = response.FirmwareVersion;
        ret->hardwareId = response.HardwareId;
        ret->manufacturer = response.Manufacturer;
        ret->model = response.Model;
        ret->serialNumber = response.SerialNumber;
        return ret;
    } else {
        soap_print_fault(self->device_soap->soap, stderr);
        //TODO error handling timout, invalid url, etc...
        return NULL;
    }
}

// Place holder to allow client compile
void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{ printf("wsdd_event_Hello\n"); }

void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{ printf("wsdd_event_Bye\n"); }

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *ProbeMatches)
{
    printf("wsdd_event_Probe\n");
    return SOAP_WSDD_ADHOC;
}

void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *ProbeMatches)
{  printf("wsdd_event_ProbeMatches\n"); }

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
    printf("wsdd_event_Resolve\n");
    return SOAP_WSDD_ADHOC;
}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char * SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{ printf("wsdd_event_ResolveMatches\n"); }

char * OnvifDevice__device_getHostname(OnvifDevice* self) {
    struct _tds__GetHostname gethostname;
    struct _tds__GetHostnameResponse response;

    if (soap_wsse_add_Timestamp(self->device_soap->soap, "Time", 10)
        || soap_wsse_add_UsernameTokenDigest(self->device_soap->soap, "Auth", "admin", "admin")){
        printf("Unable to set wsse creds...\n");
        //TODO Error handling
        return "Error";
    }

    if (soap_call___tds__GetHostname(self->device_soap->soap, self->device_soap->endpoint, "", &gethostname,  &response) == SOAP_OK){
        return response.HostnameInformation->Name;
    } else {
        soap_print_fault(self->device_soap->soap, stderr);
    }
    //TODO error handling timout, invalid url, etc...
    return "error";
}

char * OnvifDevice__media_getStreamUri(OnvifDevice* self){
    struct _trt__GetStreamUri req;
    struct _trt__GetStreamUriResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    if (soap_wsse_add_Timestamp(self->media_soap->soap, "Time", 10)
        || soap_wsse_add_UsernameTokenDigest(self->media_soap->soap, "Auth", "admin", "admin")){
        printf("Unable to set wsse creds...\n");
        //TODO Error handling
        return "Error";
    }

    if (soap_call___trt__GetStreamUri(self->media_soap->soap, self->media_soap->endpoint, "", &req, &resp) == SOAP_OK){
        return resp.MediaUri->Uri;
    } else {
        soap_print_fault(self->media_soap->soap, stderr);
    }
    //TODO error handling timout, invalid url, etc...
    return "error";
}

void OnvifDevice__init(OnvifDevice* self, char * device_url) {
    self->authorized = 0;
    self->device_soap = OnvifSoapClient__create(device_url);
    self->media_soap = OnvifDevice__device_getMediaSoap(self);
    
    printf("copy device_url : %s\n",device_url);
    char * data = malloc(strlen(device_url)+1);
    memcpy(data,device_url,strlen(device_url)+1);

    printf("getting protocol ...\n");
    self->protocol = strtok (data, "://"); //Protocol http/https
    printf("getting IP:Port ...\n");
    char *ipport = strtok (NULL, "/"); //Ip:Port
    //Clear strtok?
    printf("Clearing tok ...\n");
    while( data != NULL ) {
      data = strtok(NULL, "/");
    }

    printf("getting ip ...\n");
    self->ip = strtok (ipport, ":"); //IP - TODO Handle url without port to default on http or https.
    printf("getting port ...\n");
    self->port = strtok (NULL, "/"); //Port

    //Clear strtok?
    printf("Clear tok ...\n");
    data = strtok(NULL,"/");
    while( data != NULL ) {
      data = strtok(NULL, "/");
    }

    //TODO Lookup name outside GUI Thread
    // printf("NSLookup ...\n");
    // //Lookup hostname
    // struct in_addr in_a;
    // inet_pton(AF_INET, self->ip, &in_a);
    // struct hostent* host;
    // host = gethostbyaddr( (const void*)&in_a, 
    //                     sizeof(struct in_addr), 
    //                     AF_INET );
    // if(host){
    //     printf("Found hostname : %s\n",host->h_name);
    //     self->hostname = host->h_name;
    // } else {
    //     printf("Failed to get hostname ...\n");
    //     self->hostname = NULL;
    // }
    self->hostname = NULL;

    printf("protocol -- %s\n",self->protocol);
    printf("ip : %s\n",self->ip);
    printf("hostname -- %s\n",self->hostname);
    printf("port -- %s\n",self->port);
    free(data);
}

OnvifDevice OnvifDevice__create(char * device_url) {
    OnvifDevice result;
    printf("Creating dev url1 : %s\n",device_url);
    memset (&result, 0, sizeof (result));
    OnvifDevice__init(&result,device_url);
    return result;
}


void OnvifDevice__reset(OnvifDevice* self) {
}

void OnvifDevice__destroy(OnvifDevice* device) {
  if (device) {
     OnvifDevice__reset(device);
     free(device);
  }
}