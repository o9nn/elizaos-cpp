#include ".redis.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SessionData {
    std::string publicKey;
    std::optional<std::vector<std::string>> privileges;
    double createdAt;
};

std::future<std::string> createSession(SessionData data);

std::future<SessionData | null> getSession(const std::string& sid);

std::future<void> destroySession(const std::string& sid);

} // namespace elizaos
