#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/devfix.h"

std::shared_ptr<Action> devFix = object{
    object::pair{std::string("name"), std::string("DEV_FIX")}, 
    object::pair{std::string("similes"), array<any>()}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        return true;
    }
    }, 
    object::pair{std::string("description"), std::string("Allows developer to fix their shit")}, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std::string("DEV_FIX handler"));
        shared roomDetails = std::async([=]() { runtime->getRoom(message->roomId); });
        auto roomEntity = std::async([=]() { runtime->getEntityById(message->roomId); });
        auto agentEntityId = createUniqueUuid(runtime, runtime->agentId);
        shared agentEntity = std::async([=]() { runtime->getEntityById(agentEntityId); });
        auto spartanData = agentEntity->components->find([=](auto c) mutable
        {
            return c["type"] == SPARTAN_SERVICE_TYPE;
        }
        );
        console->log(std::string("spartanData"), spartanData);
        shared spartanDataNew = false;
        auto spartanDataDelta = false;
        if (!spartanData) {
            spartanDataNew = true;
            spartanDataDelta = true;
            spartanData = object{
                object::pair{std::string("users"), array<any>()}
            };
        }
        shared entityId = createUniqueUuid(runtime, message->metadata->authorId);
        console->log(std::string("would have responded"), responses);
        return std::shared_ptr<Promise<boolean>>();
        auto updateSpartanData = [=](auto agentEntityId, auto spartanData) mutable
        {
            if (spartanDataNew) {
                std::async([=]() { runtime->createComponent(object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std::string("agentId"), runtime->agentId}, 
                    object::pair{std::string("worldId"), roomDetails->worldId}, 
                    object::pair{std::string("roomId"), message->roomId}, 
                    object::pair{std::string("sourceEntityId"), entityId}, 
                    object::pair{std::string("entityId"), agentEntityId}, 
                    object::pair{std::string("type"), SPARTAN_SERVICE_TYPE}, 
                    object::pair{std::string("data"), spartanData}
                }); });
            } else {
                std::async([=]() { runtime->updateComponent(object{
                    object::pair{std::string("id"), spartanData["id"]}, 
                    object::pair{std::string("data"), agentEntity->components}
                }); });
            }
        };

        if (spartanDataDelta) {
            updateSpartanData(agentEntityId, spartanData);
        }
        takeItPrivate(runtime, message, std::string("What you want me to fix boss"));
        responses->get_length() = 0;
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("please run dev fix")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll fix your data")}, 
            object::pair{std::string("actions"), array<string>{ std::string("DEV_FIX") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
