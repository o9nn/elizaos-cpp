#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-app/src/utils.h"

any messageReply(any runtime, any message, any reply, any responses)
{
    auto roomDetails = std::async([=]() { runtime["getRoom"](message["roomId"]); });
    if (message["content"]["source"] == std::string("discord")) {
        auto discordService = runtime["getService"](std::string("discord"));
        if (!discordService) {
            logger->warn(std::string("no discord Service"));
            return std::shared_ptr<Promise<boolean>>();
        }
        auto isDM = roomDetails["type"] == std::string("dm");
        if (isDM) {
            discordService["sendDM"](message["metadata"]["authorId"], reply);
            responses["length"] = 0;
        } else {
            responses["length"] = 0;
            auto entityId = createUniqueUuid(runtime, message["metadata"]["authorId"]);
            responses["push"](object{
                object::pair{std::string("entityId"), std::string("entityId")}, 
                object::pair{std::string("agentId"), runtime["agentId"]}, 
                object::pair{std::string("roomId"), message["roomId"]}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), reply}, 
                    object::pair{std::string("attachments"), array<any>()}, 
                    object::pair{std::string("inReplyTo"), createUniqueUuid(runtime, message["id"])}
                }}
            });
        }
        return true;
    }
    logger->warn(std::string("unknown platform"), message["content"]["source"]);
    return false;
};


any takeItPrivate(any runtime, any message, any reply)
{
    if (message["content"]["source"] == std::string("discord")) {
        auto discordService = runtime["getService"](std::string("discord"));
        if (!discordService) {
            logger->warn(std::string("no discord Service"));
            return boolean();
        }
        discordService["sendDM"](message["metadata"]["authorId"], reply);
        return true;
    }
    logger->warn(std::string("unknown platform"), message["content"]["source"]);
    return false;
};


