#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "user-environment.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * ModuleLoader provides a clean way to load modules from the project's local node_modules
 * instead of the global CLI's bundled dependencies. This solves singleton pattern issues
 * and ensures consistent module instances across the application.
 */
class ModuleLoader {
public:
    ModuleLoader(std::optional<std::string> projectPath);
    std::string detectProjectPath();
    void if(auto isLocalModule);
    void catch(auto error);
    void if(auto isLocalModule);
    void catch(auto error);
    void setupEnvironment();
    void clearCache();
    std::string resolve(const std::string& moduleName);

private:
    NodeRequire require_;
    std::string projectPath_;
};

// Singleton instance for the current project

/**
 * Get the default module loader instance for the current project.
 * Creates a new instance if one doesn't exist.
 */
ModuleLoader getModuleLoader();

/**
 * Convenience function to load a module using the default loader.
 *
 * @param moduleName - The name of the module to load
 * @returns The loaded module
 */

/**
 * Convenience function to synchronously load a module using the default loader.
 *
 * @param moduleName - The name of the module to load
 * @returns The loaded module
 */

} // namespace elizaos
