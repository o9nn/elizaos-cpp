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

/**
 * Shared error types and utilities for ElizaOS
 */

enum ErrorCode {
  // Authentication
  UNAUTHORIZED = 'UNAUTHORIZED',
  INVALID_API_KEY = 'INVALID_API_KEY',

  // Validation
  INVALID_REQUEST = 'INVALID_REQUEST',
  MISSING_FIELD = 'MISSING_FIELD',

  // Resources
  NOT_FOUND = 'NOT_FOUND',
  ALREADY_EXISTS = 'ALREADY_EXISTS',

  // System
  INTERNAL_ERROR = 'INTERNAL_ERROR',
  NOT_IMPLEMENTED = 'NOT_IMPLEMENTED',
  RATE_LIMITED = 'RATE_LIMITED',
}

class ElizaError extends Error {
  constructor(
    public code: ErrorCode,
    message: string,
    public statusCode: number = 500,
    public details?: any
  ) {
    super(message);
    this.name = 'ElizaError';
  }

void createErrorResponse(unknown error);

} // namespace elizaos
