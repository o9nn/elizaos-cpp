#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_LIB_AGENT-RUNTIME_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_LIB_AGENT-RUNTIME_H
#include "core.h"
#include "@elizaos/core.h"
#include "./agent.h"
using agent = _default;

class GlobalElizaState;
class AgentRuntimeManager;

class GlobalElizaState : public object, public std::enable_shared_from_this<GlobalElizaState> {
public:
    using std::enable_shared_from_this<GlobalElizaState>::shared_from_this;
    boolean __elizaMigrationsRan;

    boolean __elizaManagerLogged;

    any __elizaRuntime;

    object logger;
};

extern std::shared_ptr<GlobalElizaState> globalState;
class AgentRuntimeManager : public object, public std::enable_shared_from_this<AgentRuntimeManager> {
public:
    using std::enable_shared_from_this<AgentRuntimeManager>::shared_from_this;
    static std::shared_ptr<AgentRuntimeManager> instance;

    any runtime = nullptr;

    boolean hasRunMigrations = false;

    any initializationPromise = nullptr;

    AgentRuntimeManager();
    static std::shared_ptr<AgentRuntimeManager> getInstance();
    virtual boolean isReady();
    virtual std::shared_ptr<Promise<std::shared_ptr<AgentRuntime>>> getRuntime();
    virtual std::shared_ptr<Promise<std::shared_ptr<AgentRuntime>>> createRuntime();
    virtual std::shared_ptr<Promise<void>> ensureBuiltInTables();
    virtual std::shared_ptr<Promise<std::shared_ptr<Memory>>> handleMessage(string roomId, string entityId, object content);
};

extern std::shared_ptr<AgentRuntimeManager> agentRuntime;
#endif
