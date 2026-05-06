#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// The terminal provider should provide the terminal history for the current active terminal session
// Get the current state from the service and format as text

 // Import ShellService

    // Assuming ShellService.getHistory() returns all relevant (potentially summarized by service) history
    // And that ShellService itself handles the 100k char limit and summarization (e.g. keeping last 3 turns).

        // Raw data, not truncated for display here, but source output/error might be huge.

} // namespace elizaos
