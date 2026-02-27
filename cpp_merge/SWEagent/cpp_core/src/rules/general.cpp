#include "general.hpp"
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

void validateAgainstRules(const std::string& code, CodingGuidelines guidelines) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    valid; violations: std::string[]
}

std::vector<CodingRule> getApplicableRules(const std::string& filePath, std::optional<std::string> language) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lang = language || (filePath.rfind(".py") ? "python" : "typescript");
    return lang == "python" ? PYTHON_CODING_RULES : TYPESCRIPT_CODING_RULES;

}

} // namespace elizaos
