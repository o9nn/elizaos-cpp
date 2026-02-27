#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_LIB_AGENT_RUNTIME_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_LIB_AGENT_RUNTIME_H
#include "core.hpp"
// External dependency removed
#include "./agent.h"
// Using alias removed (invalid transpilation)

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
