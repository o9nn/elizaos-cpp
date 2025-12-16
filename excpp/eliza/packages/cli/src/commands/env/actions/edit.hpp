#include ".types.hpp"
#include ".utils/file-operations.hpp"
#include ".utils/validation.hpp"
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



/**
 * Interactive environment variable editor for local .env files.
 *
 * Provides an interactive menu to edit existing variables, add new variables, and delete variables from the local .env file. Supports auto-confirmation mode for non-interactive usage.
 *
 * @param options - Edit command options
 * @param fromMainMenu - Whether this command was called from the main menu (affects return behavior)
 * @returns Promise<boolean> - Whether to return to main menu
 */
std::future<bool> editEnvVars(EditEnvOptions options, auto fromMainMenu = false);

/**
 * Helper function to add a new environment variable
 * @param envPath Path to the .env file
 * @param envVars Current environment variables
 * @param yes Whether to auto-confirm prompts
 */
std::future<void> addNewVariable(const std::string& envPath, EnvVars envVars, auto yes = false);

} // namespace elizaos
