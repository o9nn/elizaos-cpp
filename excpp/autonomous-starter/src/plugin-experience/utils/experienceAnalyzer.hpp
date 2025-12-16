#include ".types.js.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

struct ExperienceAnalysis {
    bool isSignificant;
    std::optional<std::string> learning;
    double confidence;
    std::optional<std::vector<std::string>> relatedExperiences;
    std::optional<std::vector<std::string>> actionableInsights;
};


std::future<ExperienceAnalysis> analyzeExperience(const std::optional<Experience>& partialExperience, const std::vector<Experience>& recentExperiences);;
  }

  // Check for contradictions with previous experiences
  const contradictions = findContradictions(
    partialExperience,
    recentExperiences,
  );
  if (contradictions.length > 0) {
    return {
      isSignificant: true,
      learning: `New outcome contradicts previous experience: ${partialExperience.result} vs ${contradictions[0].result}`,
      confidence: 0.8,
      relatedExperiences: contradictions.map((e) => e.id),
      actionableInsights: ["Update strategy based on new information"],
    };
  }

  // Check if this is a first-time action
  const isFirstTime = !recentExperiences.some(
    (e) => e.action === partialExperience.action,
  );
  if (isFirstTime && partialExperience.type === ExperienceType.SUCCESS) {
    return {
      isSignificant: true,
      learning: `Successfully completed new action: ${partialExperience.action}`,
      confidence: 0.7,
      actionableInsights: [
        `${partialExperience.action} is now a known capability`,
      ],
    };
  }

  // Check for failure patterns
  if (partialExperience.type === ExperienceType.FAILURE) {
    const failurePattern = detectFailurePattern(
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
      partialExperience.type !== ExperienceType.SUCCESS || Math.random() > 0.7,
    confidence: 0.5,
  };
}







struct FailurePattern {
    std::string learning;
    std::vector<std::string> relatedIds;
    std::vector<std::string> insights;
};


 has failed ${sameActionFailures.length} times recently. Need alternative approach.`,
      relatedIds: sameActionFailures.map((e) => e.id),
      insights: [
        `Avoid ${partial.action} until root cause is addressed`,
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

  return null;
}

async >
> {
  const patterns: Array<{
    description: string;
    frequency: number;
    experiences: string[];
    significance: "low" | "medium" | "high";
  }> = [];

  // Group experiences by action
  const actionGroups = new Map<string, Experience[]>();
  experiences.forEach((exp) => {
    const group = actionGroups.get(exp.action) || [];
    group.push(exp);
    actionGroups.set(exp.action, group);
  });

  // Detect success/failure patterns
  actionGroups.forEach((group, action) => {
    const successRate =
      group.filter((e) => e.outcome === OutcomeType.POSITIVE).length /
      group.length;

    if (group.length >= 5) {
      if (successRate < 0.3) {
        patterns.push({
          description: `Action ${action} has low success rate (${Math.round(successRate * 100)}%)`,
          frequency: group.length,
          experiences: group.map((e) => e.id),
          significance: "high",
        });
      } else if (successRate > 0.9) {
        patterns.push({
          description: `Action ${action} is highly reliable (${Math.round(successRate * 100)}% success)`,
          frequency: group.length,
          experiences: group.map((e) => e.id),
          significance: "medium",
        });
      }
    }
  });

  // Detect time-based patterns
  const hourlyGroups = groupByHour(experiences);
  hourlyGroups.forEach((group, hour) => {
    if (group.length >= 10) {
      const failureRate =
        group.filter((e) => e.outcome === OutcomeType.NEGATIVE).length /
        group.length;
      if (failureRate > 0.5) {
        patterns.push({
          description: `Higher failure rate during hour ${hour} (${Math.round(failureRate * 100)}%)`,
          frequency: group.length,
          experiences: group.slice(0, 5).map((e) => e.id),
          significance: "medium",
        });
      }
    }
  });

  // Detect learning velocity
  const learningExperiences = experiences.filter(
    (e) =>
      e.type === ExperienceType.DISCOVERY || e.type === ExperienceType.LEARNING,
  );

  if (learningExperiences.length >= 3) {
    const recentLearning = learningExperiences.slice(0, 10);
    const timeDiffs = [];
    for (let i = 1; i < recentLearning.length; i++) {
      timeDiffs.push(
        recentLearning[i - 1].createdAt - recentLearning[i].createdAt,
      );
    }
    const avgTimeBetweenLearning =
      timeDiffs.reduce((a, b) => a + b, 0) / timeDiffs.length;

    patterns.push({
      description: `Learning new things every ${Math.round(avgTimeBetweenLearning / 60000)} minutes on average`,
      frequency: learningExperiences.length,
      experiences: recentLearning.map((e) => e.id),
      significance: "medium",
    });
  }

  return patterns;
}

);

  return groups;
}

} // namespace elizaos
