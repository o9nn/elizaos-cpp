#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-EXPERIENCE_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-EXPERIENCE_TYPES_H
#include "core.h"
#include "@elizaos/core.h"

namespace @elizaos/core {
    class ServiceTypeRegistry;
}
enum struct ExperienceType;
enum struct OutcomeType;
class Experience;
class ExperienceQuery;
class ExperienceAnalysis;
class ExperienceEvent;
class ExperienceMemory;

namespace std::string("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string EXPERIENCE;
    };

}
extern object ExperienceServiceType, satisfies, Partial;
enum struct ExperienceType {
    SUCCESS = std::string("success"), FAILURE = std::string("failure"), DISCOVERY = std::string("discovery"), CORRECTION = std::string("correction"), LEARNING = std::string("learning"), HYPOTHESIS = std::string("hypothesis"), VALIDATION = std::string("validation"), WARNING = std::string("warning")
};
enum struct OutcomeType {
    POSITIVE = std::string("positive"), NEGATIVE = std::string("negative"), NEUTRAL = std::string("neutral"), MIXED = std::string("mixed")
};
class Experience : public object, public std::enable_shared_from_this<Experience> {
public:
    using std::enable_shared_from_this<Experience>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> agentId;

    ExperienceType type;

    OutcomeType outcome;

    string context;

    string action;

    string result;

    string learning;

    array<string> tags;

    string domain;

    array<std::shared_ptr<UUID>> relatedExperiences;

    std::shared_ptr<UUID> supersedes;

    double confidence;

    double importance;

    double createdAt;

    double updatedAt;

    double lastAccessedAt;

    double accessCount;

    string previousBelief;

    string correctedBelief;

    array<double> embedding;

    array<std::shared_ptr<UUID>> memoryIds;
};

class ExperienceQuery : public object, public std::enable_shared_from_this<ExperienceQuery> {
public:
    using std::enable_shared_from_this<ExperienceQuery>::shared_from_this;
    any type;

    any outcome;

    any domain;

    array<string> tags;

    double minImportance;

    double minConfidence;

    object timeRange;

    double limit;

    boolean includeRelated;
};

class ExperienceAnalysis : public object, public std::enable_shared_from_this<ExperienceAnalysis> {
public:
    using std::enable_shared_from_this<ExperienceAnalysis>::shared_from_this;
    string pattern;

    double frequency;

    double reliability;

    array<string> alternatives;

    array<string> recommendations;
};

class ExperienceEvent : public object, public std::enable_shared_from_this<ExperienceEvent> {
public:
    using std::enable_shared_from_this<ExperienceEvent>::shared_from_this;
    std::shared_ptr<UUID> experienceId;

    any eventType;

    double timestamp;

    Record<string, any> metadata;
};

class ExperienceMemory : public Memory, public std::enable_shared_from_this<ExperienceMemory> {
public:
    using std::enable_shared_from_this<ExperienceMemory>::shared_from_this;
    string experienceId;

    ExperienceType experienceType;
};

namespace @elizaos/core {
}
#endif
