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
#include "elizaos/plugin-forms.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Extract project name from user message
 */
std::string extractProjectName(const std::string& text);

/**
 * Extract project type from user message
 */
ProjectType extractProjectType(const std::string& text);

void getFormTemplate(const std::string& projectType);

    // Check for project creation keywords

    // Also check for "I want to..." patterns

    // Create form for project details

    // Create the form

    // Store project metadata

    // Send response with form


} // namespace elizaos
