#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_PLUGIN_HYPERFY_MANAGERS_GUARDS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_PLUGIN_HYPERFY_MANAGERS_GUARDS_H
#include "core.hpp"

class AgentActivityLock;

class AgentActivityLock : public object, public std::enable_shared_from_this<AgentActivityLock> {
public:
    using std::enable_shared_from_this<AgentActivityLock>::shared_from_this;
    double count = 0;

    virtual boolean isActive();
    virtual void enter();
    virtual void exit();
    template <typename T>
    std::shared_ptr<Promise<T>> run(std::function<std::shared_ptr<Promise<T>>()> fn);
};

extern std::shared_ptr<AgentActivityLock> agentActivityLock;
template <typename T>
std::shared_ptr<Promise<T>> AgentActivityLock::run(std::function<std::shared_ptr<Promise<T>>()> fn)
{
    this->enter();
    {
        utils::finally __finally506_537([&]() mutable
        {
            this->exit();
        });
        try
        {
            return std::async([=]() { fn(); });
        }
        catch (...)
        {
            throw;
        }
    }
}

#endif
