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
#include "services/forms-service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * CreateFormAction initializes a new form from a template or custom definition.
 */

    // Check if forms service is available

    // Check if user wants to create a form

    // Extract form type from message or options

    // Create the form

    // Get first step information

/**
 * Extract form type from user message
 */
std::string extractFormType(const std::string& text);

} // namespace elizaos
