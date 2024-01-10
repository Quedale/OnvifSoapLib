#include "onvif_media_service.h"
#include "onvif_media_profile_local.h"
#include "onvif_base_service_local.h"
#include "httpda.h"
#include "clogger.h"

typedef struct _OnvifMediaService {
    OnvifBaseService * parent;
    OnvifProfiles * profiles;
    P_MUTEX_TYPE profile_lock;
    struct http_da_info *  snapshot_da_info;
} OnvifMediaService;

OnvifMediaService * OnvifMediaService__create(OnvifDevice * device, const char * endpoint, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    OnvifMediaService * self = malloc(sizeof(OnvifMediaService));
    OnvifMediaService__init(self,device, endpoint, error_cb, error_data);
    return self;
}

void OnvifMediaService__init(OnvifMediaService * self, OnvifDevice * device, const char * endpoint, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    self->parent = OnvifBaseService__create(device, endpoint, error_cb, error_data);
    P_MUTEX_SETUP(self->profile_lock);

    self->profiles = NULL;
    self->snapshot_da_info = NULL;
}

void OnvifMediaService__destroy(OnvifMediaService * self){
    if(self){
        OnvifBaseService__destroy(self->parent);
        OnvifProfiles__destroy(self->profiles);
        P_MUTEX_CLEANUP(self->profile_lock);
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
        C_INFO("OnvifMediaService__get_profile_token : Initializing profiles\n");
        self->profiles = OnvifMediaService__getProfiles(self);
    }

    return self->profiles != NULL;
}

OnvifProfiles * OnvifMediaService__get_profiles(OnvifMediaService * self){
    OnvifProfiles * ret;
    P_MUTEX_LOCK(self->profile_lock);
    if(!OnvifMediaService__check_profiles(self)) return NULL;
    ret = OnvifProfiles__copy(self->profiles);
    P_MUTEX_UNLOCK(self->profile_lock);
    return ret;
}

OnvifProfiles * OnvifMediaService__getProfiles(OnvifMediaService* self){
    if(!self){
        C_ERROR("OnvifMediaService__getProfiles - MediaService uninitialized.\n");
        return NULL;
    }
    struct _trt__GetProfiles req;
    struct _trt__GetProfilesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifProfiles * profiles = NULL;
    
    ONVIF_INVOKE_SOAP_CALL(self, trt__GetProfiles, OnvifProfiles__create, profiles, soap, endpoint, NULL, &req,  &resp);
    
    return profiles;
}

char * OnvifMediaService__getStreamUri_callback(struct _trt__GetStreamUriResponse * resp){
    char * ret = malloc(strlen(resp->MediaUri->Uri)+1);
    strcpy(ret,resp->MediaUri->Uri);
    return ret;
}

char * OnvifMediaService__getStreamUri(OnvifMediaService* self, int profile_index){
    if(!self){
        C_ERROR("OnvifMediaService__getStreamUri - MediaService uninitialized.\n");
        return NULL;
    }
    struct _trt__GetStreamUri req;
    struct _trt__GetStreamUriResponse resp;
    char * streamuri = NULL;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    char token[255];
    memset(&token,0,sizeof(token));
    OnvifMediaService__get_profile_token(self,profile_index, token);
    req.ProfileToken = token;

    struct tt__StreamSetup ssetup;
    memset(&ssetup,0,sizeof(ssetup));
    req.StreamSetup = &ssetup;
    req.StreamSetup->Stream = tt__StreamType__RTP_Unicast;//stream type

    struct tt__Transport transport;
    memset(&transport,0,sizeof(transport));
    req.StreamSetup->Transport = &transport;
    req.StreamSetup->Transport->Protocol = tt__TransportProtocol__UDP;
    req.StreamSetup->Transport->Tunnel = 0;
    ONVIF_INVOKE_SOAP_CALL(self, trt__GetStreamUri, OnvifMediaService__getStreamUri_callback, streamuri, soap, endpoint, NULL, &req,  &resp);

    return streamuri;
}


char * OnvifMediaService__getSnapshotUri_callback(struct _trt__GetSnapshotUriResponse * response){
    char * ret_val = malloc(strlen(response->MediaUri->Uri) + 1);
    strcpy(ret_val,response->MediaUri->Uri);
    return ret_val;
}

//TODO Support timeout and invalidafter flag
char * OnvifMediaService__getSnapshotUri(OnvifMediaService *self, int profile_index){
    if(!self){
        C_ERROR("OnvifMediaService__getSnapshotUri - MediaService uninitialized.\n");
        return NULL;
    }
    struct _trt__GetSnapshotUri request;
    struct _trt__GetSnapshotUriResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));
    char * ret_val = NULL;

    C_DEBUG("OnvifMediaService__getSnapshotUri\n");
    char token[255];
    OnvifMediaService__get_profile_token(self,profile_index, token);
    request.ProfileToken = token;

    ONVIF_INVOKE_SOAP_CALL(self, trt__GetSnapshotUri, OnvifMediaService__getSnapshotUri_callback, ret_val, soap, endpoint, NULL, &request,  &response);

    return ret_val;
}

OnvifSnapshot * get_http_body(OnvifMediaService *self, char * url, int retry_flag)
{   
    size_t size = 0;
    char * buffer = NULL;
    OnvifSnapshot * snap = NULL;

    C_TRACE("get_http_body\n");
    //Creating soap instance to avoid long running locks.
    struct soap * soap = soap_new1(SOAP_IO_CHUNK);
    soap->connect_timeout = 2; // 2 sec
    soap->recv_timeout = soap->send_timeout = 10;//10 sec
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
            if(soap->error == SOAP_OK){
                snap = OnvifSnapshot__create(size,buffer);
            } else if (soap->error == 401){
                user = OnvifCredentials__get_username(OnvifDevice__get_credentials(OnvifBaseService__get_device(self->parent)));
                pass = OnvifCredentials__get_password(OnvifDevice__get_credentials(OnvifBaseService__get_device(self->parent)));
                http_da_save(soap, self->snapshot_da_info, soap->authrealm, user, pass);
                if (soap_GET(soap, url, NULL)
                    || soap_begin_recv(soap)
                    || (buffer = soap_http_get_body(soap, &size)) != NULL
                    || soap_end_recv(soap)){
                    snap = OnvifSnapshot__create(size,buffer);
                }else {
                    if (soap->error == SOAP_UDP_ERROR || soap->error == SOAP_TCP_ERROR){
                        char * new_endpoint = NULL;
                        char * master_port = OnvifDevice__get_port(OnvifBaseService__get_device(self->parent)); 
                        new_endpoint = URL__set_port(url, master_port);
                        if(strcmp(new_endpoint,url) != 0){ /* TODO compare port before constructing new URL */
                            C_WARN("Connection error. Attempting to correct URL : '%s' --> '%s'", url, new_endpoint);
                            snap = get_http_body(self, new_endpoint, 1);
                        }
                        free(new_endpoint);
                        free(master_port);
                    } else {
                        //TODO handle error codes
                        C_ERROR("get_http_body ERROR-1\n");
                        soap_print_fault(soap, stderr);
                    }
                }
            } else if (soap->error == SOAP_UDP_ERROR || soap->error == SOAP_TCP_ERROR){
                char * new_endpoint = NULL;
                char * master_port = OnvifDevice__get_port(OnvifBaseService__get_device(self->parent)); 
                new_endpoint = URL__set_port(url, master_port);
                if(strcmp(new_endpoint,url) != 0){ /* TODO compare port before constructing new URL */
                    C_WARN("Connection error. Attempting to correct URL : '%s' --> '%s'", url, new_endpoint);
                    snap = get_http_body(self, new_endpoint, 1);
                }
                free(new_endpoint);
                free(master_port);
            } else {
                //TODO handle error codes
                C_ERROR("get_http_body ERROR-2 [%d]\n", soap->error);
                soap_print_fault(soap, stderr);
            }

    } else {
        //TODO handle error codes
        C_ERROR("get_http_body ERROR-3 [%d]\n", soap->error);
        soap_print_fault(soap, stderr);
    }
    free(user);
    free(pass);
    http_da_release(soap, self->snapshot_da_info);
    soap_closesock(soap);
    soap_destroy(soap);
    soap_end(soap);
    soap_done(soap);
    soap_free(soap);
    return snap;
}

OnvifSnapshot * OnvifMediaService__getSnapshot(OnvifMediaService *self, int profile_index){
    C_DEBUG("OnvifMediaService__getSnapshot\n");
    char * snapshot_uri = OnvifMediaService__getSnapshotUri(self, profile_index);
    C_INFO("Snapshot URI : %s\n",snapshot_uri);
    OnvifSnapshot * ret = get_http_body(self, snapshot_uri, 0);
    free(snapshot_uri);
    return ret;
}

void OnvifMediaService__get_profile_token(OnvifMediaService *self, int index, char * ret){
    if(!self){
        C_ERROR("OnvifMediaService__get_profile_token - MediaService uninitialized.\n");
        ret[0] = '\0'; 
        return;
    }
    P_MUTEX_LOCK(self->profile_lock);
    C_TRACE("OnvifMediaService__get_profile_token\n");
    if(!OnvifMediaService__check_profiles(self)) { 
        ret[0] = '\0'; 
        P_MUTEX_UNLOCK(self->profile_lock);
        return;
    }

    int profileCount = OnvifProfiles__get_size(self->profiles);
    if(index >= profileCount){
        char * endpoint = OnvifMediaService__get_endpoint(self);
        C_ERROR("OnvifMediaService__get_profile_token : profile index out-of-bounds. [%s]\n", endpoint);
        free(endpoint);
    } else if(profileCount > 0 && profileCount > index) {
        OnvifProfile * profile = OnvifProfiles__get_profile(self->profiles,index);
        char * token = OnvifProfile__get_token(profile);
        if(token){
            C_TRACE("OnvifMediaService__get_profile_token : Found profile [%s]\n",token);
            strcpy(ret,token);
        } else {
            C_ERROR("OnvifMediaService__get_profile_token : Found profile, but token was NULL\n");
        }
    } else {
        C_ERROR("OnvifMediaService__get_profile_token : profile not found\n");
    }
    P_MUTEX_UNLOCK(self->profile_lock);
}