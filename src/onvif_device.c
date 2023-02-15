#include "onvif_device.h"
#include "client.h"
#include "generated/soapH.h"
#include "generated/DeviceBinding.nsmap"
#include "wsseapi.h"
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <netdb.h>
#endif
#include "wsddapi.h"
#include "httpda.h"

const char * SENDER_UNAUTHORIZED = "Sender not Authorized";
const char * ONVIF_UNAUTHORIZED = "\"http://www.onvif.org/ver10/error\":NotAuthorized";

const int UNAUTHORIZED = 0;
const int AUTHORIZED = 1;
const char * TIME_CONST = "Time";
const char * AUTH_CONST = "Auth";

struct _OnvifCred {
    char * user;
    char * pass;
};


char * OnvifDevice__device_get_username(OnvifDevice *self){
    struct _OnvifCred * cred = (struct _OnvifCred *) self->private;
    return cred->user;
}

char * OnvifDevice__device_get_password(OnvifDevice *self){
    struct _OnvifCred * cred = (struct _OnvifCred *) self->private;
    return cred->pass;
}


int set_wsse_data(OnvifDevice* self, OnvifSoapClient* soap){
    if (soap_wsse_add_Timestamp(soap->soap, TIME_CONST, 10)
        || soap_wsse_add_UsernameTokenDigest(soap->soap, AUTH_CONST, OnvifDevice__device_get_username(self),OnvifDevice__device_get_password(self))){
        //TODO Error handling
        printf("Unable to set wsse creds...\n");
        return 0;
    }

    return 1;
}
OnvifCapabilities* OnvifDevice__device_getCapabilities(OnvifDevice* self) {
    printf("OnvifDevice__device_getCapabilities");
    struct _tds__GetCapabilities gethostname;
    struct _tds__GetCapabilitiesResponse response;
    OnvifCapabilities *capabilities = NULL;

    memset (&gethostname, 0, sizeof (gethostname));
    memset (&response, 0, sizeof (response));
    
    int wsseret = set_wsse_data(self,self->device_soap);
    if(!wsseret){
        //TODO Error handling
        return NULL;
    }

    if (soap_call___tds__GetCapabilities(self->device_soap->soap, self->device_soap->endpoint, NULL, &gethostname,  &response) == SOAP_OK){
        capabilities = (OnvifCapabilities *) malloc(sizeof(OnvifCapabilities));
        OnvifMedia* media =  (OnvifMedia *) malloc(sizeof(OnvifMedia));
        media->xaddr = malloc(strlen(response.Capabilities->Media->XAddr)+1);
        strcpy(media->xaddr,response.Capabilities->Media->XAddr);
        capabilities->media = media;
        self->authorized = AUTHORIZED;
    } else {
        if(strcmp(soap_fault_string(self->device_soap->soap),SENDER_UNAUTHORIZED) == 0){
            self->authorized = UNAUTHORIZED;
            printf("unauthorized 1\n");
        } else if(strcmp(soap_fault_detail(self->device_soap->soap),ONVIF_UNAUTHORIZED) == 0){
            self->authorized = UNAUTHORIZED;
            printf("unauthorized 2\n");
        } else {
            printf("GENERIC ERROR ... \n");
            //TODO Set error...
            soap_print_fault(self->device_soap->soap, stderr);
        }
    }

    soap_destroy(self->device_soap->soap);
    soap_end(self->device_soap->soap);  
    soap_done(self->device_soap->soap); 
    return capabilities;
}

OnvifSoapClient * OnvifDevice__device_getMediaSoap(OnvifDevice* self){
    printf("OnvifDevice__device_getMediaSoap\n");
    if(!self->media_soap){
        OnvifSoapClient__destroy(self->media_soap);
    }
    OnvifSoapClient * client = NULL;
    OnvifCapabilities* capabilities = OnvifDevice__device_getCapabilities(self);
    if(capabilities){
        client = OnvifSoapClient__create(capabilities->media->xaddr,OnvifDevice__device_get_username(self),OnvifDevice__device_get_password(self));
        //Free capabilities except "capabilities->media->xaddr" to pass it to onvifSoapClient
        free(capabilities->media);
        free(capabilities);
    } else {
        printf("No capabilities...\n");
    }

    return client;
}

OnvifDeviceInformation * OnvifDevice__device_getDeviceInformation(OnvifDevice *self){
    printf("OnvifDevice__device_getDeviceInformation \n");
    struct _tds__GetDeviceInformation request;
    struct _tds__GetDeviceInformationResponse response;
    OnvifDeviceInformation *ret = (OnvifDeviceInformation *) malloc(sizeof(OnvifDeviceInformation));

    int wsseret = set_wsse_data(self,self->device_soap);
    if(!wsseret){
        //TODO Error handling
        return NULL;
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

    //FIXME soap cleaup
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

    int wsseret = set_wsse_data(self,self->device_soap);
    if(!wsseret){
        //TODO Error handling
        return NULL;
    }

    if (soap_call___tds__GetHostname(self->device_soap->soap, self->device_soap->endpoint, "", &gethostname,  &response) == SOAP_OK){
        return response.HostnameInformation->Name;
    } else {
        soap_print_fault(self->device_soap->soap, stderr);
    }
    //TODO error handling timout, invalid url, etc...

    //FIXME Soap clean up
    return "error";
}

char * OnvifDevice__media_getStreamUri(OnvifDevice* self, int profile_index){
    printf("OnvifDevice__media_getStreamUri\n");
    struct _trt__GetStreamUri req;
    struct _trt__GetStreamUriResponse resp;
    char * ret = NULL;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    req.StreamSetup = (struct tt__StreamSetup*)soap_malloc(self->media_soap->soap,sizeof(struct tt__StreamSetup));//初始化，分配空间
	req.StreamSetup->Stream = 0;//stream type

	req.StreamSetup->Transport = (struct tt__Transport *)soap_malloc(self->media_soap->soap, sizeof(struct tt__Transport));//初始化，分配空间
	req.StreamSetup->Transport->Protocol = 0;
	req.StreamSetup->Transport->Tunnel = 0;
    
    req.ProfileToken = self->profiles[profile_index].token;

    int wsseret = set_wsse_data(self,self->media_soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    if (soap_call___trt__GetStreamUri(self->media_soap->soap, self->media_soap->endpoint, "", &req, &resp) == SOAP_OK){
        ret = malloc(strlen(resp.MediaUri->Uri)+1);
        strcpy(ret,resp.MediaUri->Uri);
    } else {
        soap_print_fault(self->media_soap->soap, stderr);
    }

exit:
    soap_destroy(self->device_soap->soap);
    soap_end(self->device_soap->soap);  
    soap_done(self->device_soap->soap); 
    return ret;
}

void OnvifDevice_get_profiles(OnvifDevice* self){
    struct _trt__GetProfiles req;
    struct _trt__GetProfilesResponse resp;
    char * ret = NULL;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    int wsseret = set_wsse_data(self,self->media_soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    if (soap_call___trt__GetProfiles(self->media_soap->soap, self->media_soap->endpoint, "", &req, &resp) == SOAP_OK){
        // 遍历响应结构体中的所有配置文件
        if (resp.Profiles != NULL)
        {
            
                int count = resp.__sizeProfiles;
                self->sizeSrofiles = count;
                self->profiles = malloc(sizeof(struct OnvifProfile)*count);
                for(int i = 0; i < count; i++){
                    struct tt__Profile profile = resp.Profiles[i];
                    self->profiles[i].name = malloc(strlen(profile.Name)+1);
                    self->profiles[i].token = malloc(strlen(profile.token)+1);

                    strcpy(self->profiles[i].name,profile.Name);
                    strcpy(self->profiles[i].token,profile.token);
                }
        }  
    } else {
        soap_print_fault(self->media_soap->soap, stderr);
    }

exit:
    soap_destroy(self->device_soap->soap);
    soap_end(self->device_soap->soap);  
    soap_done(self->device_soap->soap); 
}



//TODO Support timeout and invalidafter flag
char * OnvifDevice__media_getSnapshotUri(OnvifDevice *self, int profile_index){
    printf("OnvifDevice__media_getSnapshotUri\n");
    struct _trt__GetSnapshotUri request;
    struct _trt__GetSnapshotUriResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    char * ret_val = NULL;
    int wsseret = set_wsse_data(self,self->media_soap);
    if(!wsseret){
        //TODO Error handling
        return ret_val;
    }

    request.ProfileToken = self->profiles[profile_index].token;

    if (soap_call___trt__GetSnapshotUri(self->media_soap->soap, self->media_soap->endpoint, NULL, &request,  &response) == SOAP_OK){
        ret_val = malloc(strlen(response.MediaUri->Uri) + 1);
        strcpy(ret_val,response.MediaUri->Uri);
    } else {
        soap_print_fault(self->media_soap->soap, stderr);
        //TODO error handling timout, invalid url, etc...
    }

    soap_destroy(self->media_soap->soap); 
    soap_end(self->media_soap->soap); 
    soap_done(self->media_soap->soap); 

    return ret_val;
}

struct chunk * get_http_body(OnvifDevice *self, char * url)
{

    // struct chunk * chunc = malloc(sizeof(struct chunk));
    struct chunk chunc;
    memset (&chunc, 0, sizeof (chunc));
    
    struct soap *soap = NULL;
    struct http_da_info *info = NULL;
    if (!self->snapshot){
        self->snapshot = malloc(sizeof(struct OnvifHttp));
        self->snapshot->soap = soap_new();
        self->snapshot->info = malloc(sizeof(struct http_da_info));
        memset (self->snapshot->info, 0, sizeof(struct http_da_info));
    }

    soap = self->snapshot->soap;
    info = self->snapshot->info;

    soap_register_plugin(soap, http_da);

    if (info->authrealm){
        http_da_restore(soap, info);
    }

    if (soap_GET(soap, url, NULL)
            || soap_begin_recv(soap)
            || (chunc.buffer = soap_http_get_body(soap, &(chunc.size))) != NULL
            || soap_end_recv(soap)){
            if (soap->error == 401){
                http_da_save(soap, info, soap->authrealm, OnvifDevice__device_get_username(self), OnvifDevice__device_get_password(self));
                if (soap_GET(soap, url, NULL)
                    || soap_begin_recv(soap)
                    || (chunc.buffer = soap_http_get_body(soap, &(chunc.size))) != NULL
                    || soap_end_recv(soap)){
                }else {
                    //TODO handle error codes
                    soap_print_fault(soap, stderr);
                }
            }

    } else {
        //TODO handle error codes
        soap_print_fault(soap, stderr);
    }
    soap_closesock(soap);

    //Copy chunk before cleanup
    struct chunk * nchunc = malloc(sizeof(struct chunk));
    nchunc->size = chunc.size;
    nchunc->buffer = malloc(chunc.size);
    memcpy(nchunc->buffer,chunc.buffer,chunc.size);

    return nchunc;
}

struct chunk * OnvifDevice__media_getSnapshot(OnvifDevice *self, int profile_index){
    char * snapshot_uri = OnvifDevice__media_getSnapshotUri(self, profile_index);
    struct chunk * ret = get_http_body(self, snapshot_uri);
    free(snapshot_uri);
    return ret;
}

void OnvifDevice_authenticate(OnvifDevice* self){
    self->media_soap = OnvifDevice__device_getMediaSoap(self);
    if(self->authorized){
        //TODO build the rest
    }
}

void OnvifDevice_set_credentials(OnvifDevice* self, char * user, char* pass){
    struct  _OnvifCred * pcred = (struct _OnvifCred *) self->private;
    pcred->user = realloc(pcred->user,strlen(user) + 1);
    pcred->pass = realloc(pcred->pass,strlen(pass) + 1);
    strcpy(pcred->pass,pass);
    strcpy(pcred->user,user);
}

void OnvifDevice__init(OnvifDevice* self, char * device_url) {
    self->authorized = UNAUTHORIZED;
    struct _OnvifCred * cred = malloc(sizeof(struct _OnvifCred));
    cred->pass = malloc(0);
    cred->user = malloc(0);
    self->private = cred;
    self->device_soap = OnvifSoapClient__create(device_url,OnvifDevice__device_get_username(self),OnvifDevice__device_get_password(self));
    self->media_soap = NULL;
    self->sizeSrofiles = 0;
    self->profiles = NULL;
    self->snapshot = NULL;
    
    char * data = malloc(strlen(device_url)+1);
    memcpy(data,device_url,strlen(device_url)+1);

    self->protocol = strtok (data, "://"); //Protocol http/https
    char *ipport = strtok (NULL, "/"); //Ip:Port
    //Clear strtok?
    while( data != NULL ) {
      data = strtok(NULL, "/");
    }

    self->ip = strtok (ipport, ":"); //IP - TODO Handle url without port to default on http or https.
    self->port = strtok (NULL, "/"); //Port

    //Clear strtok?
    data = strtok(NULL,"/");
    while( data != NULL ) {
      data = strtok(NULL, "/");
    }

    self->hostname = NULL;

    printf("Created Device:\n");
    printf("\tprotocol -- %s\n",self->protocol);
    printf("\tip : %s\n",self->ip);
    printf("\thostname -- %s\n",self->hostname);
    printf("\tport -- %s\n",self->port);
    free(data);
}

OnvifDevice * OnvifDevice__create(char * device_url) {
    printf("OnvifDevice__create\n");
    OnvifDevice * result = malloc(sizeof(OnvifDevice));
    OnvifDevice__init(result,device_url);
    return result;
}

void OnvifCred__destroy(struct _OnvifCred * cred){
    free(cred->pass);
    free(cred->user);
}

void OnvifProfile__destroy(struct OnvifProfile profiles){
    free(profiles.name);
    free(profiles.token);
}

void OnvifHttp__destroy(struct OnvifHttp* http){
    http_da_release(http->soap, http->info);
    soap_destroy(http->soap);
    soap_end(http->soap);  //End clears the buffer. either we copy and run end or we return without end
    soap_done(http->soap); 
    soap_free(http->soap);

}

void OnvifDevice__destroy(OnvifDevice* device) {
    printf("OnvifDevice__destroy\n");
  if (device) {
    OnvifCred__destroy((struct _OnvifCred *)device->private);
    OnvifSoapClient__destroy(device->device_soap);
    OnvifSoapClient__destroy(device->media_soap);
    for (size_t i = 0; i < device->sizeSrofiles; i++)
    {
        OnvifProfile__destroy(device->profiles[i]);
    }
    OnvifHttp__destroy(device->snapshot);
    free(device->profiles);
    free(device);
  }
}