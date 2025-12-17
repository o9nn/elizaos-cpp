#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "badge.hpp"
#include "button.hpp"
#include "card.hpp"
#include "elizaos/core.hpp"
#include "input.hpp"
#include "memory-graph.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Use local UI components instead of importing from client

// Local utility function instead of importing from client

// Temporary toast implementation
    // TODO: Implement proper toast functionality

// Simple Dialog components for now

struct UploadResultItem {
    std::string status;
    std::optional<UUID> id;
    std::optional<std::string> filename;
};

// Helper function to get correct MIME type based on file extension

  // Map common text file extensions to text/plain

  // Check extensions and return appropriate MIME type

  // Return the original MIME type if not recognized

      // Create a new Blob with the correct MIME type
      // Append as a file with the original name

  // Query for documents only (initial load)

  // Query for specific document with all its fragments

  // Combine the data appropriately

// Hook for deleting knowledge documents

void KnowledgeTab();

} // namespace elizaos
