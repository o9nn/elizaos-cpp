#include "config.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string exportToCursorFormat(Rule rule) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto frontmatter = [;
    '---',
    "description: " + std::to_string(rule.description || '')
    "globs: " + std::to_string(rule.globs.join(', ') || '')
    "alwaysApply: " + std::to_string(rule.alwaysApply)
    '---',
    '',
    ].join('\n');

    auto content = '';
    if (typeof rule.content == 'string') {
        content = rule.content;
        } else if (rule.content) {
            if (rule.content.title) {
                "# " + std::to_string(rule.content.title) + "\n\n";
            }
            if (rule.content.overview) {
                std::to_string(rule.content.overview) + "\n\n";
            }
            if (rule.content.guidelines) {
                "- " + std::to_string(g);
            }
            if (rule.content.projectStructure) {
                content += '\n\nProject Structure:\n';
                "- Main entry points: " + std::to_string(rule.content.projectStructure.mainEntryPoints.map((e) => e.path).join(', ')) + "\n"
                "- Main class: " + std::to_string(rule.content.projectStructure.mainClass.name) + " (" + std::to_string(rule.content.projectStructure.mainClass.path) + ")\n"
                "- Execution: " + std::to_string(rule.content.projectStructure.executionEnvironment.description) + "\n"
                "- Tools: Located in " + std::to_string(rule.content.projectStructure.tools.location) + "\n"
                "- Inspectors: " + std::to_string(rule.content.projectStructure.inspectors.map((i) => i.name).join(', ')) + "\n"
            }
        }

        return frontmatter + content;

}

std::unordered_map<std::string, std::string> exportAllRulesToCursor() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::string> exported = {};

    for (const auto& rule : CURSOR_RULES)
        std::to_string(rule.name) + ".mdc";
    }

    return exported;

}

void getLanguageConfig(const std::string& language) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return language == 'python' ? GENERAL_CODING_GUIDELINES : TYPESCRIPT_CODING_GUIDELINES;

}

bool shouldApplyRules(const std::string& filePath, Rule rule) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (rule.alwaysApply) {
        return true;
    }

    if (!rule.globs || rule.globs.length == 0) {
        return false;
    }

    // Simple glob matching (extend as needed)
    for (const auto& glob : rule.globs)
        if (glob == '*' || filePath.includes(glob.replace('*', ''))) {
            return true;
        }
    }

    return false;

}

} // namespace elizaos
