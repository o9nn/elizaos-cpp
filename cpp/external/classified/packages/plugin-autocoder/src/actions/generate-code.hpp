#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "elizaos/plugin-forms.hpp"
#include "services/CodeGenerationService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



    // Check if this is a form submission

    // Check for code generation keywords

    // Extract project type and data

    // Handle form flow
      // Check if we have active forms

        // We have an incomplete form, prompt for next step

      // Check if this is a form submission
        // Process form submission

        // Start generation

        // Prepare success response

    // Direct code generation without form

// Helper functions
std::future<std::string> extractProjectType(IAgentRuntime runtime, Memory message, std::optional<State> _state);

std::vector<std::string> extractRequirements(const std::string& text);

std::vector<std::string> extractAPIs(const std::string& text);

std::string extractProjectName(const std::string& text);

std::string getNextStepPrompt(const std::any& form, const std::string& completedStepId);

std::any extractProjectData(const std::any& formData);

} // namespace elizaos
