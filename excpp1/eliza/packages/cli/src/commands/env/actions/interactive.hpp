#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "edit.hpp"
#include "list.hpp"
#include "reset.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Display the main menu for environment variables
 *
 * Provides an interactive menu system for managing environment variables, including listing, editing, and resetting operations.
 */
std::future<void> showMainMenu(InteractiveEnvOptions options);

} // namespace elizaos
