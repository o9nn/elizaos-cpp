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



// GitHub configuration

struct Repository {
    std::string name;
    std::string clone_url;
    bool has_1x_branch;
};

std::future<void> main();

std::future<void> migrateRepository(Repository repo, const std::string& progress);

// Handle process termination
    // Ignore cleanup errors on exit

} // namespace elizaos
