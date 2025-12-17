#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define a mock runtime interface for tests
struct MockRuntime {
};

// Mock the OpenAI plugin
      // Mock init

// Mock the Anthropic plugin
      // Mock init

// Mock the Groq plugin
      // Mock init

      // Check OLLAMA_AVAILABLE flag first

      // If explicitly set to false, ollama is not available

      // Default to available if not explicitly disabled
      // Mock init

// Mock the ElizaOS Services plugin
      // Only throw if no API key

// Mock the local embedding plugin
      // Mock init

// Mock fetch to prevent real API calls

} // namespace elizaos
