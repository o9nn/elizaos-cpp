#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "redis.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SessionData {
    std::string publicKey;
    std::optional<std::vector<std::string>> privileges;
    double createdAt;
};

std::future<std::string> createSession(SessionData data);

std::future<void> destroySession(const std::string& sid);

} // namespace elizaos
