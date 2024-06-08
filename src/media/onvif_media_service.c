#include "onvif_media_service.h"
#include "onvif_media_profile_local.h"
#include "onvif_base_service_local.h"
#include "httpda.h"
#include "plugin/logging.h"
#include "clogger.h"
#include "url_parser.h"
#include "onvif_media_service_capabilities_local.h"

typedef struct _OnvifMediaService {
    OnvifBaseService * parent;
    OnvifProfiles * profiles;
    OnvifMediaServiceCapabilities * capabilities;
    P_MUTEX_TYPE profile_lock;
    P_MUTEX_TYPE caps_lock;
} OnvifMediaService;

OnvifMediaServiceCapabilities * OnvifMediaService__getServiceCapabilities_private(OnvifMediaService* self);

OnvifMediaService * OnvifMediaService__create(OnvifDevice * device, const char * endpoint, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    OnvifMediaService * self = malloc(sizeof(OnvifMediaService));
    OnvifMediaService__init(self,device, endpoint, error_cb, error_data);

    OnvifMediaService__getServiceCapabilities(self);
    if(!self->capabilities){
        OnvifMediaService__destroy(self);
        self = NULL;
    }
    return self;
}

void OnvifMediaService__init(OnvifMediaService * self, OnvifDevice * device, const char * endpoint, void (*error_cb)(OnvifErrorTypes type, void * user_data), void * error_data){
    self->parent = OnvifBaseService__create(device, endpoint, error_cb, error_data);
    P_MUTEX_SETUP(self->profile_lock);
    P_MUTEX_SETUP(self->caps_lock);
    self->profiles = NULL;
    self->capabilities = NULL;
}

void OnvifMediaService__destroy(OnvifMediaService * self){
    if(self){
        OnvifBaseService__destroy(self->parent);
        OnvifProfiles__destroy(self->profiles);
        // OnvifMediaServiceCapabilities__destroy(self->capabilities);
        if(self->capabilities)
            g_object_unref(self->capabilities);
        P_MUTEX_CLEANUP(self->profile_lock);
        P_MUTEX_CLEANUP(self->caps_lock);
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

OnvifMediaServiceCapabilities * OnvifMediaService__getServiceCapabilities_private(OnvifMediaService* self){

    struct _trt__GetServiceCapabilities req;
    struct _trt__GetServiceCapabilitiesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifMediaServiceCapabilities * retval = NULL;
    ONVIF_INVOKE_SOAP_CALL(self, trt__GetServiceCapabilities, OnvifMediaServiceCapabilities__new, retval, soap, NULL, &req,  &resp);
    
    return retval;

}

OnvifMediaServiceCapabilities * OnvifMediaService__getServiceCapabilities(OnvifMediaService* self){
    P_MUTEX_LOCK(self->caps_lock);
    if(!self->capabilities){
        self->capabilities = OnvifMediaService__getServiceCapabilities_private(self);
    }
    P_MUTEX_UNLOCK(self->caps_lock);
    return self->capabilities;
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
    if(!self || !self->capabilities){
        C_ERROR("[%s] OnvifMediaService__getSnapshotUri - MediaService uninitialized.",endpoint);
        goto exit;
    }

    //Service Capabilities Check
    // if(!OnvifMediaServiceCapabilities__get_snapshot_uri(self->capabilities)){
    //     C_INFO("[%s] OnvifMediaService__getSnapshotUri - MediaService doesn't support snapshots.",endpoint);
    //     goto exit;
    // }

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


//TODO Placeholder until user-input is implemented
int ssl_verify_callback(int ok, X509_STORE_CTX *store)
{
    if (!ok){
        int err = X509_STORE_CTX_get_error(store);
        switch (err){
            case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
            case X509_V_ERR_UNABLE_TO_GET_CRL:
            case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
                X509_STORE_CTX_set_error(store, X509_V_OK);
                ok = 1;
                break;
            default:
                C_ERROR("SSL ERROR : %d",err);
                break;
        }
    }
    return ok;
}

void send_http_get(struct soap * soap, char * url, char ** buffer, size_t * size){
    if(soap_GET(soap, url, NULL) != SOAP_OK){
        C_ERROR("Failed get");
        return;
    }

    if(soap_begin_recv(soap) != SOAP_OK){
        C_ERROR("Failed begin recv");
        return;
    }

    /*
    * In the case of "https://github.com/roleoroleo/yi-hack-Allwinner-v2", No "Content-Length" is set
    *  and "Transfer-Encoding" isn't set to "chunked"
    **/
    if(soap->mode != SOAP_IO_CHUNK && soap->length == 0){
        C_WARN("No content-length set. Forcing buffer flush.");
        //Cheat to force reading socket until server closes it
        soap->length = soap->recv_maxlength;
    }

    if((*buffer = soap_http_get_body(soap, size)) == NULL){
        C_ERROR("Failed get body recv");
        return;
    }

    if(soap_end_recv(soap) != SOAP_OK){
        C_ERROR("Failed end recv");
    }

//  return soap_GET(soap, url, NULL) || 
//      soap_begin_recv(soap) || 
//      (*buffer = soap_http_get_body(soap, size)) != NULL || 
//      soap_end_recv(soap);
}

OnvifSnapshot * get_http_body(OnvifMediaService *self, char * url)
{   
    size_t size = 0;
    char * buffer = NULL;
    OnvifSnapshot * snap = NULL;
    int redirect_count = 0;
    int retry_flag = 0;
    char * new_endpoint = NULL;

    //Creating seperate soap instance to avoid long running locks.
    struct soap * soap = soap_new1(SOAP_IO_DEFAULT); //SOAP_XML_STRICT may cause crash - SOAP_IO_CHUNK doesnt support HTTP auth challenge?
    soap->connect_timeout = 2; // 2 sec
    soap->recv_timeout = soap->send_timeout = 10;//10 sec
    soap_register_plugin(soap, http_da);

    soap_ssl_client_context(soap,
        SOAP_SSL_SKIP_HOST_CHECK,
        NULL,          /* no keyfile */
        NULL,          /* no keyfile password */
        NULL,  /* self-signed certificate cacert.pem */
        NULL,          /* no capath to trusted certificates */
        NULL           /* no random data to seed randomness */
    );
    soap->fsslverify = ssl_verify_callback;

    char *debug_flag = NULL;

    if (( debug_flag =getenv( "SNAP_DEBUG" )) != NULL )
        C_INFO("SNAP_DEBUG (using io default) variable set. '%s'",debug_flag) ;

    if(debug_flag){
        soap_register_plugin(soap, logging);
        soap_set_logging_outbound(soap,stdout);
        soap_set_logging_inbound(soap,stdout);
    }

    struct http_da_info snapshot_da_info;
    memset (&snapshot_da_info, 0, sizeof(struct http_da_info));

retry:
    send_http_get(soap,url,&buffer,&size);
    if(soap->error == SOAP_OK){ //Successfully
        if(size > 0){
            snap = OnvifSnapshot__create(size,buffer);
        } else {
            C_ERROR("[%s] Device returned successful empty snapshot response.",url);
        }
    } else if (soap->error == 401){ //HTTP authentication challenge
        C_DEBUG("[%s] Snapshot WWW-Authorization challenge '%s'",url, soap->authrealm);

        char * user = OnvifCredentials__get_username(OnvifDevice__get_credentials(OnvifBaseService__get_device(self->parent)));
        char * pass = OnvifCredentials__get_password(OnvifDevice__get_credentials(OnvifBaseService__get_device(self->parent)));
        http_da_save(soap, &snapshot_da_info, soap->authrealm, user, pass);
        send_http_get(soap,url,&buffer,&size);
        if(size > 0){
            snap = OnvifSnapshot__create(size,buffer);
        } else {
            C_ERROR("[%s] Device returned successful empty snapshot response.",url);
        }
        free(user);
        free(pass);
    }

    if(redirect_count < 10 && soap->error >= 301 && soap->error <= 308 && strncmp(url, soap->endpoint, 6)){
        if(new_endpoint){
            free(new_endpoint);
            new_endpoint = NULL;
        }
        //302 = contruct new uri
        if(soap->error == 302){
            ParsedURL * purl = ParsedURL__create(url);
            ParsedURL * npurl = ParsedURL__create(soap->endpoint);
            ParsedURL__set_host(purl,ParsedURL__get_host(npurl));
            ParsedURL__set_port(purl,ParsedURL__get_port(npurl));
            ParsedURL__set_protocol(purl,ParsedURL__get_protocol(npurl));
            new_endpoint = ParsedURL__toString(purl);
            ParsedURL__destroy(purl);
            ParsedURL__destroy(npurl);
            url = new_endpoint;
        } else {
            url = soap->endpoint;
        }

        redirect_count++;
        C_WARN("Redirecting to : %s [%d]",url, soap->error);
        goto retry;
    }
    
    //Root url fallback
    if (retry_flag == 0 && (soap->error == SOAP_UDP_ERROR || soap->error == SOAP_TCP_ERROR)){
        if(new_endpoint){
            free(new_endpoint);
            new_endpoint = NULL;
        }

        retry_flag = 1;
        char * tmp_endpoint = NULL;
        char * master_host = OnvifDevice__get_host(OnvifBaseService__get_device(self->parent));
        char * master_port = OnvifDevice__get_port(OnvifBaseService__get_device(self->parent)); 
        tmp_endpoint = URL__set_port(url, master_port);
        new_endpoint = URL__set_host(tmp_endpoint, master_host);
        free(tmp_endpoint);
        free(master_host);
        free(master_port);
        if(strcmp(new_endpoint,url) != 0){ /* TODO compare port before constructing new URL */
            C_WARN("[%s] Attempting to correct root URL to [%s]", url, new_endpoint);
            url = new_endpoint;
            goto retry;
        }
    }
    
    //Port fallback
    if (retry_flag == 1 && (soap->error == SOAP_UDP_ERROR || soap->error == SOAP_TCP_ERROR)){
        if(new_endpoint){
            free(new_endpoint);
            new_endpoint = NULL;
        }

        retry_flag = 2;
        char * master_port = OnvifDevice__get_port(OnvifBaseService__get_device(self->parent)); 
        new_endpoint = URL__set_port(url, master_port);
        free(master_port);
        if(strcmp(new_endpoint,url) != 0){ /* TODO compare port before constructing new URL */
            C_WARN("[%s] Attempting to correct URL port to [%s]", url, new_endpoint);
            url = new_endpoint;
            goto retry;
        }
    }
    
    //Hostname fallback
    if (retry_flag == 2 && (soap->error == SOAP_UDP_ERROR || soap->error == SOAP_TCP_ERROR)){
        if(new_endpoint){
            free(new_endpoint);
            new_endpoint = NULL;
        }

        retry_flag = 3;
        char * master_host = OnvifDevice__get_host(OnvifBaseService__get_device(self->parent)); 
        new_endpoint = URL__set_host(url, master_host);
        free(master_host);
        if(strcmp(new_endpoint,url) != 0){ /* TODO compare port before constructing new URL */
            C_WARN("[%s] Attempting to correct URL host : to [%s]", url, new_endpoint);
            url = new_endpoint;
            goto retry;
        }
    }

    //TODO SOAP_SSL_ERROR
    if (soap->error != SOAP_OK) {
        if(redirect_count >= 10){
            C_ERROR("[%s] Failed to retrieve snapshot: %s [Too many redirects (max: 10)]", url);
        } else {
            C_ERROR("[%s] Failed to retrieve snapshot: %s [%s][%d]", url, *soap_faultstring(soap),*soap_faultdetail(soap),soap->error);
        }
    }
    
    if(new_endpoint){
        free(new_endpoint);
        new_endpoint = NULL;
    }
    
    //Close connection and clean up before attempting fallback
    http_da_release(soap, &snapshot_da_info);
    soap_closesock(soap);
    soap_destroy(soap);
    soap_end(soap);
    soap_done(soap);
    soap_free(soap);

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

    ret = get_http_body(self, snapshot_uri);

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