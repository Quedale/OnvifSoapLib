
#include "onvif_media2_uri_local.h"
#include "../onvif_uri_local.h"

G_DEFINE_TYPE (OnvifMedia2Uri, OnvifMedia2Uri_, ONVIF_TYPE_URI)

static void
OnvifMedia2Uri__construct(SoapObject * obj, gpointer ptr){
    OnvifMedia2Uri * self = ONVIF_MEDIA2_URI(obj);
    struct xsd__anyURI * resp = ptr;

    if(!resp){
        return;
    }

    OnvifUri__set_uri(ONVIF_URI(self),(char*)resp);
}

static void
OnvifMedia2Uri__class_init (OnvifMedia2UriClass * klass){
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifMedia2Uri__construct;
}

static void
OnvifMedia2Uri__init (OnvifMedia2Uri * self){

}


OnvifMedia2Uri * 
OnvifMedia2StreamUri__new(struct _ns1__GetStreamUriResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA2_URI, "data", (!resp) ? NULL : resp->Uri, NULL);
}

OnvifMedia2Uri * 
OnvifMedia2SnapshotUri__new(struct _ns1__GetSnapshotUriResponse * resp){
    return g_object_new (ONVIF_TYPE_MEDIA2_URI, "data", (!resp) ? NULL : resp->Uri, NULL);
}
