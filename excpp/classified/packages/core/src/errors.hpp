#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
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

void createErrorResponse(const std::any& error);

} // namespace elizaos
