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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class PostgresConnectionManager {
public:
    PostgresConnectionManager(const std::string& connectionString);
    NodePgDatabase getDatabase();
    Pool getConnection();
    std::future<PoolClient> getClient();
    std::future<bool> testConnection();
    std::future<void> close();

private:
    Pool pool_;
    NodePgDatabase db_;
};


} // namespace elizaos
