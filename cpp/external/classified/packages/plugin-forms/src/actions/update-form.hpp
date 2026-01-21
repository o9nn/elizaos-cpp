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
 * UpdateFormAction processes user messages to extract form field values
 * and updates the active form accordingly.
 */

    // Check if forms service is available

    // Check if there are any active forms

    // Check if the message seems to contain form input

    // Simple heuristics to detect if user is providing form input
    // This could be enhanced with more sophisticated detection

    // Get active forms

    // Check if a specific form ID was provided

    // Update the form with the message

    // Prepare response based on update result

    // Add form state to response


} // namespace elizaos
