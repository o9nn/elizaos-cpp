#pragma once
#include <functional>
#include <future>
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
std::future<std::string> extractCommandFromMessage(IAgentRuntime runtime, Memory message);

// New helper function to quote arguments for shell commands like find and grep
std::string quoteShellArgs(const std::string& command);

// Helper function to save execution record to message feed
std::future<void> saveExecutionRecord(IAgentRuntime runtime, Memory messageContext, const std::string& text, std::optional<std::vector<std::string>> actions, std::optional<Media[] // Added attachments parameter> attachments);

    // Check if shell capability is enabled in runtime settings

      // No direct shell output to attach here, so save a simple record

          // Windows commands

    // Apply quoting for wildcards using the new helper void if(auto commandToRun);

      // 1. Package raw output as an attachment

      // 2. Generate summary/reflection with LLM

      // Conditionally mention the attachment

      // 3. Create the main reflection/summary message

      // 4. Callback with the summary

    // Multi-action: Run command then clear history
    // Multi-action: Check processes then kill autonomous
          // attachments: [ { id: '...', title: 'Shell Output: ls -la', ...} ] // Example of what might be here

    // Check if shell capability is enabled in runtime settings

    // Multi-action: Run sensitive command then clear

} // namespace elizaos
