#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_CACHE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_CACHE_H
#include "core.h"
#include "ioredis.h"
#include "@/types/project.h"

class CacheManager;

extern any redis;
class CacheManager : public object, public std::enable_shared_from_this<CacheManager> {
public:
    using std::enable_shared_from_this<CacheManager>::shared_from_this;
    static any get(string key);
    static void set(string key, any data, double ttl = 300);
};

#endif
