#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/rateLimit.h"

std::shared_ptr<Promise<boolean>> RateLimiter::checkLimit(string key, double limit, double window)
{
    auto current = std::async([=]() { rateLimitRedis->incr(key); });
    if (current == 1) {
        std::async([=]() { rateLimitRedis->expire(key, window); });
    }
    return current <= limit;
}

std::shared_ptr<Promise<double>> RateLimiter::getRemainingLimit(string key)
{
    auto current = std::async([=]() { rateLimitRedis->get(key); });
    return 100 - (parseInt(OR((current), (std::string("0")))));
}

any rateLimitRedis = std::make_shared<Redis>(OR((process->env->REDIS_URL), (std::string("redis://localhost:6379"))));

void Main(void)
{
}

MAIN
