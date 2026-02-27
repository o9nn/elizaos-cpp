#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_TYPES_H
#include "core.h"
#include "../agent/agents.h"
#include "../environment/swe-env.h"
#include "../agent/problem-statement.h"
#include "../environment/deployment.h"

class RunSingleActionConfig;
class RunSingleConfig;
class BatchInstanceSourceConfig;
class RunBatchConfig;
class CLIConfig;
class TrajectoryData;
class SWEBenchInstanceData;
class DemoStep;

class RunSingleActionConfig : public object, public std::enable_shared_from_this<RunSingleActionConfig> {
public:
    using std::enable_shared_from_this<RunSingleActionConfig>::shared_from_this;
    boolean openPr;

    object prConfig;

    boolean applyPatchLocally;
};

class RunSingleConfig : public object, public std::enable_shared_from_this<RunSingleConfig> {
public:
    using std::enable_shared_from_this<RunSingleConfig>::shared_from_this;
    std::shared_ptr<EnvironmentConfig> env;

    AgentConfig agent;

    ProblemStatementConfig problemStatement;

    string outputDir;

    std::shared_ptr<RunSingleActionConfig> actions;

    string envVarPath;
};

class BatchInstanceSourceConfig : public object, public std::enable_shared_from_this<BatchInstanceSourceConfig> {
public:
    using std::enable_shared_from_this<BatchInstanceSourceConfig>::shared_from_this;
    any type;

    string path;

    string filter;

    string slice;

    boolean shuffle;

    std::shared_ptr<DeploymentConfig> deployment;

    any subset;

    any split;

    string pathOverride;

    boolean evaluate;

    string dataset_name;
};

class RunBatchConfig : public object, public std::enable_shared_from_this<RunBatchConfig> {
public:
    using std::enable_shared_from_this<RunBatchConfig>::shared_from_this;
    std::shared_ptr<BatchInstanceSourceConfig> instances;

    AgentConfig agent;

    string outputDir;

    string suffix;

    boolean raiseExceptions;

    boolean redoExisting;

    string envVarPath;

    double numWorkers;

    double randomDelayMultiplier;

    boolean progressBar;
};

class CLIConfig : public object, public std::enable_shared_from_this<CLIConfig> {
public:
    using std::enable_shared_from_this<CLIConfig>::shared_from_this;
    string config;

    std::shared_ptr<EnvironmentConfig> env;

    AgentConfig agent;

    ProblemStatementConfig problemStatement;

    string outputDir;

    std::shared_ptr<RunSingleActionConfig> actions;

    std::shared_ptr<BatchInstanceSourceConfig> instances;

    double numWorkers;

    boolean redoExisting;

    double randomDelayMultiplier;
};

class TrajectoryData : public object, public std::enable_shared_from_this<TrajectoryData> {
public:
    using std::enable_shared_from_this<TrajectoryData>::shared_from_this;
    array<object> history;

    any replay_config;

    object info;

    Record<string, any> environment;

    ProblemStatementConfig problemStatement;

    array<object> trajectory;
};

class SWEBenchInstanceData : public object, public std::enable_shared_from_this<SWEBenchInstanceData> {
public:
    using std::enable_shared_from_this<SWEBenchInstanceData>::shared_from_this;
    string instance_id;

    string repo;

    string version;

    string base_commit;

    string problem_statement;

    string hints_text;

    string created_at;

    string test_patch;
};

class DemoStep : public object, public std::enable_shared_from_this<DemoStep> {
public:
    using std::enable_shared_from_this<DemoStep>::shared_from_this;
    string role;

    string content;

    string agent;

    boolean isDemo;
};

#endif
