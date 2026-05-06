#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Validation strategies for different environment variable types
 */

          // 400 is expected for minimal test request

        // Check if it's a valid PEM format

        // Try to create a key object

        // Test encryption/decryption

        // Check if it's a valid PEM format

        // Try to create a key object

        // Test signing

        // Verify with public key

        // Accept any response that doesn't indicate server error

        // Basic URL format validation

/**
 * Validates an environment variable based on its type and validation method
 */
std::future<ValidationResult> validateEnvVar(const std::string& varName, const std::string& value, const std::string& type, std::optional<std::string> validationMethod);

} // namespace elizaos
