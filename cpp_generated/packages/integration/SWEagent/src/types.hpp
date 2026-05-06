#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_TYPES_H
#include "core.h"

typedef array<std::shared_ptr<TrajectoryStep>> Trajectory;

typedef array<std::shared_ptr<HistoryItem>> History;

typedef Record<string, any> EnvVariables;

typedef Record<string, any> RegistryVariables;

typedef any FileData;

typedef Record<string, any> TemplateContext;

typedef any SerializableData;

typedef any ArgValue;

class ToolCall;
class QueryObject;
class ThinkingBlock;
class CacheControl;
class StepOutput;
class StepOutputImpl;
class TrajectoryStep;
class BaseHistoryItem;
class HistoryItem;
class AgentInfo;
class AgentRunResult;
class ModelResponse;
class ParsedArguments;
class TrajectoryData;
class ReplayConfig;
class SpinnerTask;
class InstanceStats;
class CommandProperty;
class RunContext;
class PatchInfo;
class GithubIssue;
class GithubEvent;
class ParsedArgs;
class CodedError;

class ToolCall : public object, public std::enable_shared_from_this<ToolCall> {
public:
    using std::enable_shared_from_this<ToolCall>::shared_from_this;
    string id;

    string type;

    object function;
};

class QueryObject : public object, public std::enable_shared_from_this<QueryObject> {
public:
    using std::enable_shared_from_this<QueryObject>::shared_from_this;
    string type;

    string content;
};

class ThinkingBlock : public object, public std::enable_shared_from_this<ThinkingBlock> {
public:
    using std::enable_shared_from_this<ThinkingBlock>::shared_from_this;
    string type;

    string content;

    double startTime;

    double endTime;
};

class CacheControl : public object, public std::enable_shared_from_this<CacheControl> {
public:
    using std::enable_shared_from_this<CacheControl>::shared_from_this;
    any type;

    double maxAge;

    any scope;
};

class StepOutput : public object, public std::enable_shared_from_this<StepOutput> {
public:
    using std::enable_shared_from_this<StepOutput>::shared_from_this;
    array<std::shared_ptr<QueryObject>> query;

    string thought;

    string action;

    string output;

    string observation;

    double executionTime;

    boolean done;

    any exitStatus;

    any submission;

    Record<string, string> state;

    any toolCalls;

    any toolCallIds;

    any thinkingBlocks;

    Record<string, any> extraInfo;

    template <typename RET>
    virtual RET toTemplateFormatDict() = 0;
};

class StepOutputImpl : public StepOutput, public std::enable_shared_from_this<StepOutputImpl> {
public:
    using std::enable_shared_from_this<StepOutputImpl>::shared_from_this;
    array<std::shared_ptr<QueryObject>> query = array<std::shared_ptr<QueryObject>>{ object{} };

    string thought = string_empty;

    string action = string_empty;

    string output = string_empty;

    string observation = string_empty;

    double executionTime = 0;

    boolean done = false;

    any exitStatus = nullptr;

    any submission = nullptr;

    Record<string, string> state = object{};

    any toolCalls = nullptr;

    any toolCallIds = nullptr;

    any thinkingBlocks = nullptr;

    Record<string, any> extraInfo = object{};

    template <typename RET>
    RET toTemplateFormatDict();
};

class TrajectoryStep : public object, public std::enable_shared_from_this<TrajectoryStep> {
public:
    using std::enable_shared_from_this<TrajectoryStep>::shared_from_this;
    string action;

    string observation;

    string response;

    Record<string, string> state;

    string thought;

    double executionTime;

    array<std::shared_ptr<QueryObject>> query;

    Record<string, any> extraInfo;
};

class BaseHistoryItem : public object, public std::enable_shared_from_this<BaseHistoryItem> {
public:
    using std::enable_shared_from_this<BaseHistoryItem>::shared_from_this;
    string role;

    any content;

    any messageType;
};

class HistoryItem : public BaseHistoryItem, public std::enable_shared_from_this<HistoryItem> {
public:
    using std::enable_shared_from_this<HistoryItem>::shared_from_this;
    string agent;

    boolean isDemo;

    string thought;

    any action;

    any toolCalls;

    any toolCallIds;

    array<string> tags;

    any cacheControl;

    any thinkingBlocks;
};

class AgentInfo : public object, public std::enable_shared_from_this<AgentInfo> {
public:
    using std::enable_shared_from_this<AgentInfo>::shared_from_this;
    Record<string, double> modelStats;

    any exitStatus;

    any submission;

    Record<string, any> review;

    string editedFiles30;

    string editedFiles50;

    string editedFiles70;

    Record<string, any> summarizer;

    string sweAgentHash;

    string sweAgentVersion;

    string sweRexVersion;

    string sweRexHash;
};

class AgentRunResult : public object, public std::enable_shared_from_this<AgentRunResult> {
public:
    using std::enable_shared_from_this<AgentRunResult>::shared_from_this;
    std::shared_ptr<AgentInfo> info;

    array trajectory;
};

class ModelResponse : public object, public std::enable_shared_from_this<ModelResponse> {
public:
    using std::enable_shared_from_this<ModelResponse>::shared_from_this;
    string message;

    array<std::shared_ptr<ToolCall>> toolCalls;

    array<std::shared_ptr<ToolCall>> tool_calls;

    string role;

    any content;
};

class ParsedArguments : public object, public std::enable_shared_from_this<ParsedArguments> {
public:
    using std::enable_shared_from_this<ParsedArguments>::shared_from_this;
};

class TrajectoryData : public object, public std::enable_shared_from_this<TrajectoryData> {
public:
    using std::enable_shared_from_this<TrajectoryData>::shared_from_this;
    array<object> trajectory;

    Record<string, any> info;

    array<std::shared_ptr<HistoryItem>> history;

    std::shared_ptr<ReplayConfig> replay_config;
};

class ReplayConfig : public object, public std::enable_shared_from_this<ReplayConfig> {
public:
    using std::enable_shared_from_this<ReplayConfig>::shared_from_this;
    Record<string, any> environment;

    Record<string, any> agent;

    Record<string, any> tools;
};

class SpinnerTask : public object, public std::enable_shared_from_this<SpinnerTask> {
public:
    using std::enable_shared_from_this<SpinnerTask>::shared_from_this;
    string status;

    double startTime;

    string instanceId;

    string message;
};

class InstanceStats : public object, public std::enable_shared_from_this<InstanceStats> {
public:
    using std::enable_shared_from_this<InstanceStats>::shared_from_this;
    any exitStatus;

    string result;

    double cost;

    double apiCalls;
};

class CommandProperty : public object, public std::enable_shared_from_this<CommandProperty> {
public:
    using std::enable_shared_from_this<CommandProperty>::shared_from_this;
    string type;

    string description;

    array<string> enum;

    Record<string, string> items;
};

class RunContext : public object, public std::enable_shared_from_this<RunContext> {
public:
    using std::enable_shared_from_this<RunContext>::shared_from_this;
    string outputDir;

    Record<string, any> config;

    double instanceCount;
};

class PatchInfo : public object, public std::enable_shared_from_this<PatchInfo> {
public:
    using std::enable_shared_from_this<PatchInfo>::shared_from_this;
    string source;

    string target;

    array<object> hunks;
};

class GithubIssue : public object, public std::enable_shared_from_this<GithubIssue> {
public:
    using std::enable_shared_from_this<GithubIssue>::shared_from_this;
    string title;

    any body;

    double number;

    string state;

    string created_at;

    string updated_at;

    object user;

    array<object> labels;
};

class GithubEvent : public object, public std::enable_shared_from_this<GithubEvent> {
public:
    using std::enable_shared_from_this<GithubEvent>::shared_from_this;
    string event;

    string commit_id;

    string created_at;
};

class ParsedArgs : public object, public std::enable_shared_from_this<ParsedArgs> {
public:
    using std::enable_shared_from_this<ParsedArgs>::shared_from_this;
};

class CodedError : public Error, public std::enable_shared_from_this<CodedError> {
public:
    using std::enable_shared_from_this<CodedError>::shared_from_this;
    string code;

    double status;

    string stdout;

    string stderr;
};

template <typename RET>
RET StepOutputImpl::toTemplateFormatDict()
{
    return object{
        object::pair{std::string("thought"), this->thought}, 
        object::pair{std::string("action"), this->action}, 
        object::pair{std::string("output"), this->output}, 
        object::pair{std::string("observation"), this->observation}, 
        object::pair{std::string("executionTime"), this->executionTime}, 
        object::pair{std::string("done"), this->done}, 
        object::pair{std::string("exitStatus"), this->exitStatus}, 
        object::pair{std::string("submission"), this->submission}
    };
}

#endif
