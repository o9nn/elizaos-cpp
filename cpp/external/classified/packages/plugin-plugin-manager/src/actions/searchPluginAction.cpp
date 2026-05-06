#include "searchPluginAction.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string extractSearchQuery(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Patterns for query extraction
    const auto patterns = [;
    // Direct search patterns
    /search\s+for\s+plugins?\s+(?:that\s+)?(?:can\s+)?(.+)/i,
    /find\s+plugins?\s+(?:for|that|to)\s+(.+)/i,
    /look\s+for\s+plugins?\s+(?:that\s+)?(.+)/i,
    /discover\s+plugins?\s+(?:for|that)\s+(.+)/i,
    /show\s+me\s+plugins?\s+(?:for|that)\s+(.+)/i,

    // Need-based patterns
    /need\s+(?:a\s+)?plugins?\s+(?:for|that|to)\s+(.+)/i,
    /want\s+(?:a\s+)?plugins?\s+(?:for|that|to)\s+(.+)/i,

    // Capability-based patterns
    /plugins?\s+(?:for|that\s+can|to)\s+(.+)/i,
    /what\s+plugins?\s+(?:can|do|handle)\s+(.+)/i,

    // Simple patterns
    /plugins?\s+(.+)/i,
    ];

    for (const auto& pattern : patterns)
        const auto match = text.match(pattern);
        if (match && match[1]) {
            auto query = match[1].trim();

            // Clean up common artifacts
            query = query.replace(/\?+$/, ""); // Remove trailing question marks;
            query = query.replace(/^(do|handle|manage|work\s+with)\s+/i, ""); // Remove action words;
            query = query.replace(/\s+/g, " "); // Normalize whitespace;

            if (query.length > 2) {
                return query;
            }
        }
    }

    // If no pattern matches, try to extract technology/domain keywords
    const auto techKeywords = text.match(;
    /\b(blockchain|ai|database|api|social|twitter|discord|telegram|solana|ethereum|trading|defi|nft|authentication|security|monitoring|analytics|file|image|video|audio|email|sms|payment)\b/gi;
    );

    if (techKeywords && techKeywords.length > 0) {
        return techKeywords.join(" ");
    }

    return nullptr;

}

} // namespace elizaos
