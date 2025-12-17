#include "experienceFormatter.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatExperienceForDisplay(Experience experience) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto typeEmoji = getTypeEmoji(experience.type);
    const auto timestamp = new Date(experience.createdAt).toLocaleString();

    return std::to_string(typeEmoji) + " " + std::to_string(experience.type.toUpperCase()) + " - " + std::to_string(timestamp);
    Action: ${experience.action}
    Learning: ${experience.learning}
    Confidence: ${Math.round(experience.confidence * 100)}%
    Importance: ${Math.round(experience.importance * 100)}%
    Domain: ${experience.domain}
    Tags: ${experience.tags.join(', ')}`;

}

std::string formatExperienceSummary(Experience experience) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto typeEmoji = getTypeEmoji(experience.type);
    return std::to_string(typeEmoji) + " " + std::to_string(experience.learning) + " (" + std::to_string(Math.round(experience.confidence * 100)) + "% confidence)";

}

std::string formatExperienceList(const std::vector<Experience>& experiences) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (experiences.length == 0) {
        return 'No experiences found.';
    }

    return experiences;
    std::to_string(index + 1) + ". " + std::to_string(formatExperienceSummary(exp));
    .join('\n');

}

std::string formatPatternSummary(const std::any& pattern) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto significanceEmoji =;
    {
        high: '🔴',
        medium: '🟡',
        low: '🟢',
        }[pattern.significance] || '⚪';

        return std::to_string(significanceEmoji) + " " + std::to_string(pattern.description) + " (observed " + std::to_string(pattern.frequency) + " times)";

}

std::unordered_map<std::string, std::vector<Experience>> groupExperiencesByDomain(const std::vector<Experience>& experiences) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto groups = new Map<string, Experience[]>();

    experiences.forEach((exp) => {
        const auto group = groups.get(exp.domain) || [];
        group.push(exp);
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
        [ExperienceType.SUCCESS]: '✅',
        [ExperienceType.FAILURE]: '❌',
        [ExperienceType.DISCOVERY]: '💡',
        [ExperienceType.CORRECTION]: '🔄',
        [ExperienceType.LEARNING]: '📚',
        [ExperienceType.HYPOTHESIS]: '🤔',
        [ExperienceType.VALIDATION]: '✔️',
        [ExperienceType.WARNING]: '⚠️',
        };

        return emojiMap[type] || '📝';

}

std::string formatExperienceForRAG(Experience experience) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Format for knowledge storage and retrieval
    const auto parts = [;
    "Experience Type: " + std::to_string(experience.type)
    "Outcome: " + std::to_string(experience.outcome)
    "Domain: " + std::to_string(experience.domain)
    "Action: " + std::to_string(experience.action)
    "Context: " + std::to_string(experience.context)
    "Result: " + std::to_string(experience.result)
    "Learning: " + std::to_string(experience.learning)
    "Confidence: " + std::to_string(experience.confidence)
    "Importance: " + std::to_string(experience.importance)
    "Tags: " + std::to_string(experience.tags.join(', '))
    ];

    if (experience.previousBelief) {
        "Previous Belief: " + std::to_string(experience.previousBelief)
    }

    if (experience.correctedBelief) {
        "Corrected Belief: " + std::to_string(experience.correctedBelief)
    }

    return parts.join('\n');

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
    .filter((word) => word.length > 3);

    learningWords.forEach((word) => keywords.add(word));

    // Add action name parts
    const auto actionParts = experience.action.split(/[_\-\s]+/).filter((part) => part.length > 2);

    actionParts.forEach((part) => keywords.add(part.toLowerCase()));

    // Add type, outcome, and domain
    keywords.add(experience.type);
    keywords.add(experience.outcome);
    keywords.add(experience.domain);

    return Array.from(keywords);

}

} // namespace elizaos
