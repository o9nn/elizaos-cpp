#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



    // Temporarily disabled while migrating to new registry system

std::string extractPluginInfo(const std::string& text);

std::future<std::string> resolvePluginPath(const std::string& pluginInfo);

} // namespace elizaos
