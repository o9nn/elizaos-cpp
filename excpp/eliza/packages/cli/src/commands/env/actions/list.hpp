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
 * Displays system information and lists local environment variables, masking sensitive values.
 *
 * Prints details about the current platform, architecture, CLI version, and package manager. Shows environment variables from the project's `.env` file, masking sensitive values, or provides guidance if the file is missing. Includes a link to the web UI for editing variables.
 */
std::future<void> listEnvVars();

/**
 * Handle the list command with options
 */
std::future<void> handleListCommand(ListEnvOptions options);

} // namespace elizaos
