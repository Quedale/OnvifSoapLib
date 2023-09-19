#ifndef ONVIF_SNAPSHOT_H_ 
#define ONVIF_SNAPSHOT_H_

#include "shard_export.h"

typedef struct _OnvifSnapshot OnvifSnapshot;

OnvifSnapshot * OnvifSnapshot__create(int size, char * buffer);
void OnvifSnapshot__init(OnvifSnapshot * self, int size, char * buffer);
SHARD_EXPORT void OnvifSnapshot__destroy(OnvifSnapshot * self);

SHARD_EXPORT double OnvifSnapshot__get_size(OnvifSnapshot * self);
SHARD_EXPORT char * OnvifSnapshot__get_buffer(OnvifSnapshot * self);

#endif