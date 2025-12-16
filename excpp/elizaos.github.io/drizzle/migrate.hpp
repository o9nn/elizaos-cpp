#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct MigrationMeta {
    std::vector<std::string> sql;
    bool bps;
    double folderMillis;
    std::string hash;
};

/**
 * Custom migration function that allows running migrations up to a specific number.
 * @param db The Drizzle database instance.
 * @param config Configuration object, must include `migrationsFolder`.
 * @param maxMigrationNumber Optional. If provided, only migrations up to this number (inclusive) will be run.
 */

// Ensure the directory for the database exists


} // namespace elizaos
