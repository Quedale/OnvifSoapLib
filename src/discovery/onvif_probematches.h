#ifndef prob_match_h__
#define prob_match_h__

#include <glib-object.h>
#include "shard_export.h"

#define ONVIF_TYPE_PROBEMATCHES OnvifProbeMatches__get_type()
G_DECLARE_FINAL_TYPE (OnvifProbeMatches, OnvifProbeMatches_, ONVIF, PROBEMATCHES, GObject)

struct _OnvifProbeMatches {
    GObject parent_instance;
};


struct _OnvifProbeMatchesClass {
    GObjectClass parent_class;
};

 typedef struct {
    char *prob_uuid;
    char *addr_uuid; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/d:ProbeMatch
    char ** addrs; // // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/d:ProbeMatch/wsa:EndpointReference/wsa:Address
    int addrs_count;
    char *types; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/d:ProbeMatch/d:Types
    int scope_count;
    char **scopes; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/d:ProbeMatch/d:Scopes
    char *service; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/d:ProbeMatch/d:XAddrs
    unsigned int version; // SOAP-ENV:Envelope/SOAP-ENV:Body/d:ProbeMatches/d:ProbeMatch/d:MetadataVersion
} ProbMatch;

SHARD_EXPORT OnvifProbeMatches* OnvifProbeMatches__new(); 
void OnvifProbeMatches__insert_match(OnvifProbeMatches* self, ProbMatch* match);
SHARD_EXPORT int OnvifProbeMatches__get_count(OnvifProbeMatches* self);
SHARD_EXPORT ProbMatch ** OnvifProbeMatches__get_matches(OnvifProbeMatches* self);

ProbMatch* ProbMatch__create();
void ProbMatch__destroy(ProbMatch* self);
void ProbMatch__set_prob_uuid(ProbMatch* self, char * prob_uuid);
void ProbMatch__set_addr_uuid(ProbMatch* self, char * addr_uuid);
void ProbMatch__add_addr(ProbMatch* self, char * addr);
void ProbMatch__set_types(ProbMatch* self, char * types);
void ProbMatch__insert_scope(ProbMatch* self, char * scope);
void ProbMatch__set_version(ProbMatch* self, int scope);
SHARD_EXPORT char * ProbMatch__extract_scope(char * key, ProbMatch * match);

#endif
