#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/coreExtensions.h"

void extendRuntimeWithEventUnregistration(std::shared_ptr<IAgentRuntime> runtime)
{
    auto extendedRuntime = as<any>(runtime);
    if (!extendedRuntime["unregisterEvent"]) {
        extendedRuntime["unregisterEvent"] = [=](string event, std::function<std::shared_ptr<Promise<void>>(any)> handler) mutable
        {
            auto handlers = this["events"]["get"](event);
            if (handlers) {
                auto filteredHandlers = handlers["filter"]([=](auto h) mutable
                {
                    return h != handler;
                }
                );
                if (filteredHandlers["length"] > 0) {
                    this["events"]["set"](event, filteredHandlers);
                } else {
                    this["events"]["delete"](event);
                }
            }
        };
    }
};


void extendRuntimeWithComponentUnregistration(std::shared_ptr<IAgentRuntime> runtime)
{
    auto extendedRuntime = as<any>(runtime);
    if (!extendedRuntime["unregisterAction"]) {
        extendedRuntime["unregisterAction"] = [=](string actionName) mutable
        {
            auto index = this["actions"]["findIndex"]([=](auto a) mutable
            {
                return a["name"] == actionName;
            }
            );
            if (index != -1) {
                this["actions"]["splice"](index, 1);
            }
        };
    }
    if (!extendedRuntime["unregisterProvider"]) {
        extendedRuntime["unregisterProvider"] = [=](string providerName) mutable
        {
            auto index = this["providers"]["findIndex"]([=](auto p) mutable
            {
                return p["name"] == providerName;
            }
            );
            if (index != -1) {
                this["providers"]["splice"](index, 1);
            }
        };
    }
    if (!extendedRuntime["unregisterEvaluator"]) {
        extendedRuntime["unregisterEvaluator"] = [=](string evaluatorName) mutable
        {
            auto index = this["evaluators"]["findIndex"]([=](auto e) mutable
            {
                return e["name"] == evaluatorName;
            }
            );
            if (index != -1) {
                this["evaluators"]["splice"](index, 1);
            }
        };
    }
    if (!extendedRuntime["unregisterService"]) {
        extendedRuntime["unregisterService"] = [=](string serviceType) mutable
        {
            auto service = this["services"]["get"](serviceType);
            if (service) {
                std::async([=]() { service["stop"](); });
                this["services"]["delete"](serviceType);
            }
        };
    }
};


void applyRuntimeExtensions(std::shared_ptr<IAgentRuntime> runtime)
{
    extendRuntimeWithEventUnregistration(runtime);
    extendRuntimeWithComponentUnregistration(runtime);
};


