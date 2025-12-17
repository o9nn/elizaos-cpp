#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Shared error types and utilities for ElizaOS
 */

enum ErrorCode {
  // Authentication

  // Validation

  // Resources

  // System
}

class ElizaError {
public:
    ElizaError(const std::string& message);
};

void createErrorResponse(unknown error);

} // namespace elizaos
