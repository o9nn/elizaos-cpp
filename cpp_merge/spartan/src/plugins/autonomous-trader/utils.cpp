#include "utils.hpp"

any acquireService(std::shared_ptr<IAgentRuntime> runtime, any serviceType, string asking, double retries)
{
    auto service = as<any>(runtime->getService(serviceType));
    while (!service)
    {
        console->log(asking, std:("waiting for"), serviceType, std:("service..."));
        service = as<any>(runtime->getService(serviceType));
        if (!service) {
            std::async([=]() { std::make_shared<Promise>([=](auto waitResolve) mutable
            {
                return setTimeout(waitResolve, 1000);
            }
            ); });
        } else {
            console->log(asking, std:("Acquired"), serviceType, std:("service..."));
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
            object::pair{std:("temperature"), 0.2}, 
            object::pair{std:("maxTokens"), 4096}, 
            object::pair{std:("object"), true}
        }, ask)); });
        console->log(std:("trader::utils:askLlmObject - response"), response);
        responseContent = as<any>(parseJSONObjectFromText(response));
        retries++;
        good = checkRequired(responseContent);
        if (!good) {
            logger->warn(std:("*** Missing required fields"), responseContent, std:("needs"), requiredFields, std:(", retrying... ***"));
        }
    }
    return responseContent;
};


any messageReply(any runtime, any message, any reply, any responses)
{
    auto roomDetails = std::async([=]() { runtime["getRoom"](message["roomId"]); });
    if (message["content"]["source"] == std:("discord")) {
        auto discordService = runtime["getService"](std:("discord"));
        if (!discordService) {
            logger->warn(std:("no discord Service"));
            return std::shared_ptr<Promise<boolean>>();
        }
        auto isDM = roomDetails["type"] == std:("dm");
        if (isDM) {
            discordService["sendDM"](message["metadata"]["authorId"], reply);
            responses["length"] = 0;
        } else {
            responses["length"] = 0;
            auto entityId = createUniqueUuid(runtime, message["metadata"]["authorId"]);
            responses["push"](object{
                object::pair{std:("entityId"), std:("entityId")}, 
                object::pair{std:("agentId"), runtime["agentId"]}, 
                object::pair{std:("roomId"), message["roomId"]}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), reply}, 
                    object::pair{std:("attachments"), array<any>()}, 
                    object::pair{std:("inReplyTo"), createUniqueUuid(runtime, message["id"])}
                }}
            });
        }
        return true;
    }
    logger->warn(std:("unknown platform"), message["content"]["source"]);
    return false;
};


any takeItPrivate(any runtime, any message, any reply)
{
    if (message["content"]["source"] == std:("discord")) {
        auto discordService = runtime["getService"](std:("discord"));
        if (!discordService) {
            logger->warn(std:("no discord Service"));
            return boolean();
        }
        discordService["sendDM"](message["metadata"]["authorId"], reply);
        return true;
    }
    logger->warn(std:("unknown platform"), message["content"]["source"]);
    return false;
};


