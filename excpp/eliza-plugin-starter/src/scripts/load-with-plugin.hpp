#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "loader.ts.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// ES Module dirname equivalent

struct Plugin {
    std::string name;
    std::string description;
    std::optional<std::vector<std::any>> actions;
    std::optional<std::vector<std::any>> providers;
    std::optional<std::vector<std::any>> evaluators;
    std::optional<std::vector<std::any>> services;
    std::optional<std::vector<std::any>> clients;
};

// Minimal DatabaseAdapter Mock

// Cache Adapter Implementation
class CompatibleCacheAdapter {
public:
    undefined) as T;
  }
    std::future<void> delete(const std::string& key);

// Function to dynamically load plugins from ./src/plugins

// Function to resolve plugins from their string names

// Type Guard to check if plugins are strings
std::vector<plugins is string> isStringArray(unknown plugins);

std::future<void> main();

// Run the main function

} // namespace elizaos
