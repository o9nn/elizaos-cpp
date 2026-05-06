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



// Test IDs

// Test data for memory integration tests

// Helper std::function to generate random embedding vectors
  // Normalize

// Basic memory test objects

// Memory test objects with embeddings

// Document and fragments for testing document operations

// Fragment memories that belong to the document

// Helper std::function to create similar memory for std::vector similarity testing
  // Only works if baseMemory has an embedding

  // Create a somewhat similar std::vector (higher similarity means more similar)

  // Blend the original std::vector with noise based on similarity

  // Normalize the resulting std::vector


} // namespace elizaos
