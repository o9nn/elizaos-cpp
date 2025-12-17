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



    // Only evaluate agent's own messages

      // Detect different types of experiences

      // Get relevant experiences using the RAG provider

      // 1. Detect failures and corrections
        // Check if there's a subsequent success

          // Look for contradictions in previous experiences

      // 2. Detect discoveries

      // 3. Detect successful completions and validate expectations

      // 4. Detect hypotheses or plans

      // 5. Check for pattern recognition and analyze domain
        // Lowered threshold for testing

      // 6. Analyze domain trends
        // This should be based on whether any experience was recorded above

      // If no specific experience was detected and recorded by other checks,
      // consider recording a general learning experience if the message is from the agent.
      // This is a fallback to ensure agent's utterances can be captured if not fitting other patterns.
      // Fallback: Record a general learning experience about the error

// Helper functions

std::string extractContext(const std::vector<Memory>& messages);

std::string extractAction(const std::string& text);

std::string extractError(const std::string& text);

std::string extractDiscovery(const std::string& text);

std::string extractLearning(const std::string& text, const std::string& type);

std::string extractHypothesis(const std::string& text);

std::string detectDomain(const std::string& text);

} // namespace elizaos
