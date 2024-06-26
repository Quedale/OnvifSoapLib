#include "../onvif_base_service_local.h"
#include "onvif_media_profile_local.h"
#include "onvif_media_snapshot_local.h"
#include "onvif_media_uri_local.h"
#include "onvif_media_service_capabilities_local.h"
#include "portable_thread.h"
#include "clogger.h"
#include "onvif_media_service.h"

typedef struct {
    OnvifMediaProfiles * profiles;
    P_MUTEX_TYPE profile_lock;
} OnvifMediaServicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifMediaService, OnvifMediaService_, ONVIF_TYPE_BASE_SERVICE)

static void
OnvifMediaService__dispose (GObject *self)
{
    OnvifMediaServicePrivate *priv = OnvifMediaService__get_instance_private (ONVIF_MEDIA_SERVICE(self));

    if(priv->profiles){
        g_object_unref(priv->profiles);
        priv->profiles = NULL;
    }
    P_MUTEX_CLEANUP(priv->profile_lock);

    G_OBJECT_CLASS (OnvifMediaService__parent_class)->dispose (self);
}

static void
OnvifMediaService__class_init (OnvifMediaServiceClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifMediaService__dispose;
}

static void
OnvifMediaService__init (OnvifMediaService * self)
{
    OnvifMediaServicePrivate *priv = OnvifMediaService__get_instance_private (ONVIF_MEDIA_SERVICE(self));
    P_MUTEX_SETUP(priv->profile_lock);
    priv->profiles = NULL;
}


OnvifMediaService* OnvifMediaService__new(OnvifDevice * device, const char * endpoint){
    return g_object_new (ONVIF_TYPE_MEDIA_SERVICE, "device", device, "uri", endpoint, NULL);
}

////
SoapFault OnvifMediaService__check_profiles(OnvifMediaService * self){
    OnvifMediaServicePrivate *priv = OnvifMediaService__get_instance_private (ONVIF_MEDIA_SERVICE(self));
    if(!priv->profiles || *SoapObject__get_fault(SOAP_OBJECT(priv->profiles)) != SOAP_FAULT_NONE){
        if(priv->profiles) g_object_unref(priv->profiles);
        priv->profiles = OnvifMediaService__getProfiles(self);
    }

    return *SoapObject__get_fault(SOAP_OBJECT(priv->profiles));
}

OnvifMediaProfiles * OnvifMediaService__get_profiles(OnvifMediaService * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_SERVICE (self), NULL);
    
    OnvifMediaServicePrivate *priv = OnvifMediaService__get_instance_private (ONVIF_MEDIA_SERVICE(self));
    P_MUTEX_LOCK(priv->profile_lock);
    OnvifMediaService__check_profiles(self);
    g_object_ref(priv->profiles);//Adding ref on behalf of the caller
    P_MUTEX_UNLOCK(priv->profile_lock);
    return priv->profiles;
}

OnvifMediaServiceCapabilities * OnvifMediaService__getServiceCapabilities(OnvifMediaService* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_SERVICE (self), NULL);
    
    struct _trt__GetServiceCapabilities req;
    struct _trt__GetServiceCapabilitiesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifMediaServiceCapabilities * retval = NULL;
    ONVIF_INVOKE_SOAP_CALL(self, trt__GetServiceCapabilities, OnvifMediaServiceCapabilities__new, retval, soap, NULL, &req,  &resp);
    
    return retval;

}

OnvifMediaProfiles * OnvifMediaService__getProfiles(OnvifMediaService* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_SERVICE (self), NULL);
    
    struct _trt__GetProfiles req;
    struct _trt__GetProfilesResponse resp;
    memset (&req, 0, sizeof (req));
    memset (&resp, 0, sizeof (resp));

    OnvifMediaProfiles * profiles = NULL;
    
    ONVIF_INVOKE_SOAP_CALL(self, trt__GetProfiles, OnvifMediaProfiles__new, profiles, soap, NULL, &req,  &resp);
    
    return profiles;
}

OnvifMediaUri * OnvifMediaService__getStreamUri(OnvifMediaService* self, int profile_index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_SERVICE (self), NULL);
    
    OnvifMediaUri * streamuri = NULL;

    char token[255];
    memset(&token,0,sizeof(token));
    SoapFault fault = OnvifMediaService__get_profile_token(self,profile_index, token);
    if(fault != SOAP_FAULT_NONE){
        C_WARN("Profile token not found %d.",fault); 
        streamuri = OnvifStreamUri__new(NULL);
        SoapObject__set_fault(SOAP_OBJECT(streamuri),fault);
        goto exit;
    }

    if(strlen(token) == 0) { 
        C_WARN("Profile token not found."); 
        streamuri = OnvifStreamUri__new(NULL);
        SoapObject__set_fault(SOAP_OBJECT(streamuri),SOAP_FAULT_UNEXPECTED);
        goto exit;
    }

    struct _trt__GetStreamUri req;
    struct _trt__GetStreamUriResponse resp;

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
    ONVIF_INVOKE_SOAP_CALL(self, trt__GetStreamUri, OnvifStreamUri__new, streamuri, soap, NULL, &req,  &resp);

exit:
    return streamuri;
}


OnvifMediaUri * OnvifMediaService__getSnapshotUri(OnvifMediaService *self, int profile_index){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_SERVICE (self), NULL);

    char * endpoint = OnvifBaseService__get_endpoint(ONVIF_BASE_SERVICE(self));
    OnvifMediaUri * ret_val = NULL;

    struct _trt__GetSnapshotUri request;
    struct _trt__GetSnapshotUriResponse response;
    memset (&request, 0, sizeof (request));
    memset (&response, 0, sizeof (response));

    char token[255];
    SoapFault fault = OnvifMediaService__get_profile_token(self,profile_index, token);
    if(fault != SOAP_FAULT_NONE){
        C_WARN("[%s] Profile token not found %d.",endpoint, fault); 
        ret_val = OnvifSnapshotUri__new(NULL);
        SoapObject__set_fault(SOAP_OBJECT(ret_val),fault);
        goto exit;
    }

    if(strlen(token) == 0) { 
        C_WARN("[%s] Profile token not found.", endpoint); 
        ret_val = OnvifSnapshotUri__new(NULL);
        SoapObject__set_fault(SOAP_OBJECT(ret_val),SOAP_FAULT_UNEXPECTED);
        goto exit;
    }

    request.ProfileToken = token;

    ONVIF_INVOKE_SOAP_CALL(self, trt__GetSnapshotUri, OnvifSnapshotUri__new, ret_val, soap, NULL, &request,  &response);

exit:
    free(endpoint);
    return ret_val;
}


void send_http_get(struct soap * soap, char * url, char ** buffer, size_t * size){
    if(soap_GET(soap, url, NULL) != SOAP_OK){
        C_ERROR("[%s] Failed GET getting snapshot", url);
        return;
    }

    if(soap_begin_recv(soap) != SOAP_OK){
        C_TRACE("[%s] Failed begin recv getting snapshot", url); //Happens on redirect, auth challenge, etc...
        return;
    }

    /*
    * In the case of "https://github.com/roleoroleo/yi-hack-Allwinner-v2", No "Content-Length" is set
    *  and "Transfer-Encoding" isn't set to "chunked"
    **/
    if(soap->mode != SOAP_IO_CHUNK && soap->length == 0){
        C_WARN("[%s] No content-length set getting snapshot. Forcing buffer flush.", url);
        //Cheat to force reading socket until server closes it
        soap->length = soap->recv_maxlength;
    }

    if((*buffer = soap_http_get_body(soap, size)) == NULL){
        C_ERROR("[%s] Failed get body recv getting snapshot", url);
        return;
    }

    if(soap_end_recv(soap) != SOAP_OK){
        C_ERROR("[%s] Failed end recv getting snapshot", url);
    }
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
    soap->fsslverify = OnvifBaseService__ssl_verify_callback;

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
            snap = OnvifSnapshot__new(size,buffer);
        } else {
            C_ERROR("[%s] Device returned successful empty snapshot response.",url);
        }
    } else if (soap->error == 401){ //HTTP authentication challenge
        C_DEBUG("[%s] Snapshot WWW-Authorization challenge '%s'",url, soap->authrealm);
        OnvifCredentials * creds = OnvifDevice__get_credentials(OnvifBaseService__get_device(ONVIF_BASE_SERVICE(self)));
        char * user = OnvifCredentials__get_username(creds);
        char * pass = OnvifCredentials__get_password(creds);
        http_da_save(soap, &snapshot_da_info, soap->authrealm, user, pass);
        send_http_get(soap,url,&buffer,&size);
        if(size > 0){
            snap = OnvifSnapshot__new(size,buffer);
        } else {
            C_ERROR("[%s] Device returned successful empty snapshot response.",url);
        }
        free(user);
        free(pass);
    }

    if(redirect_count < 10 && soap->error >= 301 && soap->error <= 308){
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

    OnvifDevice * dev = OnvifBaseService__get_device(ONVIF_BASE_SERVICE(self));
    //Root url fallback
    if (retry_flag == 0 && (soap->error == SOAP_UDP_ERROR || soap->error == SOAP_TCP_ERROR)){
        if(new_endpoint){
            free(new_endpoint);
            new_endpoint = NULL;
        }

        retry_flag = 1;
        char * tmp_endpoint = NULL;
        
        char * master_host = OnvifDevice__get_host(dev);
        char * master_port = OnvifDevice__get_port(dev); 
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
        char * master_port = OnvifDevice__get_port(dev); 
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
        char * master_host = OnvifDevice__get_host(dev); 
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
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_MEDIA_SERVICE (self), NULL);
    
    char * uri;
    char * endpoint = OnvifBaseService__get_endpoint(ONVIF_BASE_SERVICE(self));
    C_DEBUG("[%s] OnvifMediaService__getSnapshot",endpoint);
    /*
    * We don't check if GetSnapshotUri if is supported via GetServiceCapabilities
    * because some cameras like IMOU returns false, while still providing the capability
    */
    OnvifMediaUri * snapshot_uri = OnvifMediaService__getSnapshotUri(self, profile_index);
    SoapFault * fault = SoapObject__get_fault(SOAP_OBJECT(snapshot_uri));
    OnvifSnapshot * ret = NULL;
    switch(*fault){
        case SOAP_FAULT_NONE:
            uri = OnvifMediaUri__get_uri(snapshot_uri);
            if(!uri){
                C_WARN("[%s] No snapshot URI provided.",endpoint);
                ret = OnvifSnapshot__new(0,NULL);
                SoapObject__set_fault(SOAP_OBJECT(ret),*fault);
            } else {
                C_INFO("[%s] Snapshot URI : %s\n",endpoint,uri);
                ret = get_http_body(self, uri);
            }
            break;
        case SOAP_FAULT_ACTION_NOT_SUPPORTED:
        case SOAP_FAULT_CONNECTION_ERROR:
        case SOAP_FAULT_NOT_VALID:
        case SOAP_FAULT_UNAUTHORIZED:
        case SOAP_FAULT_UNEXPECTED:
        default:
            ret = OnvifSnapshot__new(0,NULL);
            SoapObject__set_fault(SOAP_OBJECT(ret),*fault);
            break;
    }

    g_object_unref(snapshot_uri);
    free(endpoint);
    return ret;
}

SoapFault OnvifMediaService__get_profile_token(OnvifMediaService *self, int index, char * ret){
    g_return_val_if_fail (self != NULL, SOAP_FAULT_UNEXPECTED);
    g_return_val_if_fail (ONVIF_IS_MEDIA_SERVICE (self), SOAP_FAULT_UNEXPECTED);

    OnvifMediaServicePrivate *priv = OnvifMediaService__get_instance_private (ONVIF_MEDIA_SERVICE(self));
    char * endpoint = OnvifBaseService__get_endpoint(ONVIF_BASE_SERVICE(self));

    SoapFault ret_fault = SOAP_FAULT_NONE;
    P_MUTEX_LOCK(priv->profile_lock);
    C_TRACE("[%s] OnvifMediaService__get_profile_token %i", endpoint, index);
    ret_fault = OnvifMediaService__check_profiles(self);
    if(ret_fault != SOAP_FAULT_NONE){
        ret[0] = '\0'; 
        goto exit;
    }

    int profileCount = OnvifMediaProfiles__get_size(priv->profiles);
    if(index >= profileCount){
        C_ERROR("[%s] OnvifMediaService__get_profile_token : profile index out-of-bounds.", endpoint);
    } else if(profileCount > 0 && profileCount > index) {
        OnvifProfile * profile = OnvifMediaProfiles__get_profile(priv->profiles,index);
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
exit:
    P_MUTEX_UNLOCK(priv->profile_lock);
    free(endpoint);
    return ret_fault;
}