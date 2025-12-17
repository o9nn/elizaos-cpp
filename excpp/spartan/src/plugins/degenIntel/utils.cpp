#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> acquireService(IAgentRuntime runtime, auto serviceType, auto asking = '', auto retries = 10) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto service = runtime.getService(serviceType);
    while (!service) {
        std::cout << asking << 'waiting for' << serviceType << 'service...' << std::endl;
        service = runtime.getService(serviceType);
        if (!service) {
            new Promise((waitResolve) => setTimeout(waitResolve, 1000));
            } else {
                std::cout << asking << 'Acquired' << serviceType << 'service...' << std::endl;
            }
        }
        return service;

}

std::future<void> askLlmObject(IAgentRuntime runtime, Object ask, const std::vector<std::string>& requiredFields, auto maxRetries = 3) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::optional<std::any> responseContent = nullptr;
    // Retry if missing required fields
    auto retries = 0;

    function checkRequired(resp) {
        if (!resp) return false;
        auto hasAll = true;
        for (const auto& f : requiredFields)
            if (!resp[f]) {
                hasAll = false;
                break;
            }
        }
        return hasAll;
    }

    auto good = false;
    while (retries < maxRetries && !good) {
        const auto response = runtime.useModel(ModelType.TEXT_LARGE, {;
            ...ask, // prompt, system;
            temperature: 0.2,
            maxTokens: 4096,
            object: true,
            });

            std::cout << 'trader::utils:askLlmObject - response' << response << std::endl;
            responseContent = parseJSONObjectFromText(response);

            retries++;
            good = checkRequired(responseContent);
            if (!good) {
                logger.warn(
                '*** Missing required fields',
                responseContent,
                'needs',
                requiredFields,
                ', retrying... ***';
                );
            }
        }
        return responseContent;

}

} // namespace elizaos
