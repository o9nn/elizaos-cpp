#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".services/plugin-creation-service.hpp"
#include ".utils/validation.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Zod schema for plugin specification validation

    // Check if there's already an active job

    // Validate the message contains valid JSON

    return validatePrompt(message);

      // Parse and validate specification

      // Check for job ID in message

        // Get the most recent active job

          // Get the most recent job of any status

      // Generate specification from description

      // Validate the generated specification

// Enhanced helper function to generate plugin specification from natural language
std::future<PluginSpecification> generatePluginSpecification(const std::string& description, IAgentRuntime runtime);

} // namespace elizaos
