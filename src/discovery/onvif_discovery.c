#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif
#include "plugin/logging.h"
#include "wsddapi.h"
// #include "generated/wsdd.nsmap"
// #include "../safe_namespace.h"
#include "probmatch.h"
#include "clogger.h"
#include "onvif_discovery.h"
#include "discovered_obj.h"
#include "cstring_utils.h"

struct MessageEntry {
  char * id;
  int (*cc)(DiscoveryEvent * );
  void * data;
};

//Place holder for successful client compile
void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{ C_WARN("wsdd_event_Hello"); }

void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{ C_WARN("wsdd_event_Bye"); }

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *ProbeMatches)
{
  C_WARN("wsdd_event_Probe");
  return SOAP_WSDD_ADHOC;
}


// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str)
{
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

void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *ProbeMatches)
{
  C_DEBUG("ProbeMatches found...");

  ProbMatches * matches = ProbMatches__create();
  struct MessageEntry * entry = (struct MessageEntry *) soap->user;

  //Check if message actually relates to our probe
  if (RelatesTo && strcmp(RelatesTo,entry->id)!=0) {
    C_WARN("Unrelated id received. Expecting [%s] but was [%s].", RelatesTo, entry->id);
    return;
  }

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

    ProbMatches__insert_match(matches,ret_match);

  }
  
  if(matches->match_count > 0){ //No valid match in response.
    DiscoveredServer * server =  (DiscoveredServer *) malloc(sizeof(DiscoveredServer));
    server->matches = matches;
    server->msg_uuid = (char *) MessageID;
    server->relate_uuid = (char *) RelatesTo;
    DiscoveryEvent * ret_vent = DiscoveryEvent__create(server,entry->data);
    entry->cc (ret_vent);
  } else {
    CObject__destroy((CObject *)matches);
  }

}

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
    C_WARN("wsdd_event_Resolve");
  return SOAP_WSDD_ADHOC;
}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char * SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{ C_WARN("wsdd_event_ResolveMatches"); }

void priv_sendProbe(struct soap * serv, char * type, char * id){
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

void sendProbe(void * data, int timeout, int (*cc)(DiscoveryEvent *)){
  
  char *debug_flag = NULL;

  if (( debug_flag =getenv( "PROBE_DEBUG" )) != NULL )
    C_INFO("PROBE_DEBUG variable set. '%s'",debug_flag) ;

  struct soap serv;

  memset((void*)&serv, 0, sizeof(serv));
  soap_init1(&serv,SOAP_IO_UDP);
  soap_register_plugin(&serv, soap_wsa);

  if(debug_flag){
    soap_register_plugin(&serv, logging);
    soap_set_logging_outbound(&serv,stdout);
    soap_set_logging_inbound(&serv,stdout);
  }

  if (!soap_valid_socket(soap_bind(&serv, NULL, 0, 1000)))
  {
          soap_print_fault(&serv, stderr);
          exit(1);
  }

  //TODO Support provided setting
  // serv->ipv4_multicast_ttl = 1;

  struct MessageEntry * msg = malloc(sizeof(struct MessageEntry));
  msg->cc = cc;
  char * nid = (char *) soap_wsa_rand_uuid(&serv);
  msg->id = malloc(strlen(nid)+1);
  strcpy(msg->id,nid);
  msg->data = data;

  serv.user = msg;

  //Broadcast prob request
  
  //TODO allow choosing probe types by config
  //TODOD  Filter out stuff like printers
  // C_TRACE("Sending Empty probe...");
  // priv_sendProbe(&serv, NULL, msg->id);

  // C_TRACE("Sending Device probe...");
  // priv_sendProbe(&serv, "\"http://www.onvif.org/ver10/device/wsdl\":Device", msg->id);

  C_DEBUG("Sending NVT probe...");
  priv_sendProbe(&serv, "\"http://www.onvif.org/ver10/network/wsdl\":NetworkVideoTransmitter", msg->id);

  // C_TRACE("Sending NVT probe...");
  // priv_sendProbe(&serv, "\"http://www.onvif.org/ver10/network/wsdl\":NetworkVideoDisplay", msg->id);

  
  //Listen for responses
  if (soap_wsdd_listen(&serv, timeout) != SOAP_OK){
    soap_print_fault(&serv, stderr);
    C_ERROR("error listening prob...");
  }

  if(debug_flag){
    soap_set_logging_outbound(&serv,NULL);
    soap_set_logging_inbound(&serv,NULL);
  }
  soap_destroy(&serv); // delete managed objects
  soap_end(&serv);     // delete managed data and temporaries 
  soap_done(&serv);

  free(msg->id);
  free(msg);
  return;
}

// C substring function definition
void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}

void urldecode2(char *dst, const char *src)
{
        char a, b;
        while (*src) {
                if ((*src == '%') &&
                    ((a = src[1]) && (b = src[2])) &&
                    (isxdigit(a) && isxdigit(b))) {
                        if (a >= 'a')
                                a -= 'a'-'A';
                        if (a >= 'A')
                                a -= ('A' - 10);
                        else
                                a -= '0';
                        if (b >= 'a')
                                b -= 'a'-'A';
                        if (b >= 'A')
                                b -= ('A' - 10);
                        else
                                b -= '0';
                        *dst++ = 16*a+b;
                        src+=3;
                } else if (*src == '+') {
                        *dst++ = ' ';
                        src++;
                } else {
                        *dst++ = *src++;
                }
        }
        *dst++ = '\0';
}

char * onvif_extract_scope(char * key, ProbMatch * match){
  char* ret_val = malloc(1);
  ret_val[0] = '\0';
  const char delimeter[2] = "/";
  const char * onvif_key_del = "onvif://www.onvif.org/";

  char key_w_del[strlen(key)+1+strlen(delimeter)];
  strcpy(key_w_del, key);
  strcat(key_w_del, delimeter);

  int a;
  for (a = 0 ; a < match->scope_count ; ++a) {
    if(cstring_startsWith(onvif_key_del, match->scopes[a])){
      //Drop onvif scope prefix
      char subs[strlen(match->scopes[a])-strlen(onvif_key_del) + 1];
      strncpy(subs,match->scopes[a]+(strlen(onvif_key_del)),strlen(match->scopes[a]) - strlen(onvif_key_del)+1);

      if(cstring_startsWith(key_w_del,subs)){ // Found Scope
        //Extract value
        char sval[strlen(subs)-strlen(key_w_del) + 1];
        strncpy(sval,subs+(strlen(key_w_del)),strlen(subs) - strlen(key_w_del)+1);

        //Decode http string (e.g. %20 = whitespace)
        char output[strlen(sval)+1];
        urldecode2(output, sval);

        if(strlen(ret_val)==0){
          ret_val = realloc(ret_val, strlen(output) +1);
          strcpy(ret_val,output);
        } else {
          ret_val = realloc(ret_val, strlen(ret_val) + strlen(" ") + strlen(output) +1);
          strcat(ret_val, " ");
          strcat(ret_val, output);
        }
      }
    }
  }

  return ret_val;
}