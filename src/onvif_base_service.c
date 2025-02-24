#include "onvif_base_service_local.h"
#include "generated/onvifsoap.nsmap"
#include "wsseapi.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "clogger.h"
#include "portable_thread.h"

enum {
  PROP_DEVICE = 1,
  PROP_URI = 2,
  N_PROPERTIES
};

typedef struct {
    OnvifDevice * device;
    char * endpoint;
    P_MUTEX_TYPE service_lock;
    P_MUTEX_TYPE prop_lock;
    struct http_da_info *  da_info;
} OnvifBaseServicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (OnvifBaseService, OnvifBaseService_, G_TYPE_OBJECT)
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
OnvifBaseService__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec){
    OnvifBaseService * self = ONVIF_BASE_SERVICE (object);
    OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (self);
    switch (prop_id){
        case PROP_DEVICE:
            priv->device = g_value_get_pointer (value);
            break;
        case PROP_URI:
            OnvifBaseService__set_endpoint(self,(char*)g_value_get_string (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifBaseService__get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec){
    // OnvifBaseService *thread = ONVIF_BASE_SERVICE (object);
    // OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (thread);
    switch (prop_id){
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

void
http_da_cleanup(struct http_da_info * da_info){
    if(da_info->authrealm){
        free(da_info->authrealm);
        da_info->authrealm = NULL;
    }
    if(da_info->userid){
        free(da_info->userid);
        da_info->userid = NULL;
    }
    if(da_info->passwd){
        free(da_info->passwd);
        da_info->passwd = NULL;
    }
    if(da_info->nonce){
        free(da_info->nonce);
        da_info->nonce = NULL;
    }
    if(da_info->opaque){
        free(da_info->opaque);
        da_info->opaque = NULL;
    }
    if(da_info->qop){
        free(da_info->qop);
        da_info->qop = NULL;
    }
    if(da_info->alg){
        free(da_info->alg);
        da_info->alg = NULL;
    }
}

static void
OnvifBaseService__dispose (GObject *object){
    OnvifBaseService *self = ONVIF_BASE_SERVICE (object);
    OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (self);
    if(priv->endpoint){
        free(priv->endpoint);
        priv->endpoint = NULL;
    }

    if(priv->da_info){
        http_da_cleanup(priv->da_info);
        free(priv->da_info);
        priv->da_info=NULL;
    }

    P_MUTEX_CLEANUP(priv->service_lock);
    P_MUTEX_CLEANUP(priv->prop_lock);

    G_OBJECT_CLASS (OnvifBaseService__parent_class)->dispose (object);
}


static void
OnvifBaseService__class_init (OnvifBaseServiceClass *klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifBaseService__dispose;
    object_class->set_property = OnvifBaseService__set_property;
    object_class->get_property = OnvifBaseService__get_property;

    //TODO Replace with gtype
    // obj_properties[PROP_DEVICE] =
    //     g_param_spec_object ("device",
    //                         "OnvifDevice",
    //                         "Pointer to OnvifDevice parent.",
    //                         ONVIFMGR_TYPE_APP  /* default value */,
    //                         G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

    obj_properties[PROP_DEVICE] =
        g_param_spec_pointer ("device",
                            "OnvifDevice",
                            "Pointer to OnvifDevice parent.",
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);

    obj_properties[PROP_URI] =
        g_param_spec_string ("uri",
                            "Service URI",
                            "Pointer to URI char*.",
                            NULL,
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);

    g_object_class_install_properties (object_class,
                                        N_PROPERTIES,
                                        obj_properties);
}

static void
OnvifBaseService__init (OnvifBaseService *self){
    OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (self);
    priv->device = NULL;
    priv->endpoint = NULL;
    priv->da_info = NULL;
    
    P_MUTEX_SETUP(priv->service_lock);

    P_MUTEX_SETUP(priv->prop_lock);
}

void 
OnvifBaseService__set_endpoint(OnvifBaseService * self, char * endpoint){
    OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (self);
    P_MUTEX_LOCK(priv->prop_lock);
    if(!priv->endpoint){
        priv->endpoint = malloc(strlen(endpoint)+1);
    } else {
        priv->endpoint = realloc(priv->endpoint, strlen(endpoint)+1);
    }
    strcpy(priv->endpoint, endpoint);
    P_MUTEX_UNLOCK(priv->prop_lock);
}

void 
OnvifBaseService__lock(OnvifBaseService * self){
    OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (self);
    P_MUTEX_LOCK(priv->service_lock);
}

void 
OnvifBaseService__unlock(OnvifBaseService * self){
    OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (self);
    P_MUTEX_UNLOCK(priv->service_lock);
}

char * 
OnvifBaseService__get_endpoint(OnvifBaseService * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_BASE_SERVICE (self), NULL);

    OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (self);

    char * ret = NULL;
    P_MUTEX_LOCK(priv->prop_lock);
    ret = malloc(strlen(priv->endpoint)+1);
    strcpy(ret,priv->endpoint);
    P_MUTEX_UNLOCK(priv->prop_lock);
    return ret;
}

OnvifDevice * 
OnvifBaseService__get_device(OnvifBaseService * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_BASE_SERVICE (self), NULL);

    OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (self);
    return priv->device;
}

time_t 
OnvifBaseService__get_offset_time(OnvifBaseService * self){
    OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (self);
    double offset = OnvifDevice__getTimeOffset(priv->device);

    time_t now = time( NULL);
    if(offset == 0){
        C_TRACE("[%s] No time adjustment required. Camera in sync with client.",priv->endpoint);
        return now;
    }

    struct tm now_tm = *localtime( &now);
    now_tm.tm_sec += offset;

    time_t ntime = mktime(&now_tm);

    char buff1[20];
    strftime(buff1, 20, "%Y-%m-%d %H:%M:%S", gmtime(&now));

    char buff2[20];
    strftime(buff2, 20, "%Y-%m-%d %H:%M:%S", gmtime(&ntime));

    C_TRACE("[%s] Time adjustment [%s] to [%s] offset %.0lf", priv->endpoint, buff1, buff2, offset);
    return ntime;
}

int 
OnvifBaseService__set_wsse_data(OnvifBaseService * self, SoapDef * soap){
    OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (self);
    int ret = 1;
    char * user = OnvifCredentials__get_username(OnvifDevice__get_credentials(priv->device));
    char * pass = OnvifCredentials__get_password(OnvifDevice__get_credentials(priv->device));
    soap_wsse_delete_Security(soap);
    if(user && pass){
        if (soap_wsse_add_Timestamp(soap, "Time", 10)
            || soap_wsse_add_UsernameTokenDigest_at(soap, "Auth",user,pass,OnvifBaseService__get_offset_time(self))){ //TODO Set camera time offset
            //TODO Error handling
            C_ERROR("Unable to set wsse creds...\n");
            ret = 0;
        }
    }

    //TODO support encryption
    free(user);
    free(pass);
    return ret;
}

int 
OnvifBaseService__http_challenge(OnvifBaseService * self, SoapDef * soap, char * url){
    if(!soap->authrealm){
        return 0;
    }
    
    OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (self);
    C_DEBUG("[%s] WWW-Authorization challenge '%s'",url,soap->authrealm);

    char * user = OnvifCredentials__get_username(OnvifDevice__get_credentials(priv->device));
    char * pass = OnvifCredentials__get_password(OnvifDevice__get_credentials(priv->device));
    if(!priv->da_info){
        priv->da_info = malloc(sizeof(struct http_da_info));
        memset (priv->da_info, 0, sizeof(struct http_da_info));
    } else {
        http_da_cleanup(priv->da_info);
    }
    http_da_save(soap, priv->da_info, soap->authrealm, user, pass);
    free(user);
    free(pass);
    //Reapply wsse data after challenge started
    int wsseret = OnvifBaseService__set_wsse_data(self,soap);
    if(!wsseret){
        OnvifBaseService__soap_destroy(self, soap);
        return 0;
    }

    return 1;
}

SoapDef * 
OnvifBaseService__soap_new(OnvifBaseService * self){
    OnvifBaseServicePrivate *priv = OnvifBaseService__get_instance_private (self);
    struct soap * soap = soap_new1(SOAP_XML_CANONICAL | SOAP_C_UTFSTRING); //SOAP_XML_STRICT may cause crash
    soap->connect_timeout = 2; // 2 sec
    soap->recv_timeout = soap->send_timeout = 10;//10 sec
    soap_register_plugin(soap, soap_wsse);
    soap_register_plugin(soap, http_da);
    soap_set_namespaces(soap, onvifsoap_namespaces);

    soap_ssl_client_context(soap,
        SOAP_SSL_SKIP_HOST_CHECK,
        NULL,          /* no keyfile */
        NULL,          /* no keyfile password */
        NULL,  /* self-signed certificate cacert.pem */
        NULL,          /* no capath to trusted certificates */
        NULL           /* no random data to seed randomness */
    );
    soap->fsslverify = OnvifBaseService__ssl_verify_callback;

    if(priv->da_info)
        http_da_restore(soap, priv->da_info);
        
    int wsseret = OnvifBaseService__set_wsse_data(self,soap);

    if(!wsseret){
        OnvifBaseService__soap_destroy(self, soap);
        soap = NULL;
    }


    char *debug_flag = NULL;

    if (( debug_flag =getenv( "ONVIF_DEBUG" )) != NULL )
        C_INFO("ONVIF_DEBUG variable set. '%s'",debug_flag) ;

    if(debug_flag){
        soap_register_plugin(soap, logging);
        soap_set_logging_outbound(soap,stdout);
        soap_set_logging_inbound(soap,stdout);
    }


    return (SoapDef *) soap;
}

void 
OnvifBaseService__soap_destroy(OnvifBaseService * self, SoapDef * soap){
    char *debug_flag = NULL;

    if (( debug_flag =getenv( "ONVIF_DEBUG" )) != NULL )
        C_INFO("ONVIF_DEBUG variable set. '%s'",debug_flag) ;

    if(debug_flag){
        soap_set_logging_outbound(soap,NULL);
        soap_set_logging_inbound(soap,NULL);
    }

    soap_wsse_delete_Security(soap);
    soap_wsse_verify_done(soap);
    soap_destroy(soap);
    soap_end(soap);
    soap_done(soap);
    soap_free(soap);
}

//TODO Placeholder until user-input is implemented
int 
OnvifBaseService__ssl_verify_callback(int ok, X509_STORE_CTX *store){
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

void 
OnvifBaseService__print_fault(struct soap *soap){
  if (soap_check_state(soap)){
    C_ERROR("Error: soap struct state not initialized\n");
  } else if (soap->error) {
    const char **c, *v = NULL, *s, *d;
    c = soap_faultcode(soap);
    if (!*c)
    {
      soap_set_fault(soap);
      c = soap_faultcode(soap);
    }
    if (soap->version == 2)
      v = soap_fault_subcode(soap);
    s = soap_fault_string(soap);
    d = soap_fault_detail(soap);
    C_ERROR("%s%d fault %s [%s]\n\"%s\"\nDetail: %s\n", soap->version ? "SOAP 1." : "Error ", soap->version ? (int)soap->version : soap->error, *c, v ? v : "no subcode", s ? s : "[no reason]", d ? d : "[no detail]");
  }
}