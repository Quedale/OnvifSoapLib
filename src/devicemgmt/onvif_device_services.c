#include "onvif_device_services_local.h"
#include "clogger.h"

typedef struct _OnvifService {
    char * xaddr;
    int major;
    int minor;
} OnvifService;

typedef struct {
    OnvifService * imaging;
    OnvifService * media;
    OnvifService * media2;
    OnvifService * deviceio;
    OnvifService * events;
} OnvifDeviceServicesPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifDeviceServices, OnvifDeviceServices_, SOAP_TYPE_OBJECT)

OnvifService * OnvifService__create(OnvifDeviceServices * parent, struct tds__Service * tds_serv);
void OnvifService__destroy(OnvifService * self);
static void
OnvifDeviceServices__reset (OnvifDeviceServices *self);

static void
OnvifDeviceServices__construct(SoapObject * obj, gpointer ptr){
    struct _tds__GetServicesResponse * response = ptr;
    OnvifDeviceServices * self = ONVIF_DEVICE_SERVICES(obj);
    OnvifDeviceServicesPrivate *priv = OnvifDeviceServices__get_instance_private (self);


    OnvifDeviceServices__reset(self);

    if(!response){
        return;
    }

    for(int i=0;i<response->__sizeService;i++){
        struct tds__Service tds_serv = response->Service[i];
        if(!strcmp(tds_serv.Namespace,"http://www.onvif.org/ver10/device/wsdl")){
            //Already created
        } else if(!strcmp(tds_serv.Namespace,"http://www.onvif.org/ver10/deviceIO/wsdl")){
            priv->deviceio = OnvifService__create(self,&tds_serv);
        } else if(!strcmp(tds_serv.Namespace,"http://www.onvif.org/ver20/imaging/wsdl")){
            priv->imaging = OnvifService__create(self,&tds_serv);
        } else if(!strcmp(tds_serv.Namespace,"http://www.onvif.org/ver10/media/wsdl")){
            priv->media = OnvifService__create(self,&tds_serv);
        } else if(!strcmp(tds_serv.Namespace,"http://www.onvif.org/ver20/media/wsdl")){
            priv->media2 = OnvifService__create(self,&tds_serv);
        }else if(!strcmp(tds_serv.Namespace,"http://www.onvif.org/ver10/events/wsdl")){
            priv->events = OnvifService__create(self,&tds_serv);
        } else {
            C_DEBUG("Unhandled service '%s'",tds_serv.Namespace);
        }
    }
}

static void
OnvifDeviceServices__reset (OnvifDeviceServices *self){
    OnvifDeviceServicesPrivate *priv = OnvifDeviceServices__get_instance_private (self);
    
    if(priv->imaging){
        OnvifService__destroy(priv->imaging);
        priv->imaging = NULL;
    }

    if(priv->media){
        OnvifService__destroy(priv->media);
        priv->media = NULL;
    }

    if(priv->media2){
        OnvifService__destroy(priv->media2);
        priv->media2 = NULL;
    }

    if(priv->deviceio){
        OnvifService__destroy(priv->deviceio);
        priv->deviceio = NULL;
    }

    if(priv->events){
        OnvifService__destroy(priv->events);
        priv->events = NULL;
    }
}

static void
OnvifDeviceServices__dispose (GObject *self)
{
    OnvifDeviceServices__reset(ONVIF_DEVICE_SERVICES(self));

    G_OBJECT_CLASS (OnvifDeviceServices__parent_class)->dispose (self);
}


static void
OnvifDeviceServices__class_init (OnvifDeviceServicesClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifDeviceServices__dispose;
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifDeviceServices__construct;
}

static void
OnvifDeviceServices__init (OnvifDeviceServices * self)
{
    OnvifDeviceServicesPrivate *priv = OnvifDeviceServices__get_instance_private (ONVIF_DEVICE_SERVICES(self));
    priv->imaging = NULL;
    priv->media = NULL;
    priv->media2 = NULL;
    priv->deviceio = NULL;
    priv->events = NULL;
}

OnvifDeviceServices* OnvifDeviceServices__new(struct _tds__GetServicesResponse * resp){
    return g_object_new (ONVIF_TYPE_DEVICE_SERVICES, "data", resp, NULL);
}

OnvifService * OnvifDeviceServices__get_deviceio(OnvifDeviceServices * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SERVICES (self), NULL);

    OnvifDeviceServicesPrivate *priv = OnvifDeviceServices__get_instance_private (ONVIF_DEVICE_SERVICES(self));
    return priv->deviceio;
}

SHARD_EXPORT OnvifService * OnvifDeviceServices__get_media(OnvifDeviceServices * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SERVICES (self), NULL);

    OnvifDeviceServicesPrivate *priv = OnvifDeviceServices__get_instance_private (ONVIF_DEVICE_SERVICES(self));
    return priv->media;
}

SHARD_EXPORT OnvifService * OnvifDeviceServices__get_media2(OnvifDeviceServices * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SERVICES (self), NULL);

    OnvifDeviceServicesPrivate *priv = OnvifDeviceServices__get_instance_private (ONVIF_DEVICE_SERVICES(self));
    return priv->media2;
}

SHARD_EXPORT OnvifService * OnvifDeviceServices__get_events(OnvifDeviceServices * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SERVICES (self), NULL);

    OnvifDeviceServicesPrivate *priv = OnvifDeviceServices__get_instance_private (ONVIF_DEVICE_SERVICES(self));
    return priv->events;
}

SHARD_EXPORT OnvifService * OnvifDeviceServices__get_imaging(OnvifDeviceServices * self){
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (ONVIF_IS_DEVICE_SERVICES (self), NULL);

    OnvifDeviceServicesPrivate *priv = OnvifDeviceServices__get_instance_private (ONVIF_DEVICE_SERVICES(self));
    return priv->imaging;
}

OnvifService * OnvifService__create(OnvifDeviceServices * parent, struct tds__Service * tds_serv){
    OnvifService * self = malloc(sizeof(OnvifService));
    self->xaddr = malloc(strlen(tds_serv->XAddr)+1);
    strcpy(self->xaddr,tds_serv->XAddr);

    if(tds_serv->Capabilities) {
        if(!strcmp(tds_serv->Namespace,"http://www.onvif.org/ver10/media/wsdl")){
            // struct trt__Capabilities test;

            // //TODO Properly map anyType to T_SOAP type
            // struct soap * ctx = soap_new1(SOAP_XML_IGNORENS);
            // ctx->is = tds_serv->Capabilities->__any;
            // C_FATAL("is %d: %s",strlen(ctx->is),ctx->is);
            // //Re-implement soap_read_trt__Capabilities to resolve warnings
            // soap_default_trt__Capabilities(ctx, &test);
            // if(soap_begin_recv(ctx) || !soap_get_trt__Capabilities(ctx, &test, NULL, NULL) || soap_end_recv(ctx)){
            //     soap_print_fault(ctx, stderr);
            // } else {
            //     C_DEBUG("SnapshotUri : %d",*test.SnapshotUri);
            //     C_DEBUG("Rotation : %d",*test.Rotation);
            //     C_DEBUG("ProfileCapabilities : %d",*test.ProfileCapabilities->MaximumNumberOfProfiles);
            //     C_DEBUG("is %d: %s",strlen(ctx->is),ctx->is);
            //     C_DEBUG("tds_serv->Capabilities->__any : %s",tds_serv->Capabilities->__any);
            //     C_DEBUG("XxAddr : %s",self->xaddr);
            // }
        } else {
            C_DEBUG("Service : %s -- %s",tds_serv->Namespace,self->xaddr);
        }
    } else {
        C_WARN("No capability provided.");
    }
    self->major = tds_serv->Version->Major;
    self->minor = tds_serv->Version->Minor;

    return self;
}

void OnvifService__destroy(OnvifService * self){
    if(self){
        if(self->xaddr){
            free(self->xaddr);
            self->xaddr = NULL;
        }
    }
}

SHARD_EXPORT char * OnvifService__get_address(OnvifService * self){
    return self->xaddr;
}