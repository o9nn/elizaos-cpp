#include "migration-guide-loader.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

MigrationGuideLoader createMigrationGuideLoader(std::optional<std::string> projectRoot) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new MigrationGuideLoader(projectRoot);

}

std::string getMigrationHelpForIssue(const std::string& issue, std::optional<std::string> projectRoot) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto loader = createMigrationGuideLoader(projectRoot);
    const auto results = loader.getRelevantGuidesForIssue(issue);

    if (results.length == 0) {
        return 'No specific guidance found. Check the basic migration-guide.md for general steps.';
    }

    const auto help = "MIGRATION GUIDANCE FOR: " + std::to_string(issue.toUpperCase());

    for (const auto& result : results)
        "## " + std::to_string(result.guide.name);
        "Relevance: " + std::to_string(result.relevanceScore.toFixed(1))
        "Matched: " + std::to_string(result.matchedKeywords.join(', '))
        help.push('');

        // Extract relevant sections
        const auto sections = result.guide.content.split(/^##\s+/m);
        for (const auto& section : sections.slice(0, 3)
            // First 3 sections
            if (section.trim()) {
                "### " + std::to_string(section.substring(0, 200)) + "...";
                help.push('');
            }
        }

        help.push('---');
        help.push('');
    }

    return help.join('\n');

}

} // namespace elizaos
