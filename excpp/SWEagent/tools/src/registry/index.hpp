#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Registry tool
 * Manages environment variables and state in a JSON file
 * Converted from tools/registry/
 */

/**
 * Registry class for managing environment state
 */
class EnvRegistry {
public:
    EnvRegistry(std::optional<std::string> envFile);
    void loadData();
    void saveData();
    void set(const std::string& key, unknown value);
    void delete(const std::string& key);
    std::unordered_map<std::string, std::any> getAll();

private:
    std::string envFile_;
};

// Global registry instance

// CLI setup
void setupCLI();

// Run CLI if called directly or from bin script

} // namespace elizaos
