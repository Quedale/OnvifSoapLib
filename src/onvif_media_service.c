#include "onvif_media_service.h"
#include "onvif_media_profile_local.h"
#include "httpda.h"

typedef struct _OnvifMediaService {
    OnvifBaseService * parent;
    OnvifProfiles * profiles;
    MUTEX_TYPE profile_lock;
    struct http_da_info *  snapshot_da_info;
} OnvifMediaService;

OnvifMediaService * OnvifMediaService__create(const char * endpoint, OnvifCredentials * credentials, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    OnvifMediaService * self = malloc(sizeof(OnvifMediaService));
    OnvifMediaService__init(self,endpoint, credentials, error_cb, error_data);
    return self;
}

void OnvifMediaService__init(OnvifMediaService * self,const char * endpoint, OnvifCredentials * credentials, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    self->parent = OnvifBaseService__create(endpoint, credentials, error_cb, error_data);
    self->profile_lock = MUTEX_INITIALIZER;
    MUTEX_SETUP(self->profile_lock);

    self->profiles = NULL;
    self->snapshot_da_info = NULL;
}

void OnvifMediaService__destroy(OnvifMediaService * self){
    if(self){
        OnvifBaseService__destroy(self->parent);
        OnvifProfiles__destroy(self->profiles);
        MUTEX_CLEANUP(self->profile_lock);
        free(self->profile_lock);
        if(self->snapshot_da_info)
            free(self->snapshot_da_info);
        free(self);
    }
}

OnvifBaseService * OnvifMediaService__get_parent(OnvifMediaService * self){
    return self->parent;
}

char * OnvifMediaService__get_endpoint(OnvifMediaService * self){
    return OnvifBaseService__get_endpoint(self->parent);
}

int OnvifMediaService__check_profiles(OnvifMediaService * self){
    if(!self->profiles){
        printf("OnvifMediaService__get_profile_token : Initializing profiles\n");
        self->profiles = OnvifMediaService__getProfiles(self);
    }

    return self->profiles != NULL;
}

OnvifProfiles * OnvifMediaService__get_profiles(OnvifMediaService * self){
    OnvifProfiles * ret;
    MUTEX_LOCK(self->profile_lock);
    if(!OnvifMediaService__check_profiles(self)) return NULL;
    ret = OnvifProfiles__copy(self->profiles);
    MUTEX_UNLOCK(self->profile_lock);
    return ret;
}

OnvifProfiles * OnvifMediaService__getProfiles(OnvifMediaService* self){
    if(!self){
        printf("OnvifMediaService__getProfiles - MediaService uninitialized.\n");
        return NULL;
    }
    struct _trt__GetProfiles req;
    struct _trt__GetProfilesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifBaseService__lock(self->parent);
    OnvifProfiles * ret = NULL;
    char * endpoint = OnvifMediaService__get_endpoint(self);
    struct soap * soap = OnvifBaseService__soap_new(self->parent);
    if(!soap){
        OnvifBaseService__set_error_code(self->parent,ONVIF_CONNECTION_ERROR);
        goto exit;
    } else {
        OnvifBaseService__set_error_code(self->parent,ONVIF_ERROR_NONE);
    }
    
    int soapret = soap_call___trt__GetProfiles(soap, endpoint, "", &req, &resp);
    if (soapret == SOAP_OK){
        // Traverse all configuration files in the response structure
        ret = OnvifProfiles__create(&resp);
    } else {
        OnvifBaseService__handle_soap_error(self->parent,soap,soapret);
    }

exit:
    free(endpoint);
    OnvifBaseService__soap_destroy(soap);
    OnvifBaseService__unlock(self->parent);
    
    return ret;
}

char * OnvifMediaService__getStreamUri(OnvifMediaService* self, int profile_index){
    if(!self){
        printf("OnvifMediaService__getStreamUri - MediaService uninitialized.\n");
        return NULL;
    }
    struct _trt__GetStreamUri req;
    struct _trt__GetStreamUriResponse resp;
    char * ret = NULL;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    char token[255];
    memset(&token,0,sizeof(token));
    OnvifMediaService__get_profile_token(self,profile_index, token);
    req.ProfileToken = token;

    OnvifBaseService__lock(self->parent);
    char * endpoint = OnvifMediaService__get_endpoint(self);
    struct soap * soap = OnvifBaseService__soap_new(self->parent);
    if(!soap){
        OnvifBaseService__set_error_code(self->parent,ONVIF_CONNECTION_ERROR);
        goto exit;
    } else {
        OnvifBaseService__set_error_code(self->parent,ONVIF_ERROR_NONE);
    }
    
    printf("OnvifMediaService__getStreamUri [%s][%i]\n", endpoint,profile_index);
    // req.StreamSetup = (struct tt__StreamSetup*)soap_malloc(self->media_soap,sizeof(struct tt__StreamSetup));//Initialize, allocate space
	req.StreamSetup = soap_new_tt__StreamSetup(soap,1);
    req.StreamSetup->Stream = tt__StreamType__RTP_Unicast;//stream type

	// req.StreamSetup->Transport = (struct tt__Transport *)soap_malloc(self->media_soap, sizeof(struct tt__Transport));//Initialize, allocate space
	req.StreamSetup->Transport = soap_new_tt__Transport(soap,1);
    req.StreamSetup->Transport->Protocol = tt__TransportProtocol__UDP;
	req.StreamSetup->Transport->Tunnel = 0;

    int soapret = soap_call___trt__GetStreamUri(soap, endpoint, "", &req, &resp);
    if (soapret == SOAP_OK){
        ret = malloc(strlen(resp.MediaUri->Uri)+1);
        strcpy(ret,resp.MediaUri->Uri);
    } else {
        OnvifBaseService__handle_soap_error(self->parent,soap,soapret);
    }

exit:
    free(endpoint);
    OnvifBaseService__soap_destroy(soap);
    OnvifBaseService__unlock(self->parent);
    return ret;
}


//TODO Support timeout and invalidafter flag
char * OnvifMediaService__getSnapshotUri(OnvifMediaService *self, int profile_index){
    if(!self){
        printf("OnvifMediaService__getSnapshotUri - MediaService uninitialized.\n");
        return NULL;
    }
    struct _trt__GetSnapshotUri request;
    struct _trt__GetSnapshotUriResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));
    char * ret_val = NULL;

    printf("OnvifMediaService__getSnapshotUri\n");
    char token[255];
    OnvifMediaService__get_profile_token(self,profile_index, token);
    request.ProfileToken = token;

    OnvifBaseService__lock(self->parent);
    char * endpoint = OnvifMediaService__get_endpoint(self);
    struct soap * soap = OnvifBaseService__soap_new(self->parent);
    if(!soap){
        OnvifBaseService__set_error_code(self->parent,ONVIF_CONNECTION_ERROR);
        goto exit;
    } else {
        OnvifBaseService__set_error_code(self->parent,ONVIF_ERROR_NONE);
    }

    int soapret = soap_call___trt__GetSnapshotUri(soap, endpoint, NULL, &request,  &response);
    if (soapret == SOAP_OK){
        ret_val = malloc(strlen(response.MediaUri->Uri) + 1);
        strcpy(ret_val,response.MediaUri->Uri);
    } else {
        OnvifBaseService__handle_soap_error(self->parent,soap,soapret);
    }

exit:
    free(endpoint);
    OnvifBaseService__soap_destroy(soap);
    OnvifBaseService__unlock(self->parent);
    return ret_val;
}

OnvifSnapshot * get_http_body(OnvifMediaService *self, char * url)
{   
    size_t size = 0;
    char * buffer = NULL;
    OnvifSnapshot * snap = NULL;

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
            || (buffer = soap_http_get_body(soap, &size)) != NULL
            || soap_end_recv(soap)){
            if (soap->error == 401){
                user = OnvifCredentials__get_username(OnvifBaseService__get_credentials(self->parent));
                pass = OnvifCredentials__get_password(OnvifBaseService__get_credentials(self->parent));
                http_da_save(soap, self->snapshot_da_info, soap->authrealm, user, pass);
                if (soap_GET(soap, url, NULL)
                    || soap_begin_recv(soap)
                    || (buffer = soap_http_get_body(soap, &size)) != NULL
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
    
    snap = OnvifSnapshot__create(size,buffer);

exit:
    http_da_release(soap, self->snapshot_da_info);
    soap_closesock(soap);
    soap_destroy(soap);
    soap_end(soap);
    soap_done(soap);
    soap_free(soap);
    return snap;
}

OnvifSnapshot * OnvifMediaService__getSnapshot(OnvifMediaService *self, int profile_index){
    printf("OnvifMediaService__getSnapshot\n");
    char * snapshot_uri = OnvifMediaService__getSnapshotUri(self, profile_index);
    printf("snapshot_uri : %s\n",snapshot_uri);
    OnvifSnapshot * ret = get_http_body(self, snapshot_uri);
    free(snapshot_uri);
    return ret;
}

void OnvifMediaService__get_profile_token(OnvifMediaService *self, int index, char * ret){
    if(!self){
        printf("OnvifMediaService__get_profile_token - MediaService uninitialized.\n");
        ret[0] = '\0'; 
        return;
    }
    MUTEX_LOCK(self->profile_lock);
    printf("OnvifMediaService__get_profile_token\n");
    if(!OnvifMediaService__check_profiles(self)) { 
        ret[0] = '\0'; 
        return;
    }

    int profileCount = OnvifProfiles__get_size(self->profiles);
    if(index >= profileCount){
        char * endpoint = OnvifMediaService__get_endpoint(self);
        printf("OnvifMediaService__get_profile_token : profile index out-of-bounds. [%s]\n", endpoint);
        free(endpoint);
    } else if(profileCount > 0 && profileCount > index) {
        OnvifProfile * profile = OnvifProfiles__get_profile(self->profiles,index);
        char * token = OnvifProfile__get_token(profile);
        if(token){
            printf("OnvifMediaService__get_profile_token : Found profile [%s]\n",token);
            strcpy(ret,token);
        } else {
            printf("OnvifMediaService__get_profile_token : Found profile, but token was NULL\n");
        }
    } else {
        printf("OnvifMediaService__get_profile_token : profile not found\n");
    }
    MUTEX_UNLOCK(self->profile_lock);
}