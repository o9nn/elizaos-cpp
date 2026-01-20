#include "coreExtensions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void extendRuntimeWithEventUnregistration(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto extendedRuntime = runtime;

    // Add unregisterEvent method if it doesn't exist
    if (!extendedRuntime.unregisterEvent) {
        extendedRuntime.unregisterEvent = function (;
        event: string,
        handler: (params: any) => Promise<void>
        ) {
            const auto handlers = this.events.get(event);
            if (handlers) {
                const auto filteredHandlers = handlers.filter((h) => h != handler);
                if (filteredHandlers.length > 0) {
                    this.events.set(event, filteredHandlers);
                    } else {
                        this.events.delete(event);
                    }
                }
                };
            }

}

void extendRuntimeWithComponentUnregistration(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto extendedRuntime = runtime;

    // Add unregisterAction method if it doesn't exist
    if (!extendedRuntime.unregisterAction) {
        extendedRuntime.unregisterAction = function (actionName: string) {
            const auto index = this.actions.findIndex((a) => a.name == actionName);
            if (index != -1) {
                this.actions.splice(index, 1);
            }
            };
        }

        // Add unregisterProvider method if it doesn't exist
        if (!extendedRuntime.unregisterProvider) {
            extendedRuntime.unregisterProvider = function (providerName: string) {
                const auto index = this.providers.findIndex((p) => p.name == providerName);
                if (index != -1) {
                    this.providers.splice(index, 1);
                }
                };
            }

            // Add unregisterEvaluator method if it doesn't exist
            if (!extendedRuntime.unregisterEvaluator) {
                extendedRuntime.unregisterEvaluator = function (evaluatorName: string) {
                    const auto index = this.evaluators.findIndex((e) => e.name == evaluatorName);
                    if (index != -1) {
                        this.evaluators.splice(index, 1);
                    }
                    };
                }

                // Add unregisterService method if it doesn't exist
                if (!extendedRuntime.unregisterService) {
                    extendedRuntime.unregisterService = async function (serviceType: string) {
                        const auto service = this.services.get(serviceType);
                        if (service) {
                            service.stop();
                            this.services.delete(serviceType);
                        }
                        };
                    }

}

void applyRuntimeExtensions(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    extendRuntimeWithEventUnregistration(runtime);
    extendRuntimeWithComponentUnregistration(runtime);

}

} // namespace elizaos
