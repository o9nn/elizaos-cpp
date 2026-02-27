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


// Extend the core service types with experience service
  struct ServiceTypeRegistry {
    "EXPERIENCE" EXPERIENCE;
};

// Export service type constant

enum ExperienceType {
  SUCCESS = "success", // Agent accomplished something
  FAILURE = "failure", // Agent failed at something
  DISCOVERY = "discovery", // Agent discovered new information
  CORRECTION = "correction", // Agent corrected a mistake
  LEARNING = "learning", // Agent learned something new
  HYPOTHESIS = "hypothesis", // Agent formed a hypothesis
  VALIDATION = "validation", // Agent validated a hypothesis
  WARNING = "warning", // Agent encountered a warning/limitation
}

enum OutcomeType {
  POSITIVE = "positive",
  NEGATIVE = "negative",
  NEUTRAL = "neutral",
  MIXED = "mixed",
}

struct Experience {
    UUID id;
    UUID agentId;
    ExperienceType type;
    OutcomeType outcome;
    std:; // What was happening context;
    std:; // What the agent tried to do action;
    std:; // What actually happened result;
    std:; // What was learned learning;
    std:[]; // Tags for categorization tags;
    std:; // Domain of experience (e.g., 'shell', 'coding', 'system') domain;
    std::optional<UUID[]; // Links to related experiences> relatedExperiences;
    std::optional<UUID; // If this experience updates/replaces another> supersedes;
    number; // 0-1, how confident the agent is in this learning confidence;
    number; // 0-1, how important this experience is importance;
    double createdAt;
    double updatedAt;
    std::optional<double> lastAccessedAt;
    double accessCount;
    std::optional<std:; // What the agent previously believed> previousBelief;
    std::optional<std:; // The corrected understanding> correctedBelief;
    std::optional<number[]; // For semantic search> embedding;
    std::optional<UUID[]; // Related memory IDs> memoryIds;
};

struct ExperienceQuery {
    std::optional<std::vector<ExperienceType | ExperienceType>> type;
    std::optional<std::vector<OutcomeType | OutcomeType>> outcome;
    std::optional<std::vector<std: | string>> domain;
    std::optional<std::vector<std::string>> tags;
    std::optional<double> minImportance;
    std::optional<double> minConfidence;
    std::optional<{> timeRange;
    std::optional<double> start;
    std::optional<double> end;
    std::optional<double> limit;
    std::optional<bool> includeRelated;
};

struct ExperienceAnalysis {
    std::optional<std:; // Detected pattern> pattern;
    std::optional<number; // How often this occurs> frequency;
    std::optional<number; // How reliable this knowledge is> reliability;
    std::optional<std:[]; // Alternative approaches discovered> alternatives;
    std::optional<std:[]; // Recommendations based on experience> recommendations;
};

struct ExperienceEvent {
    UUID experienceId;
    "created" | "accessed" | "updated" | "superseded" eventType;
    double timestamp;
    std::optional<std::unordered_map<std:, std:>> metadata;
};


} // namespace elizaos
