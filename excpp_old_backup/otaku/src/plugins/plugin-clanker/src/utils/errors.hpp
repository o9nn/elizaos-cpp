#include ".types.hpp"
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



class ClankerError extends Error {
  code: ErrorCode;
  details?: any;
  suggestions?: string[];

  constructor(
    code: ErrorCode,
    message: string,
    details?: any,
    suggestions?: string[],
  ) {
    super(message);
    this.name = "ClankerError";
    this.code = code;
    this.details = details;
    this.suggestions = suggestions;
  }

ErrorResponse handleError(unknown error);

bool validateAddress(const std::string& address);

bool validateAmount(const std::string& amount);

} // namespace elizaos
