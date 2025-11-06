#ifndef prob_match_h__
#define prob_match_h__

#include "shard_export.h"

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

typedef struct { 
    int match_count;
    ProbMatch **matches;
} ProbMatches;

SHARD_EXPORT ProbMatches* ProbMatches__create(); 
SHARD_EXPORT void ProbMatches__destroy(ProbMatches* self);
SHARD_EXPORT void ProbMatches__insert_match(ProbMatches* self, ProbMatch* match);
SHARD_EXPORT ProbMatch* ProbMatch__create();
SHARD_EXPORT void ProbMatch__destroy(ProbMatch* self);
SHARD_EXPORT void ProbMatch__set_prob_uuid(ProbMatch* self, char * prob_uuid);
SHARD_EXPORT void ProbMatch__set_addr_uuid(ProbMatch* self, char * addr_uuid);
SHARD_EXPORT void ProbMatch__add_addr(ProbMatch* self, char * addr);
SHARD_EXPORT void ProbMatch__set_types(ProbMatch* self, char * types);
SHARD_EXPORT void ProbMatch__insert_scope(ProbMatch* self, char * scope);
SHARD_EXPORT void ProbMatch__set_version(ProbMatch* self, int scope);

#endif
