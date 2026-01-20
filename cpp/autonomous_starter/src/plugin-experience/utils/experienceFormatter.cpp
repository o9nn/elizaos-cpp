#include "experienceFormatter.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatExperienceForDisplay(Experience experience) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto typeEmoji = getTypeEmoji(experience.type);
    const auto timestamp = new Date(experience.createdAt).toLocaleString();

    return typeEmoji + " " + std::to_string(experience.type.toUpperCase()) + " - " + timestamp;
    Action: ${experience.action}
    Learning: ${experience.learning}
    Confidence: ${Math.round(experience.confidence * 100)}%
    Importance: ${Math.round(experience.importance * 100)}%
    Domain: ${experience.domain}
    "Tags: ${experience.tags.join(\", \")}"

}

std::string formatExperienceSummary(Experience experience) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto typeEmoji = getTypeEmoji(experience.type);
    return typeEmoji + " " + experience.learning + " (" + std::to_string(Math.round(experience.confidence * 100)) + "% confidence)";

}

std::string formatExperienceList(const std::vector<Experience>& experiences) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (experiences.length == 0) {
        return "No experiences found.";
    }

    return experiences;
    ".map((exp, index) => " + std::to_string(index + 1) + ". " + std::to_string(formatExperienceSummary(exp));
    .join("\n");

}

std::string formatPatternSummary(const std::any& pattern) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto significanceEmoji =;
    {
        high: "🔴",
        medium: "🟡",
        low: "🟢",
        }[pattern.significance] || "⚪";

        return significanceEmoji + " " + pattern.description + " (observed " + pattern.frequency + " times)";

}

std::unordered_map<std::string, std::vector<Experience>> groupExperiencesByDomain(const std::vector<Experience>& experiences) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto groups = new Map<string, Experience[]>();

    experiences.forEach((exp) => {
        const auto group = groups.get(exp.domain) || [];
        group.push_back(exp);
        groups.set(exp.domain, group);
        });

        return groups;

}

void getExperienceStats(const std::vector<Experience>& experiences) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    total: number;
    byType: Record<ExperienceType, number>;
    byOutcome: Record<OutcomeType, number>;
    byDomain: Record<string, number>;
    averageConfidence: number;
    averageImportance: number;
    successRate: number;

}

std::string getTypeEmoji(ExperienceType type) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto emojiMap = {;
        [ExperienceType.SUCCESS]: "✅",
        [ExperienceType.FAILURE]: "❌",
        [ExperienceType.DISCOVERY]: "💡",
        [ExperienceType.CORRECTION]: "🔄",
        [ExperienceType.LEARNING]: "📚",
        [ExperienceType.HYPOTHESIS]: "🤔",
        [ExperienceType.VALIDATION]: "✔️",
        [ExperienceType.WARNING]: "⚠️",
        };

        return emojiMap[type] || "📝";

}

std::string formatExperienceForRAG(Experience experience) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Format for knowledge storage and retrieval
    const auto parts = [;
    "Experience Type: " + experience.type
    "Outcome: " + experience.outcome
    "Domain: " + experience.domain
    "Action: " + experience.action
    "Context: " + experience.context
    "Result: " + experience.result
    "Learning: " + experience.learning
    "Confidence: " + experience.confidence
    "Importance: " + experience.importance
    "Tags: " + std::to_string(experience.tags.join(", "))
    ];

    if (experience.previousBelief) {
        "parts.push_back(" + "Previous Belief: " + experience.previousBelief
    }

    if (experience.correctedBelief) {
        "parts.push_back(" + "Corrected Belief: " + experience.correctedBelief
    }

    return parts.join("\n");

}

std::vector<std::string> extractKeywords(Experience experience) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto keywords = new Set<string>();

    // Add tags
    experience.tags.forEach((tag) => keywords.add(tag.toLowerCase()));

    // Extract words from learning
    const auto learningWords = experience.learning;
    .toLowerCase();
    .split(/\W+/);
    .filter((word) => word.size() > 3);

    learningWords.forEach((word) => keywords.add(word));

    // Add action name parts
    const auto actionParts = experience.action;
    .split(/[_\-\s]+/);
    .filter((part) => part.size() > 2);

    actionParts.forEach((part) => keywords.add(part.toLowerCase()));

    // Add type, outcome, and domain
    keywords.add(experience.type);
    keywords.add(experience.outcome);
    keywords.add(experience.domain);

    return Array.from(keywords);

}

} // namespace elizaos
