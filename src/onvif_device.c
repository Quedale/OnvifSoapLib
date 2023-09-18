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
#include "onvif_device_profile_local.h"
#include "onvif_device_info_local.h"
#include "onvif_device_interface_local.h"

#define FAULT_UNAUTHORIZED "[\"http://www.onvif.org/ver10/error\":NotAuthorized]"

const char * TIME_CONST = "Time";
const char * AUTH_CONST = "Auth";

typedef struct _OnvifScope {
    char * scope;
    int editable;
} OnvifScope;

typedef struct _OnvifScopes {
    int count;
    OnvifScope ** scopes;
} OnvifScopes;

typedef struct _OnvifSnapshot {
    char * buffer;
    size_t size;
} OnvifSnapshot;

typedef struct _OnvifMedia {
    char *xaddr;
    //TODO StreamingCapabilities
    //TODO Extension
} OnvifMedia;

typedef struct _OnvifCapabilities {
    OnvifMedia *media;
} OnvifCapabilities;

typedef struct _OnvifDevice {
    char * protocol;
    char * ip;
    char * endpoint;
    char * port;
    char * hostname;
    pthread_mutex_t  * prop_lock;
    OnvifErrorTypes last_error;
    OnvifSoapClient* device_soap;
    OnvifSoapClient* media_soap;
    void * priv_ptr;

    pthread_mutex_t  * profile_lock;
    OnvifProfiles * profiles;
    struct http_da_info *  snapshot_da_info;
} OnvifDevice;

struct _OnvifCred {
    char * user;
    char * pass;
};

void OnvifDevice__set_last_error(OnvifDevice * self, OnvifErrorTypes error);

char * OnvifDevice__get_username(OnvifDevice *self){
    char * ret;
    pthread_mutex_lock(self->prop_lock);
    struct _OnvifCred * cred = (struct _OnvifCred *) self->priv_ptr;
    ret = malloc(strlen(cred->user)+1);
    strcpy(ret,cred->user);
    pthread_mutex_unlock(self->prop_lock);
    return ret;
}

char * OnvifDevice__get_password(OnvifDevice *self){
    char * ret;
    pthread_mutex_lock(self->prop_lock);
    struct _OnvifCred * cred = (struct _OnvifCred *) self->priv_ptr;
    ret = malloc(strlen(cred->pass)+1);
    strcpy(ret,cred->pass);
    pthread_mutex_unlock(self->prop_lock);
    return ret;
}


int set_wsse_data(OnvifDevice* self, struct soap *soap){
    int ret = 1;
    char * user = OnvifDevice__get_username(self);
    char * pass = OnvifDevice__get_password(self);
    soap_wsse_delete_Security(soap);
    if (soap_wsse_add_Timestamp(soap, TIME_CONST, 10)
        || soap_wsse_add_UsernameTokenDigest(soap, AUTH_CONST,user,pass)){
        //TODO Error handling
        printf("Unable to set wsse creds...\n");
        OnvifDevice__set_last_error(self,ONVIF_CONNECTION_ERROR);
        ret = 0;
    }

    //TODO support encryption
    free(user);
    free(pass);
    OnvifDevice__set_last_error(self,ONVIF_ERROR_NONE);
    return ret;
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

    pthread_mutex_lock(self->device_soap->lock);
    char * endpoint = OnvifDevice__get_device_endpoint(self);
    SoapWrapper * soap_wrap = OnvifSoapClient__new_soap(self->device_soap);
    
    printf("OnvifDevice__device_getCapabilities [%s]\n", endpoint);
    int wsseret = set_wsse_data(self,soap_wrap->soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___tds__GetCapabilities(soap_wrap->soap, endpoint, NULL, &gethostname,  &response);
    if (soapret == SOAP_OK){
        capabilities = (OnvifCapabilities *) malloc(sizeof(OnvifCapabilities));
        OnvifMedia* media =  (OnvifMedia *) malloc(sizeof(OnvifMedia));
        media->xaddr = malloc(strlen(response.Capabilities->Media->XAddr)+1);
        strcpy(media->xaddr,response.Capabilities->Media->XAddr);
        capabilities->media = media;
    } else {
        OnvifDevice__set_last_error(self,handle_soap_error(soap_wrap->soap,soapret));
    }

exit:
    free(endpoint);
    OnvifSoapClient__cleanup(soap_wrap);  
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

OnvifDeviceInformation * OnvifDevice__device_getDeviceInformation(OnvifDevice *self){
    struct _tds__GetDeviceInformation request;
    struct _tds__GetDeviceInformationResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    OnvifDeviceInformation *ret = NULL;

    pthread_mutex_lock(self->device_soap->lock);
    char * endpoint = OnvifDevice__get_device_endpoint(self);
    SoapWrapper * soap_wrap = OnvifSoapClient__new_soap(self->device_soap);

    printf("OnvifDevice__device_getDeviceInformation [%s]\n", endpoint);
    int wsseret = set_wsse_data(self,soap_wrap->soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___tds__GetDeviceInformation(soap_wrap->soap, endpoint, "", &request,  &response);
    if (soapret == SOAP_OK){
        ret = OnvifDeviceInformation__create(&response);
    } else {
        OnvifDevice__set_last_error(self,handle_soap_error(soap_wrap->soap,soapret));
    }

exit:
    free(endpoint);
    OnvifSoapClient__cleanup(soap_wrap);
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
                    ret_val = realloc(ret_val, strlen(ret_val) + strlen(output) + strlen(" ") +1);
                    strcat(ret_val, " ");
                    strcat(ret_val, output);
                }
            }
        }
  }

  return ret_val;
}

OnvifInterfaces * OnvifDevice__device_getNetworkInterfaces(OnvifDevice* self) {
    struct _tds__GetNetworkInterfaces req;
    struct _tds__GetNetworkInterfacesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifInterfaces * interfaces = NULL;

    pthread_mutex_lock(self->device_soap->lock);
    char * endpoint = OnvifDevice__get_device_endpoint(self);

    SoapWrapper * soap_wrap = OnvifSoapClient__new_soap(self->device_soap);

    printf("OnvifDevice__device_getNetworkInterfaces [%s]\n", endpoint);
    int wsseret = set_wsse_data(self,soap_wrap->soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___tds__GetNetworkInterfaces(soap_wrap->soap, endpoint, "", &req,  &resp);
    if (soapret == SOAP_OK){
        interfaces = OnvifInterfaces__create(&resp);
    } else {
        OnvifDevice__set_last_error(self,handle_soap_error(soap_wrap->soap,soapret));
    }

exit:
    free(endpoint);
    OnvifSoapClient__cleanup(soap_wrap);
    pthread_mutex_unlock(self->device_soap->lock);
    return interfaces;
}

OnvifScopes * OnvifDevice__device_getScopes(OnvifDevice* self) {
    struct _tds__GetScopes req;
    struct _tds__GetScopesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifScopes * scopes = NULL;

    pthread_mutex_lock(self->device_soap->lock);
    char * endpoint = OnvifDevice__get_device_endpoint(self);
    SoapWrapper * soap_wrap = OnvifSoapClient__new_soap(self->device_soap);

    printf("OnvifDevice__device_getScopes [%s]\n", endpoint);
    int wsseret = set_wsse_data(self,soap_wrap->soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___tds__GetScopes(soap_wrap->soap, endpoint, "", &req,  &resp);
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
            scopes->scopes = realloc (scopes->scopes, sizeof (OnvifScope*) * scopes->count);
            scopes->scopes[scopes->count-1] = onvifscope;

            // tt__ScopeDefinition__Fixed : tt__ScopeDefinition__Fixed || tt__ScopeDefinition__Configurable
        }
    } else {
        OnvifDevice__set_last_error(self,handle_soap_error(soap_wrap->soap,soapret));
    }

exit:
    free(endpoint);
    OnvifSoapClient__cleanup(soap_wrap);
    pthread_mutex_unlock(self->device_soap->lock);
    return scopes;
}

char * OnvifDevice__device_getHostname(OnvifDevice* self) {
    struct _tds__GetHostname gethostname;
    struct _tds__GetHostnameResponse response;
    memset (&gethostname, 0, sizeof (gethostname));
    memset (&response, 0, sizeof (response));

    char * ret = NULL;

    pthread_mutex_lock(self->device_soap->lock);
    char * endpoint = OnvifDevice__get_device_endpoint(self);
    SoapWrapper * soap_wrap = OnvifSoapClient__new_soap(self->device_soap);

    printf("OnvifDevice__device_getHostname [%s]\n", endpoint);
    int wsseret = set_wsse_data(self,soap_wrap->soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___tds__GetHostname(soap_wrap->soap, endpoint, "", &gethostname,  &response);
    if (soapret == SOAP_OK){
        ret = malloc(strlen(response.HostnameInformation->Name)+1); 
        strcpy(ret,response.HostnameInformation->Name);
    } else {
        OnvifDevice__set_last_error(self,handle_soap_error(soap_wrap->soap,soapret));
    }
    //TODO error handling timout, invalid url, etc...

exit:
    free(endpoint);
    OnvifSoapClient__cleanup(soap_wrap);
    pthread_mutex_unlock(self->device_soap->lock);
    return ret;
}

void OnvifDevice__media_getProfiles_lockable(OnvifDevice* self, int lockable){
    struct _trt__GetProfiles req;
    struct _trt__GetProfilesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    if(!OnvifSoapClient__is_valid(self->media_soap)){
        printf("ERROR media soap not created. Authenticate first.\n");
        OnvifDevice__set_last_error(self,ONVIF_SOAP_ERROR);
        return;
    }

    pthread_mutex_lock(self->media_soap->lock);
    char * endpoint = OnvifDevice__get_media_endpoint(self);
    SoapWrapper * soap_wrap = OnvifSoapClient__new_soap(self->media_soap);

    if(lockable)
        pthread_mutex_lock(self->profile_lock);

    printf("OnvifDevice__media_getProfiles [%s]\n", endpoint);
    int wsseret = set_wsse_data(self,soap_wrap->soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___trt__GetProfiles(soap_wrap->soap, endpoint, "", &req, &resp);
    if (soapret == SOAP_OK){
        // Traverse all configuration files in the response structure
        if (resp.Profiles != NULL && resp.__sizeProfiles > 0){
            OnvifProfiles__destroy(self->profiles);
            self->profiles = OnvifProfiles__create(&resp);
        }
    } else {
        OnvifDevice__set_last_error(self,handle_soap_error(soap_wrap->soap,soapret));
    }

exit:
    free(endpoint);
    OnvifSoapClient__cleanup(soap_wrap);
    pthread_mutex_unlock(self->media_soap->lock);
    if(lockable)
        pthread_mutex_unlock(self->profile_lock);
}

void OnvifDevice__get_profile_token(OnvifDevice *self, int index, char * ret){
    pthread_mutex_lock(self->profile_lock);
    printf("OnvifDevice__get_profile_token\n");
    if(!self->profiles){
        printf("OnvifDevice__get_profile_token : Initializing profiles\n");
        OnvifDevice__media_getProfiles_lockable(self,0);
    }

    int profileCount = OnvifProfiles__get_size(self->profiles);
    if(index >= profileCount){
        char * endpoint = OnvifDevice__get_media_endpoint(self);
        printf("OnvifDevice__get_profile_token : profile index out-of-bounds. [%s]\n", endpoint);
        free(endpoint);
    } else if(profileCount > 0 && profileCount > index) {
        OnvifProfile * profile = OnvifProfiles__get_profile(self->profiles,index);
        char * token = OnvifProfile__get_token(profile);
        if(token){
            printf("OnvifDevice__get_profile_token : Found profile [%s]\n",token);
            strcpy(ret,token);
        } else {
            printf("OnvifDevice__get_profile_token : Found profile, but token was NULL\n");
        }
    } else {
        printf("OnvifDevice__get_profile_token : profile not found\n");
    }
    pthread_mutex_unlock(self->profile_lock);
}

char * OnvifDevice__media_getStreamUri(OnvifDevice* self, int profile_index){
    struct _trt__GetStreamUri req;
    struct _trt__GetStreamUriResponse resp;
    char * ret = NULL;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    if(!OnvifSoapClient__is_valid(self->media_soap)){
        printf("ERROR media soap not created. Authenticate first.\n");
        OnvifDevice__set_last_error(self,ONVIF_SOAP_ERROR);
        return NULL;
    }

    char token[255];
    memset(&token,0,sizeof(token));
    OnvifDevice__get_profile_token(self,profile_index, token);
    req.ProfileToken = token;

    pthread_mutex_lock(self->media_soap->lock);
    char * endpoint = OnvifDevice__get_media_endpoint(self);
    SoapWrapper * soap_wrap = OnvifSoapClient__new_soap(self->media_soap);

    printf("OnvifDevice__media_getStreamUri [%s][%i]\n", endpoint,profile_index);
    // req.StreamSetup = (struct tt__StreamSetup*)soap_malloc(self->media_soap->soap,sizeof(struct tt__StreamSetup));//Initialize, allocate space
	req.StreamSetup = soap_new_tt__StreamSetup(soap_wrap->soap,1);
    req.StreamSetup->Stream = tt__StreamType__RTP_Unicast;//stream type

	// req.StreamSetup->Transport = (struct tt__Transport *)soap_malloc(self->media_soap->soap, sizeof(struct tt__Transport));//Initialize, allocate space
	req.StreamSetup->Transport = soap_new_tt__Transport(soap_wrap->soap,1);
    req.StreamSetup->Transport->Protocol = tt__TransportProtocol__UDP;
	req.StreamSetup->Transport->Tunnel = 0;

    int wsseret = set_wsse_data(self,soap_wrap->soap);
    if(!wsseret){
        //TODO Error handling
        goto exit;
    }

    int soapret = soap_call___trt__GetStreamUri(soap_wrap->soap, endpoint, "", &req, &resp);
    if (soapret == SOAP_OK){
        ret = malloc(strlen(resp.MediaUri->Uri)+1);
        strcpy(ret,resp.MediaUri->Uri);
    } else {
        OnvifDevice__set_last_error(self,handle_soap_error(soap_wrap->soap,soapret));
    }

exit:
    free(endpoint);
    OnvifSoapClient__cleanup(soap_wrap);
    pthread_mutex_unlock(self->media_soap->lock);
    return ret;
}

void OnvifDevice__media_getProfiles(OnvifDevice* self){
    OnvifDevice__media_getProfiles_lockable(self,1);
}  

//TODO Support timeout and invalidafter flag
char * OnvifDevice__media_getSnapshotUri(OnvifDevice *self, int profile_index){
    struct _trt__GetSnapshotUri request;
    struct _trt__GetSnapshotUriResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));
    char * ret_val = NULL;

    printf("OnvifDevice__media_getSnapshotUri\n");
    char token[255];
    OnvifDevice__get_profile_token(self,profile_index, token);
    request.ProfileToken = token;

    pthread_mutex_lock(self->media_soap->lock);
    char * endpoint = OnvifDevice__get_media_endpoint(self);
    SoapWrapper * soap_wrap = OnvifSoapClient__new_soap(self->media_soap);

    printf("OnvifDevice__media_getSnapshotUri [%s][%i]\n", endpoint,profile_index);
    int wsseret = set_wsse_data(self,soap_wrap->soap);
    if(!wsseret){
        //TODO Error handling
        printf("OnvifDevice__media_getSnapshotUri - Failed to set credentials\n");
        goto exit;
    }

    int soapret = soap_call___trt__GetSnapshotUri(soap_wrap->soap, endpoint, NULL, &request,  &response);
    if (soapret == SOAP_OK){
        ret_val = malloc(strlen(response.MediaUri->Uri) + 1);
        strcpy(ret_val,response.MediaUri->Uri);
    } else {
        OnvifDevice__set_last_error(self,handle_soap_error(soap_wrap->soap,soapret));
    }

exit:
    free(endpoint);
    OnvifSoapClient__cleanup(soap_wrap);
    pthread_mutex_unlock(self->media_soap->lock);
    return ret_val;
}

OnvifSnapshot * get_http_body(OnvifDevice *self, char * url)
{
    OnvifSnapshot chunc;
    OnvifSnapshot * nchunc = NULL;
    memset (&chunc, 0, sizeof (chunc));
    printf("get_http_body\n");
    //Creating soap instance to avoid long running locks.
    struct soap * soap = soap_new1(SOAP_IO_CHUNK);
    if (!self->snapshot_da_info){
        self->snapshot_da_info = malloc(sizeof(struct http_da_info));
        memset (self->snapshot_da_info, 0, sizeof(struct http_da_info));
    }

    soap_register_plugin(soap, http_da);

    if (self->snapshot_da_info->authrealm){
        http_da_restore(soap, self->snapshot_da_info);
    }
    char * user = NULL;
    char * pass = NULL;
    if (soap_GET(soap, url, NULL)
            || soap_begin_recv(soap)
            || (chunc.buffer = soap_http_get_body(soap, &(chunc.size))) != NULL
            || soap_end_recv(soap)){
            if (soap->error == 401){
                user = OnvifDevice__get_username(self);
                pass = OnvifDevice__get_password(self);
                http_da_save(soap, self->snapshot_da_info, soap->authrealm, user, pass);
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
    free(user);
    free(pass);
    //Copy OnvifSnapshot before cleanup
    nchunc = malloc(sizeof(OnvifSnapshot));
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

OnvifSnapshot * OnvifDevice__media_getSnapshot(OnvifDevice *self, int profile_index){
    printf("OnvifDevice__media_getSnapshot\n");
    char * snapshot_uri = OnvifDevice__media_getSnapshotUri(self, profile_index);
    printf("snapshot_uri : %s\n",snapshot_uri);
    OnvifSnapshot * ret = get_http_body(self, snapshot_uri);
    free(snapshot_uri);
    return ret;
}

void OnvifDevice_authenticate(OnvifDevice* self){
    char * endpoint = OnvifDevice__get_device_endpoint(self);
    printf("OnvifDevice_authenticate [%s]\n", endpoint);
    free(endpoint);

    OnvifDevice__device_createMediaSoap(self);
    if(!OnvifSoapClient__is_valid(self->media_soap)){
        return;
    }

    endpoint = OnvifDevice__get_media_endpoint(self);
    printf("Created Media soap [%s]\n",endpoint);
    free(endpoint);

    char * stream_uri = OnvifDevice__media_getStreamUri(self,0);
    if(!stream_uri){
        return;
    }
    printf("StreamURI : %s\n",stream_uri);
}

void OnvifDevice_set_credentials(OnvifDevice* self,const char * user,const char* pass){
    pthread_mutex_lock(self->prop_lock);
    struct  _OnvifCred * pcred = (struct _OnvifCred *) self->priv_ptr;
    pcred->user = realloc(pcred->user,strlen(user) + 1);
    pcred->pass = realloc(pcred->pass,strlen(pass) + 1);
    strcpy(pcred->pass,pass);
    strcpy(pcred->user,user);
    pthread_mutex_unlock(self->prop_lock);
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
    self->prop_lock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(self->prop_lock, NULL);
    self->last_error = ONVIF_NOT_SET;
    struct _OnvifCred * cred = malloc(sizeof(struct _OnvifCred));
    cred->pass = malloc(0);
    cred->user = malloc(0);
    self->priv_ptr = cred;
    self->device_soap = OnvifSoapClient__create();
    OnvifSoapClient__set_endpoint(self->device_soap,(char *)device_url);
    self->media_soap = OnvifSoapClient__create();
    self->profiles = NULL;
    self->profile_lock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(self->profile_lock, NULL);
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

void OnvifDevice__destroy(OnvifDevice* device) {
    printf("OnvifDevice__destroy\n");
    if (device) {
        OnvifCred__destroy((struct _OnvifCred *)device->priv_ptr);
        OnvifSoapClient__destroy(device->device_soap);
        OnvifSoapClient__destroy(device->media_soap);
        OnvifProfiles__destroy(device->profiles);
        if(device->snapshot_da_info)
            free(device->snapshot_da_info);
        pthread_mutex_destroy(device->profile_lock);
        pthread_mutex_destroy(device->prop_lock);
        free(device->profiles);
        free(device);
    }
}

char * OnvifDevice__get_ip(OnvifDevice* self){
    char * ret;
    pthread_mutex_lock(self->prop_lock);
    ret = malloc(strlen(self->ip)+1);
    strcpy(ret,self->ip);
    pthread_mutex_unlock(self->prop_lock);
    return ret;
}

char * OnvifDevice__get_port(OnvifDevice* self){
    char * ret;
    pthread_mutex_lock(self->prop_lock);
    ret = malloc(strlen(self->port)+1);
    strcpy(ret,self->port);
    pthread_mutex_unlock(self->prop_lock);
    return ret;
}

OnvifProfiles * OnvifDevice__get_profiles(OnvifDevice * self){
    OnvifProfiles * ret;
    pthread_mutex_lock(self->profile_lock);
    if(!self->profiles){
        printf("OnvifDevice__get_profile_token : Initializing profiles\n");
        OnvifDevice__media_getProfiles_lockable(self,0);
    }
    ret = OnvifProfiles__copy(self->profiles);
    pthread_mutex_unlock(self->profile_lock);
    return ret;
}

double OnvifSnapshot__get_size(OnvifSnapshot * self){
    return self->size;
}

char * OnvifSnapshot__get_buffer(OnvifSnapshot * self){
    return self->buffer;
}

void OnvifDevice__set_last_error(OnvifDevice * self, OnvifErrorTypes error){
    pthread_mutex_lock(self->prop_lock);
    self->last_error = error;
    pthread_mutex_unlock(self->prop_lock);
}

OnvifErrorTypes OnvifDevice__get_last_error(OnvifDevice * self){
    OnvifErrorTypes ret;
    pthread_mutex_lock(self->prop_lock);
    ret = self->last_error;
    pthread_mutex_unlock(self->prop_lock);
    return ret;
}

char * OnvifDevice__get_device_endpoint(OnvifDevice * self){
    char * ret = NULL;
    pthread_mutex_lock(self->prop_lock);
    ret = malloc(strlen(self->device_soap->endpoint)+1);
    strcpy(ret,self->device_soap->endpoint);
    pthread_mutex_unlock(self->prop_lock);
    return ret;
}

char * OnvifDevice__get_media_endpoint(OnvifDevice * self){
    if(!self || !self->media_soap){
        return NULL;
    }

    char * ret = NULL;
    pthread_mutex_lock(self->prop_lock);
    ret = malloc(strlen(self->media_soap->endpoint)+1);
    strcpy(ret,self->media_soap->endpoint);
    pthread_mutex_unlock(self->prop_lock);
    return ret;
}