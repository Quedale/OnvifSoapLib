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
#include "ip_match.h"


#define FAULT_UNAUTHORIZED "[\"http://www.onvif.org/ver10/error\":NotAuthorized]"

const char * TIME_CONST = "Time";
const char * AUTH_CONST = "Auth";

struct _OnvifCred {
    char * user;
    char * pass;
};


char * OnvifDevice__device_get_username(OnvifDevice *self){
    struct _OnvifCred * cred = (struct _OnvifCred *) self->priv_ptr;
    return cred->user;
}

char * OnvifDevice__device_get_password(OnvifDevice *self){
    struct _OnvifCred * cred = (struct _OnvifCred *) self->priv_ptr;
    return cred->pass;
}


int set_wsse_data(OnvifDevice* self, OnvifSoapClient* soap){
    if (soap_wsse_add_Timestamp(soap->soap, TIME_CONST, 10)
        || soap_wsse_add_UsernameTokenDigest(soap->soap, AUTH_CONST, OnvifDevice__device_get_username(self),OnvifDevice__device_get_password(self))){
        //TODO Error handling
        printf("Unable to set wsse creds...\n");
        self->last_error = ONVIF_CONNECTION_ERROR;
        return 0;
    }

    self->last_error = ONVIF_ERROR_NONE;
    return 1;
}

OnvifErrorTypes handle_soap_error(struct soap * soap, int error_code){
    if (error_code == SOAP_UDP_ERROR || 
        error_code == SOAP_TCP_ERROR || 
        error_code == SOAP_HTTP_ERROR || 
        error_code == SOAP_SSL_ERROR || 
        error_code == SOAP_ZLIB_ERROR){
        printf("CONNECTION ERROR\n");
        return ONVIF_CONNECTION_ERROR;
    } else if (strcmp(soap_fault_subcode(soap),FAULT_UNAUTHORIZED)) {
        printf("Warning : NOT AUTHORIZED\n");
        return ONVIF_NOT_AUTHORIZED;
    } else { //Mostly SOAP_FAULT
        printf("SOAP ERROR %i [%s]\n",error_code, soap_fault_subcode(soap));
        soap_print_fault(soap, stderr);
        return ONVIF_SOAP_ERROR;
    }
}

OnvifCapabilities* OnvifDevice__device_getCapabilities(OnvifDevice* self) {
    struct _tds__GetCapabilities gethostname;
    struct _tds__GetCapabilitiesResponse response;
    OnvifCapabilities *capabilities = NULL;

    memset (&gethostname, 0, sizeof (gethostname));
    memset (&response, 0, sizeof (response));

    pthread_mutex_unlock(self->device_soap->lock);
    printf("OnvifDevice__device_getCapabilities [%s]\n", self->device_soap->endpoint);
    int wsseret = set_wsse_data(self,self->device_soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___tds__GetCapabilities(self->device_soap->soap, self->device_soap->endpoint, NULL, &gethostname,  &response);
    if (soapret == SOAP_OK){
        capabilities = (OnvifCapabilities *) malloc(sizeof(OnvifCapabilities));
        OnvifMedia* media =  (OnvifMedia *) malloc(sizeof(OnvifMedia));
        media->xaddr = malloc(strlen(response.Capabilities->Media->XAddr)+1);
        strcpy(media->xaddr,response.Capabilities->Media->XAddr);
        capabilities->media = media;
    } else {
        self->last_error = handle_soap_error(self->device_soap->soap,soapret);    
    }

exit:
    soap_destroy(self->device_soap->soap);
    soap_end(self->device_soap->soap);  
    pthread_mutex_unlock(self->device_soap->lock);
    return capabilities;
}

void OnvifDevice__device_createMediaSoap(OnvifDevice* self){
    //Check before lock to avoid locking/waiting uselessly
    if(OnvifSoapClient__is_valid(self->media_soap)){
        return;
    }

    pthread_mutex_lock(self->media_soap->lock);

    //Check again in case it was created during lock
    if(OnvifSoapClient__is_valid(self->media_soap)){
        goto exit;
    }

    printf("OnvifDevice__device_createMediaSoap\n");
    OnvifCapabilities* capabilities = OnvifDevice__device_getCapabilities(self);
    if(capabilities){
        OnvifSoapClient__set_endpoint(self->media_soap,capabilities->media->xaddr);
        //Free capabilities except "capabilities->media->xaddr" to pass it to onvifSoapClient
        free(capabilities->media);
        free(capabilities);
    } else {
        printf("No capabilities...\n");
    }

exit:
    pthread_mutex_unlock(self->media_soap->lock);
}

void OnvifDeviceInformation__destroy(OnvifDeviceInformation *self){
    if(self){
        free(self->firmwareVersion);
        free(self->hardwareId);
        free(self->manufacturer);
        free(self->model);
        free(self->serialNumber);
        free(self);
    }
}

OnvifDeviceInformation * OnvifDevice__device_getDeviceInformation(OnvifDevice *self){
    struct _tds__GetDeviceInformation request;
    struct _tds__GetDeviceInformationResponse response;
    OnvifDeviceInformation *ret = NULL;

    pthread_mutex_lock(self->device_soap->lock);
    printf("OnvifDevice__device_getDeviceInformation [%s]\n", self->device_soap->endpoint);
    int wsseret = set_wsse_data(self,self->device_soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___tds__GetDeviceInformation(self->device_soap->soap, self->device_soap->endpoint, "", &request,  &response);
    if (soapret == SOAP_OK){
        ret = (OnvifDeviceInformation *) malloc(sizeof(OnvifDeviceInformation));

        ret->firmwareVersion = (char*) malloc(strlen(response.FirmwareVersion)+1);
        strcpy(ret->firmwareVersion,response.FirmwareVersion);

        ret->hardwareId = (char*) malloc(strlen(response.HardwareId)+1);
        strcpy(ret->hardwareId,response.HardwareId);

        ret->manufacturer = (char*) malloc(strlen(response.Manufacturer)+1);
        strcpy(ret->manufacturer,response.Manufacturer);

        ret->model = (char*) malloc(strlen(response.Model)+1);
        strcpy(ret->model,response.Model);

        ret->serialNumber = (char*) malloc(strlen(response.SerialNumber)+1);
        strcpy(ret->serialNumber,response.SerialNumber);
    } else {
        self->last_error = handle_soap_error(self->device_soap->soap,soapret);    
    }

exit:
    soap_destroy(self->device_soap->soap);
    soap_end(self->device_soap->soap);  
    pthread_mutex_unlock(self->device_soap->lock);
    return ret;
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

void urldecode(char *dst, const char *src)
{
        char a, b;
        while (*src) {
                if ((*src == '%') &&
                    ((a = src[1]) && (b = src[2])) &&
                    (isxdigit(a) && isxdigit(b))) {
                        if (a >= 'a')
                                a -= 'a'-'A';
                        if (a >= 'A')
                                a -= ('A' - 10);
                        else
                                a -= '0';
                        if (b >= 'a')
                                b -= 'a'-'A';
                        if (b >= 'A')
                                b -= ('A' - 10);
                        else
                                b -= '0';
                        *dst++ = 16*a+b;
                        src+=3;
                } else if (*src == '+') {
                        *dst++ = ' ';
                        src++;
                } else {
                        *dst++ = *src++;
                }
        }
        *dst++ = '\0';
}

int startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}

void OnvifScopes__destroy(OnvifScopes * scopes){
    if(scopes){
        int i;
        for(i=0;i<scopes->count;i++){
            free(scopes->scopes[i]->scope);
            free(scopes->scopes[i]);
        }
        free(scopes->scopes);
        free(scopes);
    }
}

char * OnvifScopes__extract_scope(OnvifScopes * scopes, char * key){
    printf("OnvifScopes__extract_scope %s\n", key);
    char* ret_val = "";
    const char delimeter[2] = "/";
    const char * onvif_key_del = "onvif://www.onvif.org/";

    char key_w_del[strlen(key)+1+strlen(delimeter)];
    strcpy(key_w_del, key);
    strcat(key_w_del, delimeter);

    int a;
    for (a = 0 ; a < scopes->count ; ++a) {
        if(startsWith(onvif_key_del, scopes->scopes[a]->scope)){
            //Drop onvif scope prefix
            char subs[strlen(scopes->scopes[a]->scope)-strlen(onvif_key_del) + 1];
            strncpy(subs,scopes->scopes[a]->scope+(strlen(onvif_key_del)),strlen(scopes->scopes[a]->scope) - strlen(onvif_key_del)+1);

            if(startsWith(key_w_del,subs)){ // Found Scope
                //Extract value
                char sval[strlen(subs)-strlen(key_w_del) + 1];
                strncpy(sval,subs+(strlen(key_w_del)),strlen(subs) - strlen(key_w_del)+1);

                //Decode http string (e.g. %20 = whitespace)
                char output[strlen(sval)+1];
                urldecode(output, sval);

                if(strlen(ret_val)==0){
                    ret_val = malloc(strlen(output)+1);
                    memcpy(ret_val,output,strlen(output)+1);
                } else {
                    ret_val = realloc(ret_val, strlen(ret_val) + strlen(output) +1);
                    strcat(ret_val, " ");
                    strcat(ret_val, output);
                }
            }
        }
  }

  return ret_val;
}

void OnvifInterfaces__destroy(OnvifInterfaces * self){
    if(self){
        for(int i=0;i<self->count;i++){
            free(self->interfaces[i]->token);
            if(self->interfaces[i]->name)
                free(self->interfaces[i]->name);
            if(self->interfaces[i]->mac)
                free(self->interfaces[i]->mac);
            if(self->interfaces[i]->ipv4_manual){
                for(int a=0;a<self->interfaces[i]->ipv4_manual_count;a++){
                    free(self->interfaces[i]->ipv4_manual[a]);
                }
                free(self->interfaces[i]->ipv4_manual);
            }
            if(self->interfaces[i]->ipv4_link_local)
                free(self->interfaces[i]->ipv4_link_local);
            if(self->interfaces[i]->ipv4_from_dhcp)
                free(self->interfaces[i]->ipv4_from_dhcp);
            
            free(self->interfaces[i]);
        }

        free(self);
    }
}

OnvifInterfaces * OnvifDevice__device_getNetworkInterfaces(OnvifDevice* self) {
    struct _tds__GetNetworkInterfaces req;
    struct _tds__GetNetworkInterfacesResponse resp;
    OnvifInterfaces * interfaces = NULL;

    pthread_mutex_lock(self->device_soap->lock);

    printf("OnvifDevice__device_getNetworkInterfaces [%s]\n", self->device_soap->endpoint);
    int wsseret = set_wsse_data(self,self->device_soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___tds__GetNetworkInterfaces(self->device_soap->soap, self->device_soap->endpoint, "", &req,  &resp);
    if (soapret == SOAP_OK){
        interfaces = malloc(sizeof(OnvifInterfaces));
        interfaces->interfaces = malloc(0);
        interfaces->count = 0;

        int i;
        for(i=0;i<resp.__sizeNetworkInterfaces;i++){
            struct tt__NetworkInterface interface = resp.NetworkInterfaces[i];
            OnvifInterface * onvifinterface = malloc(sizeof(OnvifInterface));
            onvifinterface->enabled = interface.Enabled;
            onvifinterface->token = (char*) malloc(strlen(interface.token)+1);
            strcpy(onvifinterface->token,interface.token);
            
            if(interface.Info){
                onvifinterface->has_info = 1;
                onvifinterface->name = (char*) malloc(strlen(interface.Info->Name)+1);
                strcpy(onvifinterface->name,interface.Info->Name);
                onvifinterface->mac = (char*) malloc(strlen(interface.Info->HwAddress)+1);
                strcpy(onvifinterface->mac,interface.Info->HwAddress);

                onvifinterface->mtu = interface.Info->MTU[0];
            } else {
                onvifinterface->has_info = 0;
            }
            
            //struct tt__IPv4NetworkInterface*     IPv4
            if(interface.IPv4){
                onvifinterface->ipv4_enabled = interface.IPv4->Enabled;
                onvifinterface->ipv4_dhcp = interface.IPv4->Config->DHCP;
                onvifinterface->ipv4_manual_count = interface.IPv4->Config->__sizeManual;
                onvifinterface->ipv4_manual = NULL;
                onvifinterface->ipv4_link_local = NULL;
                onvifinterface->ipv4_from_dhcp = NULL;

                //Manually configured IPs
                if(interface.IPv4->Config->__sizeManual > 0){
                    struct tt__PrefixedIPv4Address * manuals = interface.IPv4->Config->Manual;
                    for(int a=0;a<onvifinterface->ipv4_manual_count;a++){
                        struct tt__PrefixedIPv4Address manual = manuals[a];
                        onvifinterface->ipv4_manual = realloc(onvifinterface->ipv4_manual,sizeof(char *) * onvifinterface->ipv4_manual_count);
                        onvifinterface->ipv4_manual[onvifinterface->ipv4_manual_count-1] = malloc(strlen(manual.Address) + 1);
                        strcpy(onvifinterface->ipv4_manual[onvifinterface->ipv4_manual_count-1],manual.Address);
                    }
                }

                //Link local address.
                if(interface.IPv4->Config->LinkLocal){
                    onvifinterface->ipv4_link_local = malloc(strlen(interface.IPv4->Config->LinkLocal->Address)+1);
                    strcpy(onvifinterface->ipv4_link_local,interface.IPv4->Config->LinkLocal->Address);
                }

                //DHCP IP
                if(interface.IPv4->Config->FromDHCP){
                    onvifinterface->ipv4_from_dhcp = malloc(strlen(interface.IPv4->Config->FromDHCP->Address)+1);
                    strcpy(onvifinterface->ipv4_from_dhcp,interface.IPv4->Config->FromDHCP->Address);
                }
            } else {
                onvifinterface->ipv4_enabled = 0;
            }

            //TODO IPv6

            interfaces->count++;
            interfaces->interfaces = realloc (interfaces->interfaces, sizeof (onvifinterface) * interfaces->count);
            interfaces->interfaces[interfaces->count-1] = onvifinterface;
        }

    } else {
        self->last_error = handle_soap_error(self->device_soap->soap,soapret);    
    }

exit:
    soap_destroy(self->device_soap->soap);
    soap_end(self->device_soap->soap);  
    pthread_mutex_unlock(self->device_soap->lock);
    return interfaces;
}

OnvifScopes * OnvifDevice__device_getScopes(OnvifDevice* self) {
    struct _tds__GetScopes req;
    struct _tds__GetScopesResponse resp;
    OnvifScopes * scopes = NULL;

    pthread_mutex_lock(self->device_soap->lock);
    printf("OnvifDevice__device_getScopes [%s]\n", self->device_soap->endpoint);
    int wsseret = set_wsse_data(self,self->device_soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___tds__GetScopes(self->device_soap->soap, self->device_soap->endpoint, "", &req,  &resp);
    if (soapret == SOAP_OK){
        scopes = malloc(sizeof(OnvifScopes));
        scopes->scopes = malloc(0);
        scopes->count = 0;

        int i;
        for(i=0;i<resp.__sizeScopes;i++){
            struct tt__Scope scope = resp.Scopes[i];
            // tt__ScopeDefinition scopedef = scope.ScopeDef;
            xsd__anyURI scopeitem = scope.ScopeItem;

            OnvifScope * onvifscope = malloc(sizeof(OnvifScope));
            onvifscope->scope = (char*) malloc(strlen(scopeitem)+1);
            strcpy(onvifscope->scope,scopeitem);

            scopes->count++;
            scopes->scopes = realloc (scopes->scopes, sizeof (OnvifScope) * scopes->count);
            scopes->scopes[scopes->count-1] = onvifscope;

            // tt__ScopeDefinition__Fixed : tt__ScopeDefinition__Fixed || tt__ScopeDefinition__Configurable
        }
    } else {
        self->last_error = handle_soap_error(self->device_soap->soap,soapret);    
    }

exit:
    soap_destroy(self->device_soap->soap);
    soap_end(self->device_soap->soap);  
    pthread_mutex_unlock(self->device_soap->lock);
    return scopes;
}

char * OnvifDevice__device_getHostname(OnvifDevice* self) {
    struct _tds__GetHostname gethostname;
    struct _tds__GetHostnameResponse response;
    char * ret = NULL;

    pthread_mutex_lock(self->device_soap->lock);
    printf("OnvifDevice__device_getHostname [%s]\n", self->device_soap->endpoint);
    int wsseret = set_wsse_data(self,self->device_soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___tds__GetHostname(self->device_soap->soap, self->device_soap->endpoint, "", &gethostname,  &response);
    if (soapret == SOAP_OK){
        ret = malloc(strlen(response.HostnameInformation->Name)+1); 
        strcpy(ret,response.HostnameInformation->Name);
    } else {
        self->last_error = handle_soap_error(self->device_soap->soap,soapret);    
    }
    //TODO error handling timout, invalid url, etc...

exit:
    soap_destroy(self->device_soap->soap);
    soap_end(self->device_soap->soap);  
    pthread_mutex_unlock(self->device_soap->lock);
    return ret;
}

char * OnvifDevice__media_getStreamUri(OnvifDevice* self, int profile_index){
    struct _trt__GetStreamUri req;
    struct _trt__GetStreamUriResponse resp;
    char * ret = NULL;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    if(!OnvifSoapClient__is_valid(self->media_soap)){
        printf("ERROR media soap not created. Authenticate first.\n");
        self->last_error = ONVIF_SOAP_ERROR;
        return NULL;
    }

    if(!self->profiles){
        OnvifDevice_get_profiles(self);
    }
    
    if(profile_index >= self->sizeSrofiles){
        printf("OnvifDevice__media_getStreamUri [%s]: profile index out-of-bounds.\n", self->media_soap->endpoint);
        // goto exit
    } else if(self->sizeSrofiles > 0) {
        req.ProfileToken = self->profiles[profile_index].token;
    }

    pthread_mutex_lock(self->media_soap->lock);
    printf("OnvifDevice__media_getStreamUri [%s][%i]\n", self->media_soap->endpoint,profile_index);
    // req.StreamSetup = (struct tt__StreamSetup*)soap_malloc(self->media_soap->soap,sizeof(struct tt__StreamSetup));//Initialize, allocate space
	req.StreamSetup = soap_new_tt__StreamSetup(self->media_soap->soap,1);
    req.StreamSetup->Stream = tt__StreamType__RTP_Unicast;//stream type

	// req.StreamSetup->Transport = (struct tt__Transport *)soap_malloc(self->media_soap->soap, sizeof(struct tt__Transport));//Initialize, allocate space
	req.StreamSetup->Transport = soap_new_tt__Transport(self->media_soap->soap,1);
    req.StreamSetup->Transport->Protocol = tt__TransportProtocol__UDP;
	req.StreamSetup->Transport->Tunnel = 0;

    int wsseret = set_wsse_data(self,self->media_soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___trt__GetStreamUri(self->media_soap->soap, self->media_soap->endpoint, "", &req, &resp);
    if (soapret == SOAP_OK){
        ret = malloc(strlen(resp.MediaUri->Uri)+1);
        strcpy(ret,resp.MediaUri->Uri);
    } else {
        self->last_error = handle_soap_error(self->device_soap->soap,soapret);    
    }

exit:
    soap_destroy(self->media_soap->soap);
    soap_end(self->media_soap->soap);  
    pthread_mutex_unlock(self->media_soap->lock);
    return ret;
}

void OnvifDevice_get_profiles(OnvifDevice* self){
    struct _trt__GetProfiles req;
    struct _trt__GetProfilesResponse resp;

    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    if(!OnvifSoapClient__is_valid(self->media_soap)){
        printf("ERROR media soap not created. Authenticate first.\n");
        self->last_error = ONVIF_SOAP_ERROR;
        return;
    }
    
    pthread_mutex_lock(self->media_soap->lock);
    printf("OnvifDevice_get_profiles [%s]\n", self->media_soap->endpoint);
    int wsseret = set_wsse_data(self,self->media_soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___trt__GetProfiles(self->media_soap->soap, self->media_soap->endpoint, "", &req, &resp);
    if (soapret == SOAP_OK){
        // Traverse all configuration files in the response structure
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
        self->last_error = handle_soap_error(self->device_soap->soap,soapret);    
    }

exit:
    soap_destroy(self->media_soap->soap);
    soap_end(self->media_soap->soap);  
    pthread_mutex_unlock(self->media_soap->lock);
}



//TODO Support timeout and invalidafter flag
char * OnvifDevice__media_getSnapshotUri(OnvifDevice *self, int profile_index){
    struct _trt__GetSnapshotUri request;
    struct _trt__GetSnapshotUriResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));
    char * ret_val = NULL;

    if(!self->profiles){
        OnvifDevice_get_profiles(self);
    }

    if(profile_index >= self->sizeSrofiles){
        printf("OnvifDevice__media_getSnapshotUri : profile index out-of-bounds. [%s]\n", self->media_soap->endpoint);
    } else if(self->sizeSrofiles > 0) {
        request.ProfileToken = self->profiles[profile_index].token;
    }

    pthread_mutex_lock(self->media_soap->lock);
    printf("OnvifDevice__media_getSnapshotUri [%s][%i]\n", self->media_soap->endpoint,profile_index);
    int wsseret = set_wsse_data(self,self->media_soap);
    if(!wsseret){
        //TODO Error handling
        printf("OnvifDevice__media_getSnapshotUri - Failed to set credentials\n");
        goto exit;
    }

    int soapret = soap_call___trt__GetSnapshotUri(self->media_soap->soap, self->media_soap->endpoint, NULL, &request,  &response);
    if (soapret == SOAP_OK){
        ret_val = malloc(strlen(response.MediaUri->Uri) + 1);
        strcpy(ret_val,response.MediaUri->Uri);
    } else {
        self->last_error = handle_soap_error(self->device_soap->soap,soapret);    
    }

exit:
    soap_destroy(self->media_soap->soap); 
    soap_end(self->media_soap->soap); 
    pthread_mutex_unlock(self->media_soap->lock);
    return ret_val;
}

struct chunk * get_http_body(OnvifDevice *self, char * url)
{
    struct chunk chunc;
    struct chunk * nchunc = NULL;
    memset (&chunc, 0, sizeof (chunc));
    
    //Creating soap instance to avoid long running locks.
    struct soap *soap = soap_new();
    if (!self->snapshot_da_info){
        self->snapshot_da_info = malloc(sizeof(struct http_da_info));
        memset (self->snapshot_da_info, 0, sizeof(struct http_da_info));
    }

    soap_register_plugin(soap, http_da);

    if (self->snapshot_da_info->authrealm){
        http_da_restore(soap, self->snapshot_da_info);
    }

    if (soap_GET(soap, url, NULL)
            || soap_begin_recv(soap)
            || (chunc.buffer = soap_http_get_body(soap, &(chunc.size))) != NULL
            || soap_end_recv(soap)){
            if (soap->error == 401){
                http_da_save(soap, self->snapshot_da_info, soap->authrealm, OnvifDevice__device_get_username(self), OnvifDevice__device_get_password(self));
                if (soap_GET(soap, url, NULL)
                    || soap_begin_recv(soap)
                    || (chunc.buffer = soap_http_get_body(soap, &(chunc.size))) != NULL
                    || soap_end_recv(soap)){
                }else {
                    //TODO handle error codes
                    printf("get_http_body ERROR-1\n");
                    soap_print_fault(soap, stderr);
                    goto exit;
                }
            }

    } else {
        //TODO handle error codes
        printf("get_http_body ERROR-2\n");
        soap_print_fault(soap, stderr);
        goto exit;
    }

    //Copy chunk before cleanup
    nchunc = malloc(sizeof(struct chunk));
    nchunc->size = chunc.size;
    nchunc->buffer = malloc(chunc.size);
    memcpy(nchunc->buffer,chunc.buffer,chunc.size);

exit:
    http_da_release(soap, self->snapshot_da_info);
    soap_closesock(soap);
    soap_destroy(soap); 
    soap_end(soap); 
    soap_done(soap);
    soap_free(soap); 
    
    return nchunc;
}

struct chunk * OnvifDevice__media_getSnapshot(OnvifDevice *self, int profile_index){
    char * snapshot_uri = OnvifDevice__media_getSnapshotUri(self, profile_index);
    struct chunk * ret = get_http_body(self, snapshot_uri);
    free(snapshot_uri);
    return ret;
}

void OnvifDevice_authenticate(OnvifDevice* self){
    printf("OnvifDevice_authenticate [%s]\n", self->device_soap->endpoint);
    OnvifDevice__device_createMediaSoap(self);
    if(!OnvifSoapClient__is_valid(self->media_soap)){
        return;
    }

    printf("Created Media soap [%s]\n",self->media_soap->endpoint);
    char * stream_uri = OnvifDevice__media_getStreamUri(self,0);
    if(!stream_uri){
        return;
    }
    printf("StreamURI : %s\n",stream_uri);
}

void OnvifDevice_set_credentials(OnvifDevice* self,const char * user,const char* pass){
    struct  _OnvifCred * pcred = (struct _OnvifCred *) self->priv_ptr;
    pcred->user = realloc(pcred->user,strlen(user) + 1);
    pcred->pass = realloc(pcred->pass,strlen(pass) + 1);
    strcpy(pcred->pass,pass);
    strcpy(pcred->user,user);
}

int OnvifDevice__is_valid(OnvifDevice* self){
    if(!self->protocol){
        return 0;
    }
    if(!self->hostname && !self->ip){
        return 0;
    }
    //According to Core specification, the device entry point it set. (5.1.1)
    if(!self->endpoint || strcmp(self->endpoint,"onvif/device_service")){
        return 0;
    }

    //TODO Test endpoint
    return 1;
}

void OnvifDevice__init(OnvifDevice* self, const char * device_url) {
    self->last_error = ONVIF_NOT_SET;
    struct _OnvifCred * cred = malloc(sizeof(struct _OnvifCred));
    cred->pass = malloc(0);
    cred->user = malloc(0);
    self->priv_ptr = cred;
    self->device_soap = OnvifSoapClient__create();
    OnvifSoapClient__set_endpoint(self->device_soap,(char *)device_url);
    self->media_soap = OnvifSoapClient__create();
    self->sizeSrofiles = 0;
    self->profiles = NULL;
    self->snapshot_da_info = NULL;

    char * data = malloc(strlen(device_url)+1);
    memcpy(data,device_url,strlen(device_url)+1);

    if(strstr(data,"://")){
        self->protocol = strtok_r ((char *)data, "://", &data);
    } else {
        self->protocol = NULL;
    }

    self->port = NULL;

    char * hostorip = NULL;
    char *hostport = strtok_r ((char *)data, "/", &data);
    self->endpoint = strtok_r ((char *)data, "\n", &data);

    //TODO Support IPv6
    if(hostport){
        if(strstr(hostport,":")){ //If the port is set
            hostorip = strtok_r (hostport, ":", &hostport);
            self->port = strtok_r ((char *)hostport, "/", &hostport);
        } else { //If no port is set
            hostorip = strtok_r (hostport, "/", &hostport);
        }
    } else {
        hostorip = NULL;
    }

    if(is_valid_ip(hostorip)){
        self->ip = hostorip;
        self->hostname = NULL;
    } else {
        self->ip = NULL;
        self->hostname = hostorip;
    }

    if(!self->port && self->protocol && !strcmp(self->protocol,"http")){
        self->port = "80";
    } else if(!self->port && self->protocol && !strcmp(self->protocol,"https")){
        self->port = "443";
    }

    printf("Created Device:\n");
    printf("\tprotocol -- %s\n",self->protocol);
    printf("\tip : %s\n",self->ip);
    printf("\thostname : %s\n",self->hostname);
    printf("\tport -- %s\n",self->port);
    printf("\tendpoint : %s\n",self->endpoint);
}

OnvifDevice * OnvifDevice__create(const char * device_url) {
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

void OnvifDevice__destroy(OnvifDevice* device) {
    printf("OnvifDevice__destroy\n");
    if (device) {
        OnvifCred__destroy((struct _OnvifCred *)device->priv_ptr);
        OnvifSoapClient__destroy(device->device_soap);
        OnvifSoapClient__destroy(device->media_soap);
        for (size_t i = 0; i < device->sizeSrofiles; i++)
        {
            OnvifProfile__destroy(device->profiles[i]);
        }
        if(device->snapshot_da_info)
            free(device->snapshot_da_info);
        free(device->profiles);
        free(device);
    }
}