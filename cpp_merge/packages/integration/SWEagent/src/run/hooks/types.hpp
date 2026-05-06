#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_HOOKS_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_HOOKS_TYPES_H
#include "core.hpp"
#include "../../agent/problem-statement.h"
#include "../../environment/swe-env.h"
#include "../../types.h"

class RunHook;
class AbstractRunHook;
class CombinedRunHooks;

class RunHook : public object, public std::enable_shared_from_this<RunHook> {
public:
    using std::enable_shared_from_this<RunHook>::shared_from_this;
    virtual void onInit(Record<string, any> run) = 0;
    virtual void onStart() = 0;
    virtual void onEnd() = 0;
    virtual void onInstanceStart(object params) = 0;
    virtual void onInstanceSkipped() = 0;
    virtual void onInstanceCompleted(object params) = 0;
};

class AbstractRunHook : public RunHook, public std::enable_shared_from_this<AbstractRunHook> {
public:
    using std::enable_shared_from_this<AbstractRunHook>::shared_from_this;
    virtual void onInit(Record<string, any> _run);
    virtual void onStart();
    virtual void onEnd();
    virtual void onInstanceStart(object _params);
    virtual void onInstanceSkipped();
    virtual void onInstanceCompleted(object _params);
};

class CombinedRunHooks : public RunHook, public std::enable_shared_from_this<CombinedRunHooks> {
public:
    using std::enable_shared_from_this<CombinedRunHooks>::shared_from_this;
    array<std::shared_ptr<RunHook>> _hooks = array<std::shared_ptr<RunHook>>();

    virtual void addHook(std::shared_ptr<RunHook> hook);
    virtual array<std::shared_ptr<RunHook>> get_hooks();
    virtual void onInit(Record<string, any> run);
    virtual void onStart();
    virtual void onEnd();
    virtual void onInstanceStart(object params);
    virtual void onInstanceSkipped();
    virtual void onInstanceCompleted(object params);
};

#endif
