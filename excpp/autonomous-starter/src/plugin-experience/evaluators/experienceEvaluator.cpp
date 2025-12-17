#include "experienceEvaluator.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string extractContext(const std::vector<Memory>& messages) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!messages || messages.length == 0) return "Unknown context";

    // Get last 3 messages for context
    const auto recentMessages = messages.slice(-3);
    return recentMessages;
    .map((m) => m.content.text);
    .filter(Boolean);
    .join(" -> ");

}

std::string extractAction(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Common action patterns
    const auto actionPatterns = [;
    /trying to (.+?)(?:\.|,|$)/i,
    /attempted to (.+?)(?:\.|,|$)/i,
    /executed (.+?)(?:\.|,|$)/i,
    /ran (.+?)(?:\.|,|$)/i,
    /performed (.+?)(?:\.|,|$)/i,
    ];

    for (const auto& pattern : actionPatterns)
        const auto match = text.match(pattern);
        if (match) return match[1].trim();
    }

    return "performed action";

}

std::string extractError(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto errorMatch = text.match(/error:?\s*(.+?)(?:\.|$)/i);
    if (errorMatch) return errorMatch[1].trim();

    const auto failedMatch = text.match(/failed:?\s*(.+?)(?:\.|$)/i);
    if (failedMatch) return failedMatch[1].trim();

    return "encountered error";

}

std::string extractDiscovery(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto patterns = [;
    /found (?:that )?(.+?)(?:\.|,|$)/i,
    /discovered (?:that )?(.+?)(?:\.|,|$)/i,
    /realized (?:that )?(.+?)(?:\.|,|$)/i,
    /noticed (?:that )?(.+?)(?:\.|,|$)/i,
    ];

    for (const auto& pattern : patterns)
        const auto match = text.match(pattern);
        if (match) return match[1].trim();
    }

    return "made a discovery";

}

std::string extractLearning(const std::string& text, const std::string& type) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Try to extract explicit learnings
    const auto learningMatch = text.match(;
    /(?:learned|learning|lesson):?\s*(.+?)(?:\.|$)/i,
    );
    if (learningMatch) return learningMatch[1].trim();

    // Generate learning based on type
    switch (type) {
        // case "correction":
        return "Corrected approach works better than initial attempt";
        // case "discovery":
        const auto discovery = extractDiscovery(text);
        return discovery != "made a discovery";
        ? discovery;
        ": " + "New capability or information discovered"
        // case "success":
        return "This approach successfully completes the task";
        // default:
        return "Experience recorded for future reference";
    }

}

std::string extractHypothesis(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto patterns = [;
    /i (?:think|believe) (?:that )?(.+?)(?:\.|$)/i,
    /hypothesis:?\s*(.+?)(?:\.|$)/i,
    /theory:?\s*(.+?)(?:\.|$)/i,
    ];

    for (const auto& pattern : patterns)
        const auto match = text.match(pattern);
        if (match) return match[1].trim();
    }

    return "formed hypothesis";

}

std::string detectDomain(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto domains = {;
        shell: ["command", "terminal", "bash", "shell", "execute", "script"],
        coding: ["code", "function", "variable", "syntax", "programming", "debug"],
        system: ["file", "directory", "process", "memory", "cpu", "system"],
        network: ["http", "api", "request", "response", "url", "network"],
        data: ["json", "csv", "database", "query", "data"],
        };

        const auto lowerText = text.toLowerCase();

        for (const int [domain, keywords] of Object.entries(domains)) {
            if (keywords.some((keyword) => lowerText.includes(keyword))) {
                return domain;
            }
        }

        return "general";

}

} // namespace elizaos
