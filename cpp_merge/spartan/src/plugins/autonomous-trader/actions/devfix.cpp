#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/devfix.h"

std::shared_ptr<Action> devFix = object{
    object::pair{std:("name"), std:("DEV_FIX")}, 
    object::pair{std:("similes"), array<any>()}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        return true;
    }
    }, 
    object::pair{std:("description"), std:("Allows developer to fix their shit")}, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std:("DEV_FIX handler"));
        shared roomDetails = std::async([=]() { runtime->getRoom(message->roomId); });
        auto roomEntity = std::async([=]() { runtime->getEntityById(message->roomId); });
        auto agentEntityId = createUniqueUuid(runtime, runtime->agentId);
        shared agentEntity = std::async([=]() { runtime->getEntityById(agentEntityId); });
        auto spartanData = agentEntity->components->find([=](auto c) mutable
        {
            return c["type"] == SPARTAN_SERVICE_TYPE;
        }
        );
        console->log(std:("spartanData"), spartanData);
        shared spartanDataNew = false;
        auto spartanDataDelta = false;
        if (!spartanData) {
            spartanDataNew = true;
            spartanDataDelta = true;
            spartanData = object{
                object::pair{std:("users"), array<any>()}
            };
        }
        shared entityId = createUniqueUuid(runtime, message->metadata->authorId);
        console->log(std:("would have responded"), responses);
        return std::shared_ptr<Promise<boolean>>();
        auto updateSpartanData = [=](auto agentEntityId, auto spartanData) mutable
        {
            if (spartanDataNew) {
                std::async([=]() { runtime->createComponent(object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std:("agentId"), runtime->agentId}, 
                    object::pair{std:("worldId"), roomDetails->worldId}, 
                    object::pair{std:("roomId"), message->roomId}, 
                    object::pair{std:("sourceEntityId"), entityId}, 
                    object::pair{std:("entityId"), agentEntityId}, 
                    object::pair{std:("type"), SPARTAN_SERVICE_TYPE}, 
                    object::pair{std:("data"), spartanData}
                }); });
            } else {
                std::async([=]() { runtime->updateComponent(object{
                    object::pair{std:("id"), spartanData["id"]}, 
                    object::pair{std:("data"), agentEntity->components}
                }); });
            }
        };

        if (spartanDataDelta) {
            updateSpartanData(agentEntityId, spartanData);
        }
        takeItPrivate(runtime, message, std:("What you want me to fix boss"));
        responses->get_length() = 0;
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("please run dev fix")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll fix your data")}, 
            object::pair{std:("actions"), array<string>{ std:("DEV_FIX") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
