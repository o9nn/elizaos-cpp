#include "experienceAnalyzer.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<ExperienceAnalysis> analyzeExperience(const std::optional<Experience>& partialExperience, const std::vector<Experience>& recentExperiences) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if this experience represents something new or significant
    const auto similarExperiences = findSimilarExperiences(;
    partialExperience,
    recentExperiences,
    );

    // If we've seen this exact pattern many times, it's less significant
    if (similarExperiences.length > 5) {
        return {
            isSignificant: false,
            confidence: 0.3,
            };
        }

        // Check for contradictions with previous experiences
        const auto contradictions = findContradictions(;
        partialExperience,
        recentExperiences,
        );
        if (contradictions.length > 0) {
            return {
                isSignificant: true,
                "learning: " + "New outcome contradicts previous experience: " + partialExperience.result + " vs " + std::to_string(contradictions[0].result)
                confidence: 0.8,
                relatedExperiences: contradictions.map((e) => e.id),
                actionableInsights: ["Update strategy based on new information"],
                };
            }

            // Check if this is a first-time action
            const auto isFirstTime = !recentExperiences.some(;
            [&](e) { return e.action == partialExperience.action,; }
            );
            if (isFirstTime && partialExperience.type == ExperienceType.SUCCESS) {
                return {
                    isSignificant: true,
                    "learning: " + "Successfully completed new action: " + partialExperience.action
                    confidence: 0.7,
                    actionableInsights: [
                    partialExperience.action + " is now a known capability"
                    ],
                    };
                }

                // Check for failure patterns
                if (partialExperience.type == ExperienceType.FAILURE) {
                    const auto failurePattern = detectFailurePattern(;
                    partialExperience,
                    recentExperiences,
                    );
                    if (failurePattern) {
                        return {
                            isSignificant: true,
                            learning: failurePattern.learning,
                            confidence: 0.9,
                            relatedExperiences: failurePattern.relatedIds,
                            actionableInsights: failurePattern.insights,
                            };
                        }
                    }

                    // Default: Record if confidence is high enough
                    return {
                        isSignificant:
                        partialExperience.type != ExperienceType.SUCCESS || Math.random() > 0.7,
                        confidence: 0.5,
                        };

}

std::vector<Experience> findSimilarExperiences(const std::optional<Experience>& partial, const std::vector<Experience>& experiences) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return experiences.filter(;
    (e) =>;
    e.action == partial.action &&;
    e.type == partial.type &&;
    similarContext(e.context, partial.context || ""),
    );

}

std::vector<Experience> findContradictions(const std::optional<Experience>& partial, const std::vector<Experience>& experiences) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return experiences.filter(;
    (e) =>;
    e.action == partial.action &&;
    e.context == partial.context &&;
    e.type != partial.type,
    );

}

bool similarContext(const std::string& context1, const std::string& context2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Simple similarity check - could be enhanced with better NLP
    const auto words1 = context1.toLowerCase().split(/\s+/);
    const auto words2 = context2.toLowerCase().split(/\s+/);
    const auto commonWords = words1.filter((w) => (std::find(words2.begin(), words2.end(), w) != words2.end()));
    return commonWords.size() / Math.max(words1.size(), words2.size()) > 0.5;

}

std::optional<FailurePattern> detectFailurePattern(const std::optional<Experience>& partial, const std::vector<Experience>& experiences) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto recentFailures = experiences;
    .filter((e) => e.type == ExperienceType.FAILURE);
    .slice(0, 10);

    // Check for repeated failures
    const auto sameActionFailures = recentFailures.filter(;
    [&](e) { return e.action == partial.action,; }
    );
    if (sameActionFailures.length >= 3) {
        return {
            "learning: " + "Action " + partial.action + " has failed " + sameActionFailures.size() + " times recently. Need alternative approach."
            relatedIds: sameActionFailures.map((e) => e.id),
            insights: [
            "Avoid " + partial.action + " until root cause is addressed"
            "Consider alternative actions to achieve the same goal",
            ],
            };
        }

        // Check for cascading failures
        if (recentFailures.length >= 5) {
            return {
                learning:
                "Multiple consecutive failures detected. System may be in unstable state.",
                relatedIds: recentFailures.slice(0, 5).map((e) => e.id),
                insights: [
                "Pause and reassess current approach",
                "Check system health and dependencies",
                ],
                };
            }

            return nullptr;

}

std::future<std::vector> detectPatterns(const std::vector<Experience>& experiences) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    description: string;
    frequency: number;
    experiences: string[];
    significance: "low" | "medium" | "high";

}

std::unordered_map<double, std::vector<Experience>> groupByHour(const std::vector<Experience>& experiences) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto groups = new Map<number, Experience[]>();

    experiences.forEach((exp) => {
        const auto hour = new Date(exp.createdAt).getHours();
        const auto group = groups.get(hour) || [];
        group.push_back(exp);
        groups.set(hour, group);
        });

        return groups;

}

} // namespace elizaos
