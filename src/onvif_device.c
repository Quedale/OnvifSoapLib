#include "onvif_device.h"
// #include "ip_match.h"
#include "clogger.h"
#include "url_parser.h"
#include <stdlib.h>
#include "portable_thread.h"

#if defined(_WIN32) || defined(_WIN64)
/* We are on Windows */
# define strtok_r strtok_s
#endif

enum {
  PROP_URI = 1,
  N_PROPERTIES
};

typedef struct {
    ParsedURL * purl;
    time_t * datetime;
    double time_offset;
    P_MUTEX_TYPE auth_lock;
    int authenticated;

    P_MUTEX_TYPE prop_lock;
    
    OnvifDeviceService * device_service;
    OnvifDeviceIOService * deviceio_service;
    P_MUTEX_TYPE media_lock;
    OnvifMedia1Service * media1_service;
    OnvifMedia2Service * media2_service;

    OnvifCredentials * credentials;
} OnvifDevicePrivate ;

G_DEFINE_TYPE_WITH_PRIVATE (OnvifDevice, OnvifDevice_, G_TYPE_OBJECT)
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static SoapFault 
OnvifDevice__createServices(OnvifDevice* self){
    SoapFault ret_fault = SOAP_FAULT_NONE;
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    if(priv->media1_service){
        return ret_fault;
    }

    P_MUTEX_LOCK(priv->media_lock);

    //Check again in case it was created during lock
    if(priv->media1_service){
        goto exit;
    }

    ONVIF_DEVICE_TRACE("[%s] OnvifDevice__createServices",self);
    OnvifDeviceServices * services = OnvifDeviceService__getServices(priv->device_service, TRUE);
    SoapFault * caps_fault = SoapObject__get_fault(SOAP_OBJECT(services));
    OnvifService * service;
    switch(*caps_fault){
        case SOAP_FAULT_NONE:
            service = OnvifDeviceServices__get_media(services);
            if(service) priv->media1_service = OnvifMedia1Service__new(self, OnvifService__get_address(service));
            service = OnvifDeviceServices__get_media2(services);
            if(service) priv->media2_service = OnvifMedia2Service__new(self, OnvifService__get_address(service));
            service = OnvifDeviceServices__get_deviceio(services);
            if(service) priv->deviceio_service = OnvifDeviceIOService__new(self, OnvifService__get_address(service));
            break;
        case SOAP_FAULT_UNAUTHORIZED:
        case SOAP_FAULT_CONNECTION_ERROR:
            ONVIF_DEVICE_ERROR("[%s] Failed to retrieve device services...\n", self);
            ret_fault = *caps_fault;
            break;
        case SOAP_FAULT_ACTION_NOT_SUPPORTED:
        case SOAP_FAULT_NOT_VALID:
        case SOAP_FAULT_UNEXPECTED:
        default:
            ONVIF_DEVICE_ERROR("[%s] GetServices failed...\n", self);
            ONVIF_DEVICE_ERROR("[%s] Fallback on GetCapabilities...\n", self);
            OnvifMedia * media;
            OnvifDeviceIOCapabilities * deviceio;
            OnvifCapabilities* capabilities = OnvifDeviceService__getCapabilities(priv->device_service);
            caps_fault = SoapObject__get_fault(SOAP_OBJECT(capabilities));
            switch(*caps_fault){
                case SOAP_FAULT_NONE:
                    media = OnvifCapabilities__get_media(capabilities);
                    deviceio = OnvifCapabilities__get_deviceio(capabilities);
                    priv->media1_service = OnvifMedia1Service__new(self, OnvifMedia__get_address(media));
                    priv->media2_service = OnvifMedia2Service__new(self, OnvifMedia__get_address(media));
                    priv->deviceio_service = OnvifDeviceIOService__new(self, OnvifDeviceIOCapabilities__get_address(deviceio));
                    break;
                case SOAP_FAULT_ACTION_NOT_SUPPORTED:
                case SOAP_FAULT_CONNECTION_ERROR:
                case SOAP_FAULT_NOT_VALID:
                case SOAP_FAULT_UNAUTHORIZED:
                case SOAP_FAULT_UNEXPECTED:
                default:
                    ONVIF_DEVICE_ERROR("[%s] Failed to retrieve service URIs...", self);
                    ret_fault = *caps_fault;
                    break;
            }
            g_object_unref(capabilities);
            break;
    }
    g_object_unref(services);

exit:
    P_MUTEX_UNLOCK(priv->media_lock);
    return ret_fault;
}

time_t * 
OnvifDevice__getSystemDateTime(OnvifDevice * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), NULL);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    P_MUTEX_LOCK(priv->prop_lock);
    return priv->datetime;
    P_MUTEX_UNLOCK(priv->prop_lock);
}

double 
OnvifDevice__getTimeOffset(OnvifDevice * self){
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), 0);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    return priv->time_offset;
}

SoapFault 
OnvifDevice__authenticate(OnvifDevice* self){
    g_return_val_if_fail (self != NULL, SOAP_FAULT_UNEXPECTED);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), SOAP_FAULT_UNEXPECTED);
    ONVIF_DEVICE_INFO("[%s] OnvifDevice__authenticate",self);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    SoapFault ret_fault;

    P_MUTEX_LOCK(priv->auth_lock);
    if(priv->authenticated){
        ret_fault = SOAP_FAULT_NONE;
        goto exit;
    }

    if(!priv->datetime){
        OnvifDeviceDateTime * onviftime = OnvifDeviceService__getSystemDateAndTime(priv->device_service);
        time_t datetime;
        ret_fault = *SoapObject__get_fault(SOAP_OBJECT(onviftime));
        switch(ret_fault){
            case SOAP_FAULT_NONE:
                datetime = *OnvifDeviceDateTime__get_datetime(onviftime);
                if(datetime > 0){
                    P_MUTEX_LOCK(priv->prop_lock);
                    priv->time_offset = difftime(datetime,time(NULL));
                    priv->datetime = malloc(sizeof(time_t));
                    memcpy(priv->datetime, &datetime, sizeof(datetime));

                    char str_now[20];
                    strftime(str_now, 20, "%Y-%m-%d %H:%M:%S", localtime(priv->datetime));
                    ONVIF_DEVICE_INFO("[%s] Camera SystemDateAndTime : '%s'\n",self, str_now);

                    P_MUTEX_UNLOCK(priv->prop_lock);
                } else {
                    ONVIF_DEVICE_ERROR("[%s] Camera SystemDateAndTime not set\n",self);
                }
                g_object_unref(onviftime);
                break;
            case SOAP_FAULT_ACTION_NOT_SUPPORTED:
            case SOAP_FAULT_CONNECTION_ERROR:
            case SOAP_FAULT_NOT_VALID:
            case SOAP_FAULT_UNAUTHORIZED:
            case SOAP_FAULT_UNEXPECTED:
            default:
                ONVIF_DEVICE_ERROR("[%s] OnvifDevice__authenticate - Failed to retrieve SystemDateTime [%d]\n",self, ret_fault);
                g_object_unref(onviftime);
                goto exit;
        }
    }

    ret_fault = OnvifDevice__createServices(self);
    switch(ret_fault){
        case SOAP_FAULT_NONE:
            if(!priv->media1_service){
                ONVIF_DEVICE_ERROR("[%s] OnvifDevice__authenticate - Device doesn't support media service [%d]\n",self, ret_fault);
                ret_fault = SOAP_FAULT_UNEXPECTED;
                goto exit;
            }
            ONVIF_MEDIA_DEBUG("[%s] Successfully created Media soap",self);
            break;
        case SOAP_FAULT_ACTION_NOT_SUPPORTED:
        case SOAP_FAULT_CONNECTION_ERROR:
        case SOAP_FAULT_NOT_VALID:
        case SOAP_FAULT_UNAUTHORIZED:
        case SOAP_FAULT_UNEXPECTED:
        default:
            ONVIF_DEVICE_ERROR("[%s] OnvifDevice__authenticate - Failed to create media service [%d]\n",self, ret_fault);
            goto exit;
    }

    char * uri = NULL;
    OnvifMedia1Uri * media_uri = OnvifMedia1Service__getStreamUri(priv->media1_service,0);
    ret_fault = *SoapObject__get_fault(SOAP_OBJECT(media_uri));
    switch(ret_fault){
        case SOAP_FAULT_NONE:
            uri = OnvifUri__get_uri(ONVIF_URI(media_uri));
            if(!uri){
                ONVIF_MEDIA_ERROR("[%s] No StreamURI provided...",self);
                g_object_unref(priv->media1_service);
                priv->media1_service = NULL;
            } else {
                ONVIF_MEDIA_ERROR("[%s] StreamURI : %s",self, uri);
                P_MUTEX_LOCK(priv->prop_lock);
                priv->authenticated = 1;
                P_MUTEX_UNLOCK(priv->prop_lock);
            }
            break;
        case SOAP_FAULT_ACTION_NOT_SUPPORTED:
        case SOAP_FAULT_CONNECTION_ERROR:
        case SOAP_FAULT_NOT_VALID:
        case SOAP_FAULT_UNAUTHORIZED:
        case SOAP_FAULT_UNEXPECTED:
        default:
            ONVIF_MEDIA_ERROR("[%s] Failed to retrieve StreamURI...",self);
            g_object_unref(priv->media1_service);
            priv->media1_service = NULL;
            break;
    }
    
    g_object_unref(media_uri);

exit:
    P_MUTEX_UNLOCK(priv->auth_lock);
    return ret_fault;
}

int 
OnvifDevice__is_authenticated(OnvifDevice* self){
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), FALSE);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    int ret;
    P_MUTEX_LOCK(priv->auth_lock);
    ret = priv->authenticated;
    P_MUTEX_UNLOCK(priv->auth_lock);
    return ret;
}

void 
OnvifDevice__set_credentials(OnvifDevice* self,const char * user,const char* pass){
    g_return_if_fail (self != NULL);
    g_return_if_fail (ONVIF_IS_DEVICE (self));
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    P_MUTEX_LOCK(priv->prop_lock);
    OnvifCredentials__set_username(priv->credentials,user);
    OnvifCredentials__set_password(priv->credentials,pass);
    P_MUTEX_UNLOCK(priv->prop_lock);
}

OnvifCredentials * 
OnvifDevice__get_credentials(OnvifDevice * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), NULL);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    return priv->credentials;
}

int 
OnvifDevice__is_valid(OnvifDevice* self){
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), FALSE);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    //According to Core specification, the device entry point it set. (5.1.1)
    if(!ParsedURL__is_valid(priv->purl) || !priv->purl->service || strcmp(priv->purl->service,"onvif/device_service")){
        return 0;
    }

    //TODO Test endpoint
    return 1;
}

OnvifDeviceService * 
OnvifDevice__get_device_service(OnvifDevice* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), NULL);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    return priv->device_service;
}

OnvifDeviceIOService * 
OnvifDevice__get_deviceio_service(OnvifDevice* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), NULL);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    return priv->deviceio_service;
}

OnvifMediaService * 
OnvifDevice__get_media_service(OnvifDevice* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), NULL);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    return ONVIF_MEDIA_SERVICE(priv->media1_service);
}

OnvifMedia1Service * 
OnvifDevice__get_media1_service(OnvifDevice* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), NULL);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    return priv->media1_service;
}

OnvifMedia2Service * 
OnvifDevice__get_media2_service(OnvifDevice* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), NULL);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    return priv->media2_service;
}

static void
OnvifDevice__dispose (GObject *object){
    OnvifDevice *self = ONVIF_DEVICE (object);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);

    ONVIF_DEVICE_DEBUG("[%s] OnvifDevice__dispose",self);

    if(priv->credentials){
        OnvifCredentials__destroy(priv->credentials);
        priv->credentials = NULL;
    }

    if(priv->device_service){
        g_object_unref(priv->device_service);
        priv->device_service = NULL;
    }

    if(priv->deviceio_service){
        g_object_unref(priv->deviceio_service);
        priv->deviceio_service = NULL;
    }

    if(priv->media1_service){
        g_object_unref(priv->media1_service);
        priv->media1_service = NULL;
    }

    if(priv->media2_service){
        g_object_unref(priv->media2_service);
        priv->media2_service = NULL;
    }
    
    P_MUTEX_CLEANUP(priv->media_lock);
    P_MUTEX_CLEANUP(priv->prop_lock);
    P_MUTEX_CLEANUP(priv->auth_lock);

    if(priv->purl){
        ParsedURL__destroy(priv->purl);
        priv->purl = NULL;
    }

    if(priv->datetime){
        free(priv->datetime);
        priv->datetime = NULL;
    }

    G_OBJECT_CLASS (OnvifDevice__parent_class)->dispose (object);
}

static void
OnvifDevice__set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec){
    OnvifDevice * self = ONVIF_DEVICE (object);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    const char * strval;

    switch (prop_id){
        case PROP_URI:
            strval = g_value_get_string (value);
            priv->purl = ParsedURL__create((char*)strval);
            priv->device_service = OnvifDeviceService__new(self, strval);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifDevice__get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec){
    OnvifDevice * self = ONVIF_DEVICE (object);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    switch (prop_id){
        case PROP_URI:
            g_value_set_string (value, OnvifBaseService__get_endpoint(ONVIF_BASE_SERVICE(priv->device_service)));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifDevice__constructed (GObject * object){
    OnvifDevice *self = ONVIF_DEVICE (object);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    C_INFO("Created Device:\n");
    C_INFO("\tprotocol -- %s\n",priv->purl->protocol);
    C_INFO("\thost : %s\n",priv->purl->hostorip);
    C_INFO("\tport -- %s\n",priv->purl->port);
    C_INFO("\tendpoint : %s\n",priv->purl->service);
}

static void
OnvifDevice__class_init (OnvifDeviceClass *klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifDevice__dispose;
    object_class->set_property = OnvifDevice__set_property;
    object_class->get_property = OnvifDevice__get_property;
    object_class->constructed = OnvifDevice__constructed;
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
OnvifDevice__init(OnvifDevice* self) {
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    P_MUTEX_SETUP(priv->prop_lock);
    P_MUTEX_SETUP(priv->auth_lock);
    P_MUTEX_SETUP(priv->media_lock);

    priv->authenticated = 0;
    priv->time_offset = 0;
    priv->credentials = OnvifCredentials__create();
    priv->device_service = NULL;
    priv->deviceio_service = NULL;
    priv->media1_service = NULL;
    priv->purl = NULL;
    priv->datetime = NULL;
}

OnvifDevice * 
OnvifDevice__new(char * device_url) {
    return g_object_new (ONVIF_TYPE_DEVICE, "uri",device_url, NULL);
}

char * 
OnvifDevice__get_host(OnvifDevice* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), NULL);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    char * ret = NULL;
    P_MUTEX_LOCK(priv->prop_lock);
    if(priv->purl->hostorip){
        ret = malloc(strlen(priv->purl->hostorip)+1);
        strcpy(ret,priv->purl->hostorip);
    }
    P_MUTEX_UNLOCK(priv->prop_lock);
    return ret;
}

char * 
OnvifDevice__get_port(OnvifDevice* self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE (self), NULL);
    OnvifDevicePrivate *priv = OnvifDevice__get_instance_private (self);
    char * ret = NULL;
    P_MUTEX_LOCK(priv->prop_lock);
    if(priv->purl->port){
        ret = malloc(strlen(priv->purl->port)+1);
        strcpy(ret,priv->purl->port);
    }
    P_MUTEX_UNLOCK(priv->prop_lock);
    return ret;
}
