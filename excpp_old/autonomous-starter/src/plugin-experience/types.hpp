#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// Extend the core service types with experience service
  struct ServiceTypeRegistry {
    "EXPERIENCE" EXPERIENCE;
};

// Export service type constant

enum ExperienceType {
}

enum OutcomeType {
}

struct Experience {
    UUID id;
    UUID agentId;
    ExperienceType type;
    OutcomeType outcome;
    std::string context;
    std::string action;
    std::string result;
    std::string learning;
    std::vector<std::string> tags;
    std::string domain;
    std::optional<std::vector<UUID>> relatedExperiences;
    std::optional<UUID> supersedes;
    double confidence;
    double importance;
    double createdAt;
    double updatedAt;
    std::optional<double> lastAccessedAt;
    double accessCount;
    std::optional<std::string> previousBelief;
    std::optional<std::string> correctedBelief;
    std::optional<std::vector<double>> embedding;
    std::optional<std::vector<UUID>> memoryIds;
};

struct ExperienceQuery {
    std::optional<std::variant<ExperienceType, std::vector<ExperienceType>>> type;
    std::optional<std::variant<OutcomeType, std::vector<OutcomeType>>> outcome;
    std::optional<std::variant<std::string, std::vector<std::string>>> domain;
    std::optional<std::vector<std::string>> tags;
    std::optional<double> minImportance;
    std::optional<double> minConfidence;
    std::optional<double> start;
    std::optional<double> end;
    std::optional<double> limit;
    std::optional<bool> includeRelated;
};

struct ExperienceAnalysis {
    std::optional<std::string> pattern;
    std::optional<double> frequency;
    std::optional<double> reliability;
    std::optional<std::vector<std::string>> alternatives;
    std::optional<std::vector<std::string>> recommendations;
};

struct ExperienceEvent {
    UUID experienceId;
    std::variant<"created", "accessed", "updated", "superseded"> eventType;
    double timestamp;
};


} // namespace elizaos
