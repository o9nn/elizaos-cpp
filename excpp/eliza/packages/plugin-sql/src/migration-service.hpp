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
#include "custom-migrator.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class DatabaseMigrationService {
public:
    DatabaseMigrationService();
    std::future<void> initializeWithDatabase(DrizzleDatabase db);
    void discoverAndRegisterPluginSchemas(const std::vector<Plugin>& plugins);
    std::future<void> runAllPluginMigrations();
};


} // namespace elizaos
