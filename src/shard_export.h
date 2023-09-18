#ifndef SHARD_EXPORT_H_ 
#define SHARD_EXPORT_H_

#ifdef __cplusplus
    #define SHARD_EXTERN extern "C"
#else
    #define SHARD_EXTERN extern
#endif

#ifdef _WIN32
    #ifdef BUILD_SHARD
        #define SHARD_EXPORT SHARD_EXTERN __declspec(dllexport)
    #else
        #define SHARD_EXPORT SHARD_EXTERN __declspec(dllimport)
    #endif
#else
    #ifdef BUILD_SHARD
        #define SHARD_EXPORT SHARD_EXTERN  __attribute__((visibility("default")))
    #else
        #define SHARD_EXPORT SHARD_EXTERN
    #endif
#endif

#endif