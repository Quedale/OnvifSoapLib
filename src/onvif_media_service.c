#include "onvif_media_service.h"
#include "onvif_media_profile_local.h"
#include "onvif_base_service_local.h"
#include "httpda.h"
#include "plugin/logging.h"
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
        if(self->snapshot_da_info){
            //It might be more efficient to clean up da_info manually
            struct soap fakesoap;
            soap_init1(&fakesoap, SOAP_XML_CANONICAL | SOAP_C_UTFSTRING);
            soap_register_plugin(&fakesoap, http_da);
            http_da_release(&fakesoap, self->snapshot_da_info);
            free(self->snapshot_da_info);
            soap_destroy(&fakesoap);
            soap_end(&fakesoap);
            soap_done(&fakesoap);
        }

        free(self);
    }
}

OnvifBaseService * OnvifMediaService__get_parent(OnvifMediaService * self){
    return self->parent;
}

char * OnvifMediaService__get_endpoint(OnvifMediaService * self){
    if(!self) return NULL;
    return OnvifBaseService__get_endpoint(self->parent);
}

int OnvifMediaService__check_profiles(OnvifMediaService * self){
    if(!self->profiles){
        self->profiles = OnvifMediaService__getProfiles(self);
    }

    return self->profiles != NULL;
}

OnvifProfiles * OnvifMediaService__get_profiles(OnvifMediaService * self){
    if(!self) return NULL;
    OnvifProfiles * ret;
    P_MUTEX_LOCK(self->profile_lock);
    if(!OnvifMediaService__check_profiles(self)) return NULL;
    ret = OnvifProfiles__copy(self->profiles);
    P_MUTEX_UNLOCK(self->profile_lock);
    return ret;
}

OnvifProfiles * OnvifMediaService__getProfiles(OnvifMediaService* self){
    if(!self){
        char * endpoint = OnvifMediaService__get_endpoint(self);
        C_ERROR("[%s] OnvifMediaService__getProfiles - MediaService uninitialized.",endpoint);
        free(endpoint);
        return NULL;
    }
    struct _trt__GetProfiles req;
    struct _trt__GetProfilesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifProfiles * profiles = NULL;
    
    ONVIF_INVOKE_SOAP_CALL(self, trt__GetProfiles, OnvifProfiles__create, profiles, soap, NULL, &req,  &resp);
    
    return profiles;
}

char * OnvifMediaService__getStreamUri_callback(struct _trt__GetStreamUriResponse * resp){
    char * ret = malloc(strlen(resp->MediaUri->Uri)+1);
    strcpy(ret,resp->MediaUri->Uri);
    return ret;
}

char * OnvifMediaService__getStreamUri(OnvifMediaService* self, int profile_index){
    if(!self){
        char * endpoint = OnvifMediaService__get_endpoint(self);
        C_ERROR("[%s] OnvifMediaService__getStreamUri - MediaService uninitialized.",endpoint);
        free(endpoint);
        return NULL;
    }

    char token[255];
    memset(&token,0,sizeof(token));
    OnvifMediaService__get_profile_token(self,profile_index, token);
    if(strlen(token) == 0) { C_WARN("Profile token not found."); return NULL; }

    struct _trt__GetStreamUri req;
    struct _trt__GetStreamUriResponse resp;
    char * streamuri = NULL;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));
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
    ONVIF_INVOKE_SOAP_CALL(self, trt__GetStreamUri, OnvifMediaService__getStreamUri_callback, streamuri, soap, NULL, &req,  &resp);

    return streamuri;
}


char * OnvifMediaService__getSnapshotUri_callback(struct _trt__GetSnapshotUriResponse * response){
    char * ret_val = malloc(strlen(response->MediaUri->Uri) + 1);
    strcpy(ret_val,response->MediaUri->Uri);
    return ret_val;
}

//TODO Support timeout and invalidafter flag
char * OnvifMediaService__getSnapshotUri(OnvifMediaService *self, int profile_index){
    char * endpoint = OnvifMediaService__get_endpoint(self);
    char * ret_val = NULL;
    if(!self){
        C_ERROR("[%s] OnvifMediaService__getSnapshotUri - MediaService uninitialized.",endpoint);
        goto exit;
    }
    struct _trt__GetSnapshotUri request;
    struct _trt__GetSnapshotUriResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    char token[255];
    OnvifMediaService__get_profile_token(self,profile_index, token);
    request.ProfileToken = token;

    ONVIF_INVOKE_SOAP_CALL(self, trt__GetSnapshotUri, OnvifMediaService__getSnapshotUri_callback, ret_val, soap, NULL, &request,  &response);

exit:
    free(endpoint);
    return ret_val;
}

OnvifSnapshot * get_http_body(OnvifMediaService *self, struct soap * soap, char * url, int retry_flag)
{   
    size_t size = 0;
    char * buffer = NULL;
    OnvifSnapshot * snap = NULL;

    if (soap_GET(soap, url, NULL) || soap_begin_recv(soap) || (buffer = soap_http_get_body(soap, &size)) != NULL || soap_end_recv(soap)){
        if(soap->error == SOAP_OK){ //Successfully
            snap = OnvifSnapshot__create(size,buffer);
            goto exit;
        } else if (soap->error == 401){ //HTTP authentication challenge
            C_DEBUG("Snapshot WWW-Authorization challenge '%s'",soap->authrealm);
            if(!self->snapshot_da_info){
                self->snapshot_da_info = malloc(sizeof(struct http_da_info));
                memset (self->snapshot_da_info, 0, sizeof(struct http_da_info));
            } else {
                http_da_release(soap, self->snapshot_da_info);
            }

            char * user = OnvifCredentials__get_username(OnvifDevice__get_credentials(OnvifBaseService__get_device(self->parent)));
            char * pass = OnvifCredentials__get_password(OnvifDevice__get_credentials(OnvifBaseService__get_device(self->parent)));
            http_da_save(soap, self->snapshot_da_info, soap->authrealm, user, pass);
            free(user);
            free(pass);
            if ((soap_GET(soap, url, NULL) || soap_begin_recv(soap) || (buffer = soap_http_get_body(soap, &size)) != NULL || soap_end_recv(soap)) && soap->error == SOAP_OK){
                snap = OnvifSnapshot__create(size,buffer);
            }
        }
    }


    //Extracting original error message to print error associated with the returned URL, not the fallback ones
    const char * original_fstr = NULL;
    const char * original_fdtl = NULL;
    if ((soap->error != SOAP_OK && soap->error != SOAP_UDP_ERROR && soap->error != SOAP_TCP_ERROR) || !retry_flag) {
        //TODO handle error codes
        original_fstr = *soap_faultstring(soap);
        original_fdtl = *soap_faultdetail(soap);
    }

    //Root url fallback
    if (!retry_flag && (soap->error == SOAP_UDP_ERROR || soap->error == SOAP_TCP_ERROR)){
        char * new_endpoint = NULL;
        char * tmp_endpoint = NULL;
        char * master_host = OnvifDevice__get_host(OnvifBaseService__get_device(self->parent));
        char * master_port = OnvifDevice__get_port(OnvifBaseService__get_device(self->parent)); 
        tmp_endpoint = URL__set_port(url, master_port);
        new_endpoint = URL__set_host(tmp_endpoint, master_host);
        free(tmp_endpoint);
        free(master_host);
        free(master_port);
        if(strcmp(new_endpoint,url) != 0){ /* TODO compare port before constructing new URL */
            C_WARN("Connection error. Attempting to correct root URL : [%s] --> [%s]", url, new_endpoint);
            snap = get_http_body(self, soap, new_endpoint, 1);
        }
        free(new_endpoint);
        if(snap) goto exit;
    }

    //Port fallback
    if (!retry_flag && (soap->error == SOAP_UDP_ERROR || soap->error == SOAP_TCP_ERROR)){
        char * new_endpoint = NULL;
        char * master_port = OnvifDevice__get_port(OnvifBaseService__get_device(self->parent)); 
        new_endpoint = URL__set_port(url, master_port);
        if(strcmp(new_endpoint,url) != 0){ /* TODO compare port before constructing new URL */
            C_WARN("Connection error. Attempting to correct URL port : [%s] --> [%s]", url, new_endpoint);
            snap = get_http_body(self, soap, new_endpoint, 1);
        }
        free(new_endpoint);
        free(master_port);
        if(snap) goto exit;
    }

    //Hostname fallback
    if (!retry_flag && (soap->error == SOAP_UDP_ERROR || soap->error == SOAP_TCP_ERROR)){
        char * new_endpoint = NULL;
        char * master_host = OnvifDevice__get_host(OnvifBaseService__get_device(self->parent)); 
        new_endpoint = URL__set_host(url, master_host);
        if(strcmp(new_endpoint,url) != 0){ /* TODO compare port before constructing new URL */
            C_WARN("Connection error. Attempting to correct URL host : [%s] --> [%s]", url, new_endpoint);
            snap = get_http_body(self, soap, new_endpoint, 1);
        }
        free(new_endpoint);
        free(master_host);
        if(snap) goto exit;
    }

    if ((soap->error != SOAP_OK && soap->error != SOAP_UDP_ERROR && soap->error != SOAP_TCP_ERROR) || !retry_flag) {
        C_ERROR("[%s] Failed to retrieve snapshot: %s [%s][%d]", url, original_fstr,original_fdtl,soap->error);
    }

exit:
    return snap;
}

OnvifSnapshot * OnvifMediaService__getSnapshot(OnvifMediaService *self, int profile_index){
    char * endpoint = OnvifMediaService__get_endpoint(self);
    C_DEBUG("[%s] OnvifMediaService__getSnapshot",endpoint);
    //TODO check if GetSnapshotUri is support via GetServiceCapabilities
    char * snapshot_uri = OnvifMediaService__getSnapshotUri(self, profile_index);
    OnvifSnapshot * ret = NULL;
    if(!snapshot_uri){
        C_WARN("[%s] No snapshot URI found.",endpoint);
        goto exit;
    }
    C_INFO("[%s] Snapshot URI : %s\n",endpoint,snapshot_uri);

    //Creating soap instance to avoid long running locks.
    struct soap * soap = soap_new2(SOAP_IO_CHUNK, SOAP_IO_DEFAULT); //SOAP_XML_STRICT may cause crash - SOAP_IO_CHUNK doesnt support HTTP auth challenge?
    soap->connect_timeout = 2; // 2 sec
    soap->recv_timeout = soap->send_timeout = 10;//10 sec
    soap_register_plugin(soap, http_da);

    char *debug_flag = NULL;

    if (( debug_flag =getenv( "SNAP_DEBUG" )) != NULL )
        C_INFO("SNAP_DEBUG (using io chunk) variable set. '%s'",debug_flag) ;

    if(debug_flag){
        soap_register_plugin(soap, logging);
        soap_set_logging_outbound(soap,stdout);
        soap_set_logging_inbound(soap,stdout);
    }

    if (self->snapshot_da_info){
        http_da_restore(soap, self->snapshot_da_info);
    }

    ret = get_http_body(self, soap, snapshot_uri, 0);

    soap_closesock(soap);
    soap_destroy(soap);
    soap_end(soap);
    soap_done(soap);
    soap_free(soap);

    free(snapshot_uri);

exit:
    free(endpoint);
    return ret;
}

void OnvifMediaService__get_profile_token(OnvifMediaService *self, int index, char * ret){
    char * endpoint = OnvifMediaService__get_endpoint(self);
    if(!self){
        C_ERROR("[%s] OnvifMediaService__get_profile_token - MediaService uninitialized.",endpoint);
        ret[0] = '\0'; 
        goto exit;
    }
    P_MUTEX_LOCK(self->profile_lock);
    C_TRACE("[%s] OnvifMediaService__get_profile_token %i", endpoint, index);
    if(!OnvifMediaService__check_profiles(self)) { 
        ret[0] = '\0'; 
        goto unlock_exit;
    }

    int profileCount = OnvifProfiles__get_size(self->profiles);
    if(index >= profileCount){
        C_ERROR("[%s] OnvifMediaService__get_profile_token : profile index out-of-bounds.", endpoint);
    } else if(profileCount > 0 && profileCount > index) {
        OnvifProfile * profile = OnvifProfiles__get_profile(self->profiles,index);
        char * token = OnvifProfile__get_token(profile);
        if(token){
            C_TRACE("[%s] OnvifMediaService__get_profile_token : Found profile [%s]",endpoint, token);
            strcpy(ret,token);
        } else {
            C_ERROR("[%s] OnvifMediaService__get_profile_token : Found profile, but token was NULL", endpoint);
        }
    } else {
        C_ERROR("[%s] OnvifMediaService__get_profile_token : profile not found", endpoint);
    }
unlock_exit:
    P_MUTEX_UNLOCK(self->profile_lock);
exit:
    free(endpoint);
}