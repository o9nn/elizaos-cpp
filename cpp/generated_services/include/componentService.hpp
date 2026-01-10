#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-TODO_SERVICES_COMPONENTSERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-TODO_SERVICES_COMPONENTSERVICE_H
#include "core.h"
#include "@elizaos/core.h"
#include "./dbCompatibility.h"

class ComponentService;

class ComponentService : public object, public std::enable_shared_from_this<ComponentService> {
public:
    using std::enable_shared_from_this<ComponentService>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    ComponentService(std::shared_ptr<IAgentRuntime> runtime_);
    virtual std::shared_ptr<Promise<any>> getComponent(std::shared_ptr<UUID> entityId, string componentType);
    virtual std::shared_ptr<Promise<boolean>> createComponent(object params);
    virtual std::shared_ptr<Promise<boolean>> updateComponent(object params);
    virtual std::shared_ptr<Component> normalizeComponent(std::shared_ptr<Component> component);
    virtual void normalizeBooleans(any obj);
};

std::shared_ptr<ComponentService> createComponentService(std::shared_ptr<IAgentRuntime> runtime);

#endif
