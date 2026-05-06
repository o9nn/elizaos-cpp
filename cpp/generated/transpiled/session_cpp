#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/auth/session.h"

std::shared_ptr<Promise<string>> createSession(std::shared_ptr<SessionData> data)
{
    auto redis = std::async([=]() { getGlobalRedisCache(); });
    auto sid = uuid();
    std::async([=]() { redis->set(std::string("sid:") + sid + string_empty, JSON->stringify(data), SESSION_TTL); });
    return sid;
};


std::shared_ptr<Promise<any>> getSession(string sid)
{
    auto redis = std::async([=]() { getGlobalRedisCache(); });
    auto raw = std::async([=]() { redis->get(std::string("sid:") + sid + string_empty); });
    return (raw) ? any((as<std::shared_ptr<SessionData>>(JSON->parse(raw)))) : any(nullptr);
};


void destroySession(string sid)
{
    auto redis = std::async([=]() { getGlobalRedisCache(); });
    std::async([=]() { redis->del(std::string("sid:") + sid + string_empty); });
};


double SESSION_TTL = 1 * 24 * 60 * 60;

void Main(void)
{
}

MAIN
