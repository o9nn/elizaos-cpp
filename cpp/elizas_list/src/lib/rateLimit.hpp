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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class RateLimiter {
public:
    static std::future<bool> checkLimit(const std::string& key, double limit = 100, double window = 3600);
    static std::future<double> getRemainingLimit(const std::string& key);
};
 
} // namespace elizaos
