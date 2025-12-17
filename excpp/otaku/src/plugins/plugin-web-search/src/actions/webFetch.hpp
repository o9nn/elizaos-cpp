#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".services/firecrawlService.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::string MaxTokens(const std::string& data, double maxTokens = DEFAULT_MAX_FETCH_CHARS);

    // Parameter schema for tool calling

            // Read parameters from state (extracted by multiStepDecisionTemplate)
            
            // Support both actionParams (new pattern) and webFetch (legacy pattern)
            
            // Extract and validate URL parameter (required)

            // Validate URL format
                new URL(url);

            // Store input parameters for return

            // Use provided parameters or defaults

                // Build response text with markdown content

            // Try to capture input params even in failure


} // namespace elizaos
