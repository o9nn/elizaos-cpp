#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_AGENT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_AGENT_H
#include "core.h"
#include "./knowledge.h"
#include "./primitives.h"
#include "./state.h"

typedef any TemplateType;

class MessageExample;
class Character;
enum struct AgentStatus;
class Agent;

class MessageExample : public object, public std::enable_shared_from_this<MessageExample> {
public:
    using std::enable_shared_from_this<MessageExample>::shared_from_this;
    string name;

    std::shared_ptr<Content> content;
};

class Character : public object, public std::enable_shared_from_this<Character> {
public:
    using std::enable_shared_from_this<Character>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string username;

    string system;

    object templates;

    any bio;

    array<array<std::shared_ptr<MessageExample>>> messageExamples;

    array<string> postExamples;

    array<string> topics;

    array<string> adjectives;

    array<any> knowledge;

    array<string> plugins;

    object settings;

    object secrets;

    object style;
};

enum struct AgentStatus {
    ACTIVE = std:("active"), INACTIVE = std:("inactive")
};
class Agent : public Character, public std::enable_shared_from_this<Agent> {
public:
    using std::enable_shared_from_this<Agent>::shared_from_this;
    boolean enabled;

    AgentStatus status;

    double createdAt;

    double updatedAt;
};

#endif
