#include "elizaos/core.hpp"
#include "schema.hpp"
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



using DatabaseConnection = std::variant<ReturnType<typeof drizzle>, ReturnType<typeof drizzlePglite>>;

struct ConnectionOptions {
    std::optional<std::string> postgresUrl;
    std::optional<std::string> dataDir;
};

std::future<DatabaseConnection> createDatabaseConnection(ConnectionOptions options);

DatabaseConnection getDatabase();

std::future<void> closeDatabase();

} // namespace elizaos
