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


class CacheManager {
public:
    std::future<void> delete(const std::string& key);
    std::future<void> clear();
};


} // namespace elizaos
