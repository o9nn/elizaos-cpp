#include "general.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void validateAgainstRules(const std::string& code, CodingGuidelines guidelines) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    valid: boolean; violations: string[]
}

std::vector<CodingRule> getApplicableRules(const std::string& filePath, std::optional<std::variant<'python', 'typescript'>> language) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lang = language || (filePath.endsWith('.py') ? 'python' : 'typescript');
    return lang == 'python' ? PYTHON_CODING_RULES : TYPESCRIPT_CODING_RULES;

}

} // namespace elizaos
