#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_RATELIMIT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_RATELIMIT_H
#include "core.hpp"
#include "ioredis.hpp"

class RateLimiter;

extern any rateLimitRedis;
class RateLimiter : public object, public std::enable_shared_from_this<RateLimiter> {
public:
    using std::enable_shared_from_this<RateLimiter>::shared_from_this;
    static std::shared_ptr<Promise<boolean>> checkLimit(string key, double limit = 100, double window = 3600);
    static std::shared_ptr<Promise<double>> getRemainingLimit(string key);
};

#endif
