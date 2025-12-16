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
 * Interface for defining the structure of an API response.
 * @template T - The type of data included in the response.
 */
interface ApiResponse<T> {
  success: boolean;
  data?: T;
  error?: {
    code: string;
    message: string;
    details?: unknown;
  };
}

/**
 * Defines the structure of AgentBasic interface.
 */
struct AgentBasic {
    std::string id;
    std::string name;
    std::optional<std::string> status;
};


} // namespace elizaos
