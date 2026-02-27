#include "utils.hpp"

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


