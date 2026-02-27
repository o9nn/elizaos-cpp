#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_CACHE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_CACHE_H
#include "core.hpp"
#include "ioredis.hpp"
// External dependency removed

class CacheManager;

extern any redis;
class CacheManager : public object, public std::enable_shared_from_this<CacheManager> {
public:
    using std::enable_shared_from_this<CacheManager>::shared_from_this;
    static any get(string key);
    static void set(string key, any data, double ttl = 300);
};

#endif
