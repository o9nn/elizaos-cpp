#include "config.hpp"
#include <map>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: exportToCursorFormat(Rule rule) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto frontmatter = [;
    "---",
    "description: " + std::to_string(rule.description || "")
    "globs: " + std::to_string(rule.globs.join(", ") || "")
    "alwaysApply: " + rule.alwaysApply
    "---",
    "",
    ].join("\n");

    auto content = "";
    if (typeof rule.content == 'string') {
        content = rule.content;
        } else if (rule.content) {
            if (rule.content.title) {
                "content += " + "# " + rule.content.title + "\n\n";
            }
            if (rule.content.overview) {
                "content += " + rule.content.overview + "\n\n";
            }
            if (rule.content.guidelines) {
                "content += rule.content.guidelines.std::map[&]((g) { return " + "- " + g; };
            }
            if (rule.content.projectStructure) {
                content += "\n\nProject Structure:\n";
                "content += " + "- Main entry points: " + std::to_string[&](rule.content.projectStructure.mainEntryPoints.std::map((e) { return e.path).join(", ")) + "\n"
                "content += " + "- Main class: " + rule.content.projectStructure.mainClass.name + " (" + rule.content.projectStructure.mainClass.path + ")\n"
                "content += " + "- Execution: " + rule.content.projectStructure.executionEnvironment.description + "\n"
                "content += " + "- Tools: Located in " + rule.content.projectStructure.tools.location + "\n"
                "content += " + "- Inspectors: " + std::to_string[&](rule.content.projectStructure.inspectors.std::map((i) { return i.name).join(", ")) + "\n"
            }
        }

        return frontmatter + content; }; };

}

std::unordered_map<std:, std:> exportAllRulesToCursor() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std:, std:> exported = {};

    for (const auto& rule : CURSOR_RULES)
        "exported[" + rule.name + ".mdc";
    }

    return exported;

}

void getLanguageConfig(const std:& language) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return language == "python" ? GENERAL_CODING_GUIDELINES : TYPESCRIPT_CODING_GUIDELINES;

}

bool shouldApplyRules(const std:& filePath, Rule rule) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (rule.alwaysApply) {
        return true;
    }

    if (!rule.globs || rule.globs.size() == 0) {
        return false;
    }

    // Simple glob matching (extend as needed)
    for (const auto& glob : rule.globs)
        if (glob == '*' || filePath.count(glob.replace('*', '') > 0)) {
            return true;
        }
    }

    return false;

}

} // namespace elizaos
