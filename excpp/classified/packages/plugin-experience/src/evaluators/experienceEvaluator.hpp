#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".service.hpp"
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



    // Only run every 10 messages and only on agent messages

    // Check cooldown - only extract experiences every 10 messages

    // Trigger extraction every 10 messages

    // Get last 10 messages as context for analysis

    // Combine recent messages into analysis context

    // Query existing experiences for similarity check

    // Use LLM to extract novel experiences from the conversation

    // Record each novel experience
      // Max 3 experiences per extraction

// Helper functions

std::string sanitizeContext(const std::string& text);

std::string detectDomain(const std::string& text);

} // namespace elizaos
