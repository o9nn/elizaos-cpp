#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class RateLimiter {
public:
    std::future<bool> checkLimit(const std::string& key, number = 100 limit, number = 3600 window);
    std::future<double> getRemainingLimit(const std::string& key);
};
 
} // namespace elizaos
