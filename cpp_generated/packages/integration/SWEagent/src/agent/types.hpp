#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_AGENT_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_AGENT_TYPES_H
#include "core.h"

typedef any MessageContent;

class ModelOutput;
class ToolCall;
class ThinkingBlock;
class Command;
class FormatDict;
class ExtraFields;
class StatsData;
class APIResponse;

class ModelOutput : public object, public std::enable_shared_from_this<ModelOutput> {
public:
    using std::enable_shared_from_this<ModelOutput>::shared_from_this;
    string message;

    string content;

    array<std::shared_ptr<ToolCall>> toolCalls;

    array<std::shared_ptr<ThinkingBlock>> thinkingBlocks;

    object usage;
};

class ToolCall : public object, public std::enable_shared_from_this<ToolCall> {
public:
    using std::enable_shared_from_this<ToolCall>::shared_from_this;
    string id;

    string type;

    object function;
};

class ThinkingBlock : public object, public std::enable_shared_from_this<ThinkingBlock> {
public:
    using std::enable_shared_from_this<ThinkingBlock>::shared_from_this;
    string type;

    string content;
};

class Command : public object, public std::enable_shared_from_this<Command> {
public:
    using std::enable_shared_from_this<Command>::shared_from_this;
    string name;

    string endName;

    string installScript;
};

class FormatDict : public object, public std::enable_shared_from_this<FormatDict> {
public:
    using std::enable_shared_from_this<FormatDict>::shared_from_this;
};

class ExtraFields : public object, public std::enable_shared_from_this<ExtraFields> {
public:
    using std::enable_shared_from_this<ExtraFields>::shared_from_this;
};

class StatsData : public object, public std::enable_shared_from_this<StatsData> {
public:
    using std::enable_shared_from_this<StatsData>::shared_from_this;
    double instanceCost;

    double tokensSent;

    double tokensReceived;

    double apiCalls;
};

class APIResponse : public object, public std::enable_shared_from_this<APIResponse> {
public:
    using std::enable_shared_from_this<APIResponse>::shared_from_this;
    array<object> choices;

    object usage;
};

#endif
