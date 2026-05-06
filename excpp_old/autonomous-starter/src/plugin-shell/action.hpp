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

// run shell command

 // Import ShellService

// XML template for command extraction

// Helper function to extract command from natural language

// New helper function to quote arguments for shell commands like find and grep
std::string quoteShellArgs(const std::string& command);

// Helper function to save execution record to message feed

      // No direct shell output to attach here, so save a simple record

    // Apply quoting for wildcards using the new helper void if(auto commandToRun);

      // 1. Package raw output as an attachment

      // 2. Generate summary/reflection with LLM

      // Conditionally mention the attachment

      // 3. Create the main reflection/summary message

      // 4. Callback with the summary
          // attachments: [ { id: '...', title: 'Shell Output: ls -la', ...} ] // Example of what might be here

    // Always allow this action for debugging
      // Try to get the autonomous service and stop it


} // namespace elizaos
