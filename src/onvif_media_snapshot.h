#ifndef ONVIF_SNAPSHOT_H_ 
#define ONVIF_SNAPSHOT_H_

#include "shard_export.h"
#include <stddef.h>

typedef struct _OnvifSnapshot OnvifSnapshot;

SHARD_EXPORT OnvifSnapshot * OnvifSnapshot__create(size_t size, char * buffer);
SHARD_EXPORT void OnvifSnapshot__init(OnvifSnapshot * self, size_t size, char * buffer);
SHARD_EXPORT void OnvifSnapshot__destroy(OnvifSnapshot * self);

SHARD_EXPORT double OnvifSnapshot__get_size(OnvifSnapshot * self);
SHARD_EXPORT char * OnvifSnapshot__get_buffer(OnvifSnapshot * self);

#endif