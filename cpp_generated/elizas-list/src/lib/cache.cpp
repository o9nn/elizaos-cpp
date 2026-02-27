#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/cache.h"

any CacheManager::get(string key)
{
    try
    {
        auto cached = std::async([=]() { redis->get(key); });
        return (cached) ? any(JSON->parse(cached)) : any(nullptr);
    }
    catch (const any& error)
    {
        console->error(std::string("Cache get error:"), error);
        return nullptr;
    }
}

void CacheManager::set(string key, any data, double ttl)
{
    try
    {
        std::async([=]() { redis->setex(key, ttl, JSON->stringify(data)); });
    }
    catch (const any& error)
    {
        console->error(std::string("Cache set error:"), error);
    }
}

any redis = std::make_shared<Redis>(OR((process->env->REDIS_URL), (std::string("redis://localhost:6379"))));

void Main(void)
{
}

MAIN
