#include ".services/tavilyService.hpp"
#include "elizaos/core.hpp"
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



std::string MaxTokens(const std::string& data, number = DEFAULT_MAX_WEB_SEARCH_CHARS maxTokens);

    // Parameter schema for tool calling

            // Read parameters from state (extracted by multiStepDecisionTemplate)
            
            // Support both actionParams (new pattern) and webSearch (legacy pattern)
            
            // Extract and validate query parameter (required)

            // Build enhanced query with source if provided

            // Store input parameters for return

            // Use provided parameters or defaults

            // Try to capture input params even in failure
            
            
} // namespace elizaos
