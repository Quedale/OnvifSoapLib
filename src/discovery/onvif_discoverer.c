//
// Created by quedale on 2025-12-10.
//
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif
#include "wsdd.nsmap"
#include "plugin/logging.h"
#include "wsddapi.h"
#include "onvif_discoverer.h"
#include "clogger.h"
#include "onvif_probematches.h"

enum {
    PROP_RETRY_COUNT = 1,
    PROP_TIMEOUT = 2,
    N_PROPERTIES
};

enum {
    PROBE_MATCH,
    LAST_SIGNAL
};

typedef struct {
    int retry_count;
    int timeout;
} OnvifDiscovererPrivate ;

static guint signals[LAST_SIGNAL] = { 0 };
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE_WITH_PRIVATE (OnvifDiscoverer, OnvifDiscoverer_, G_TYPE_OBJECT)

static void
OnvifDiscoverer__set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec){
    OnvifDiscoverer * self = ONVIF_DISCOVERER (object);
    OnvifDiscovererPrivate *priv = OnvifDiscoverer__get_instance_private (self);
    int intval;
    switch (prop_id){
        case PROP_RETRY_COUNT:
            intval = g_value_get_int(value);
            priv->retry_count = intval;
            break;
        case PROP_TIMEOUT:
            intval = g_value_get_int(value);
            priv->timeout = intval;
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifDiscoverer__get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec){
    OnvifDiscoverer * self = ONVIF_DISCOVERER (object);
    OnvifDiscovererPrivate *priv = OnvifDiscoverer__get_instance_private (self);
    switch (prop_id){
        case PROP_RETRY_COUNT:
            g_value_set_int (value, priv->retry_count);
            break;
        case PROP_TIMEOUT:
            g_value_set_int (value, priv->timeout);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
OnvifDiscoverer__init(OnvifDiscoverer* self) {
    OnvifDiscovererPrivate *priv = OnvifDiscoverer__get_instance_private (self);
    priv->retry_count = 1;
    priv->timeout = 1;
}

static void
OnvifDiscoverer__class_init (OnvifDiscovererClass *klass){
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = OnvifDiscoverer__set_property;
    object_class->get_property = OnvifDiscoverer__get_property;

    GType params[1];
    params[0] = ONVIF_TYPE_PROBEMATCHES | G_SIGNAL_TYPE_STATIC_SCOPE;
    signals[PROBE_MATCH] =
        g_signal_newv ("probe-match",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                        NULL /* closure */,
                        NULL /* accumulator */,
                        NULL /* accumulator data */,
                        NULL /* C marshaller */,
                        G_TYPE_NONE /* return_type */,
                        1     /* n_params */,
                        params  /* param_types */);
                        
    obj_properties[PROP_RETRY_COUNT] =
            g_param_spec_int ("retry-count",
                                 "Retry Count",
                                 "Maximum number of retry",
                                 1,INT_MAX,1,
                                G_PARAM_CONSTRUCT | G_PARAM_WRITABLE);
    obj_properties[PROP_TIMEOUT] =
            g_param_spec_int ("timeout",
                              "Timeout",
                              "Message wait timeout",
                              1,INT_MAX,1,
                              G_PARAM_CONSTRUCT | G_PARAM_WRITABLE);

    g_object_class_install_properties (object_class,
                                       N_PROPERTIES,
                                       obj_properties);
}

OnvifDiscoverer *
OnvifDiscoverer__new(int retry, int timeout) {
    return g_object_new (ONVIF_TYPE_DISCOVERER, "retry-count",retry, "timeout", timeout, NULL);
}

static void
OnvifDiscoverer__sendMessage(struct soap * serv, char * type, const char * id){
    int ret = soap_wsdd_Probe(serv,SOAP_WSDD_ADHOC,SOAP_WSDD_TO_TS,"soap.udp://239.255.255.250:3702", id,
                              NULL, //ReplyTo
                              type, //Type
                              NULL, // Scopes
                              NULL); // MatchBy

    if (ret != SOAP_OK){
        soap_print_fault(serv, stderr);
        C_ERROR("error sending prob...%i",ret);
    }
}

static void
OnvifDiscoverer__sendProbe(OnvifDiscoverer * self){
    char *debug_flag = NULL;

    if (( debug_flag =getenv( "PROBE_DEBUG" )) != NULL )
        C_INFO("PROBE_DEBUG variable set. '%s'",debug_flag);

    struct soap serv;
    memset((void*)&serv, 0, sizeof(serv));
    soap_init1(&serv,SOAP_IO_UDP);
    soap_set_namespaces(&serv, disco_namespaces);
    soap_register_plugin(&serv, soap_wsa);

    if(debug_flag){
        soap_register_plugin(&serv, logging);
        soap_set_logging_outbound(&serv,stdout);
        soap_set_logging_inbound(&serv,stdout);
    }

    if (!soap_valid_socket(soap_bind(&serv, NULL, 0, 1000))){
        C_ERROR("Failed to bind discovery socket");
        soap_print_fault(&serv, stderr);
        goto exit;
    }

    //TODO Support provided setting
    // serv->ipv4_multicast_ttl = 1;

    //Broadcast prob request

    //TODO allow choosing probe types by config
    //TODOD  Filter out stuff like printers
    // C_TRACE("Sending Empty probe...");
    // priv_sendProbe(&serv, NULL, msg->id);

    // C_TRACE("Sending Device probe...");
    // priv_sendProbe(&serv, "\"http://www.onvif.org/ver10/device/wsdl\":Device", msg->id);

    C_DEBUG("Sending NVT probe...");
    serv.user = self;

    OnvifDiscoverer__sendMessage(&serv, "\"http://www.onvif.org/ver10/network/wsdl\":NetworkVideoTransmitter", soap_wsa_rand_uuid(&serv));

    //Listen for responses
    OnvifDiscovererPrivate *priv = OnvifDiscoverer__get_instance_private (self);
    if (soap_wsdd_listen(&serv, priv->timeout) != SOAP_OK){
        soap_print_fault(&serv, stderr);
        C_ERROR("error listening prob...");
    }


exit:
    if(debug_flag){
        soap_set_logging_outbound(&serv,NULL);
        soap_set_logging_inbound(&serv,NULL);
    }
    soap_destroy(&serv); // delete managed objects
    soap_end(&serv);     // delete managed data and temporaries
    soap_done(&serv);
}

void
OnvifDiscoverer__start(OnvifDiscoverer * self){
    g_return_if_fail (self != NULL);
    g_return_if_fail (ONVIF_IS_DISCOVERER (self));
    OnvifDiscovererPrivate *priv = OnvifDiscoverer__get_instance_private (self);
    for(int i=0;i<priv->retry_count;i++){
        OnvifDiscoverer__sendProbe(self);
    }
}

void
OnvifDiscoverer__stop(OnvifDiscoverer * self){
    g_return_if_fail (self != NULL);
    g_return_if_fail (ONVIF_IS_DISCOVERER (self));
    C_FIXME("TODO Implement stop");
}

// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str){
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *ProbeMatches){
    C_DEBUG("ProbeMatches found...");

    OnvifDiscoverer * self = ONVIF_DISCOVERER(soap->user);

    OnvifProbeMatches * matches = NULL;
    int i;
    for (i=0;i<ProbeMatches->__sizeProbeMatch;i++){
        struct wsdd__ProbeMatchType match = ProbeMatches->ProbeMatch[i];
        //Check if valid ONVIF device
        //TODO possible additional validation
        if(!match.XAddrs || !match.Types){
            C_WARN("Invalid response. Ingnoring...(Use 'PROBE_DEBUG' environment variable to troubleshoot)");
            continue;
        }

        ProbMatch *ret_match = ProbMatch__create();
        ProbMatch__set_prob_uuid(ret_match,(char *) RelatesTo);

        char* token;
        while ((token = strtok_r(match.XAddrs, " ", &match.XAddrs))){
            ProbMatch__add_addr(ret_match,token);
        }
        ProbMatch__set_types(ret_match,match.Types);

        if(match.MetadataVersion){
            ProbMatch__set_version(ret_match,match.MetadataVersion);
        }
        // if(match.wsa__EndpointReference.Address){
        //   ProbMatch__set_addr_uuid(ret_match,match.wsa__EndpointReference.Address);
        // }
        if(match.Scopes && match.Scopes->__item){
            char *tmp;
            char *p = strtok_r ((char *)match.Scopes->__item, "\n", &match.Scopes->__item);
            char *q = NULL;
            while (p != NULL){
                tmp=trimwhitespace(p);
                if(tmp[0] == '\0'){
                    p = strtok_r(match.Scopes->__item,"\n", &match.Scopes->__item);
                    continue;
                }

                q = strtok_r(tmp," ", &tmp);
                while(q != NULL){
                    ProbMatch__insert_scope(ret_match,q);
                    q = strtok_r (tmp, " ", &tmp);
                }
                p = strtok_r ((char *)match.Scopes->__item, "\n", &match.Scopes->__item);
            }

        }

        if(!matches) matches = OnvifProbeMatches__new();
        OnvifProbeMatches__insert_match(matches,ret_match);
    }

    if(matches) { //No valid match in response.
        g_signal_emit (self, signals[PROBE_MATCH], 0, matches);
        g_object_unref(matches);
    }
}

//Place holder for successful client compile
void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{ C_WARN("wsdd_event_Hello"); }

void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{ C_WARN("wsdd_event_Bye"); }

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *ProbeMatches){
    C_WARN("wsdd_event_Probe");
    return SOAP_WSDD_ADHOC;
}

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match){
    C_WARN("wsdd_event_Resolve");
    return SOAP_WSDD_ADHOC;
}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char * SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{ C_WARN("wsdd_event_ResolveMatches"); }
