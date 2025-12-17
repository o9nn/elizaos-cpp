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
#include "elizaos/core.hpp"
#include "schema.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use




struct ConnectionOptions {
    std::optional<std::string> postgresUrl;
    std::optional<std::string> dataDir;
};

std::future<DatabaseConnection> createDatabaseConnection(ConnectionOptions options);

DatabaseConnection getDatabase();

std::future<void> closeDatabase();

} // namespace elizaos
