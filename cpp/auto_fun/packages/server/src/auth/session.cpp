#include "session.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> createSession(SessionData data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto redis = getGlobalRedisCache();
    const auto sid = uuid();
    "redis.set(" + "sid:" + sid
    return sid;

}

std::future<std::optional<SessionData>> getSession(const std::string& sid) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto redis = getGlobalRedisCache();
    const auto raw = "redis.get(" + "sid:" + sid;
    return raw ? (/* JSON.parse */ raw) : nullptr;

}

std::future<void> destroySession(const std::string& sid) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto redis = getGlobalRedisCache();
    "redis.del(" + "sid:" + sid

}

} // namespace elizaos
