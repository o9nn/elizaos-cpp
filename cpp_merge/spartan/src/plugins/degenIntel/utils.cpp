#include "utils.hpp"
#include <string>

any acquireService(std::shared_ptr<IAgentRuntime> runtime, any serviceType, string asking, double retries)
{
    auto service = as<any>(runtime->getService(serviceType));
    while (!service)
    {
        console->log(asking, std::string("waiting for"), serviceType, std::string("service..."));
        service = as<any>(runtime->getService(serviceType));
        if (!service) {
            std::async([=]() { std::make_shared<Promise>([=](auto waitResolve) mutable
            {
                return setTimeout(waitResolve, 1000);
            }
            ); });
        } else {
            console->log(asking, std::string("Acquired"), serviceType, std::string("service..."));
        }
    }
    return service;
};


any askLlmObject(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Object> ask, array<string> requiredFields, double maxRetries)
{
    auto responseContent = nullptr;
    auto retries = 0;
    auto checkRequired = [=](auto resp) mutable
    {
        if (!resp) return false;
        auto hasAll = true;
        for (auto& f : requiredFields)
        {
            if (!const_(resp)[f]) {
                hasAll = false;
                break;
            }
        }
        return hasAll;
    };

    auto good = false;
    while (AND((retries < maxRetries), (!good)))
    {
        auto response = std::async([=]() { runtime->useModel(ModelType->TEXT_LARGE, utils::assign(object{
            , 
            object::pair{std::string("temperature"), 0.2}, 
            object::pair{std::string("maxTokens"), 4096}, 
            object::pair{std::string("object"), true}
        }, ask)); });
        console->log(std::string("trader::utils:askLlmObject - response"), response);
        responseContent = as<any>(parseJSONObjectFromText(response));
        retries++;
        good = checkRequired(responseContent);
        if (!good) {
            logger->warn(std::string("*** Missing required fields"), responseContent, std::string("needs"), requiredFields, std::string(", retrying... ***"));
        }
    }
    return responseContent;
};


