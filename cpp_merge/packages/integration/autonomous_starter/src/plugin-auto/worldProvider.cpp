#include "worldProvider.hpp"

string AUTO_WORLD_SEED = std:("autonomous_world_singleton");
string AUTO_ROOM_SEED = std:("autonomous_room_singleton");
std::shared_ptr<Provider> autonomousWorldProvider = object{
    object::pair{std:("name"), std:("AUTONOMOUS_WORLD")}, 
    object::pair{std:("description"), std:("Information about the autonomous world and room setup")}, 
    object::pair{std:("position"), 50}, 
    object::pair{std:("get"), [=](auto runtime, auto message) mutable
    {
        try
        {
            auto worldId = as<std::shared_ptr<UUID>>(runtime->getSetting(std:("WORLD_ID")));
            auto autonomousRoomId = createUniqueUuid(runtime, AUTO_ROOM_SEED);
            if (!worldId) {
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("worldId"), nullptr}, 
                        object::pair{std:("roomId"), autonomousRoomId}, 
                        object::pair{std:("status"), std:("no_world_id")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("autonomousWorld"), std:("No WORLD_ID configured - autonomous system may not be fully initialized.")}
                    }}, 
                    object::pair{std:("text"), std:("No WORLD_ID configured - autonomous system may not be fully initialized.")}
                };
            }
            auto [world, room] = std::async([=]() { Promise->all(std::tuple<any, any>{ runtime->getWorld(worldId), runtime->getRoom(autonomousRoomId) }); });
            auto worldInfo = (world) ? any(object{
                object::pair{std:("id"), world->id}, 
                object::pair{std:("name"), world->name}, 
                object::pair{std:("serverId"), world->serverId}, 
                object::pair{std:("agentId"), world->agentId}
            }) (nullptr);
            auto roomInfo = (room) ? any(object{
                object::pair{std:("id"), room->id}, 
                object::pair{std:("name"), room->name}, 
                object::pair{std:("type"), room->type}, 
                object::pair{std:("worldId"), room->worldId}, 
                object::pair{std:("source"), room->source}
            }) (nullptr);
            auto statusText = (array<string>{ std:("# Autonomous World Status"), string_empty, std:("**World ID:** ") + worldId + string_empty, std:("**World Status:** ") + (world) ? std:("Found") : std:("Not Found") + string_empty, (world) ? any(std:("**World Name:** ") + world->name + string_empty) (string_empty), string_empty, std:("**Autonomous Room ID:** ") + autonomousRoomId + string_empty, std:("**Room Status:** ") + (room) ? std:("Found") : std:("Not Found") + string_empty, (room) ? any(std:("**Room Name:** ") + room->name + string_empty) (string_empty), (room) ? any(std:("**Room Type:** ") + room->type + string_empty) (string_empty), (room) ? any(std:("**Room World ID:** ") + room->worldId + string_empty) (string_empty), string_empty, std:("**Agent ID:** ") + runtime->agentId + string_empty, std:("**Character Name:** ") + runtime->character->name + string_empty })->filter(Boolean)->join(std:("\
"));
            auto formattedText = addHeader(std:("# Autonomous World Information"), statusText);
            return object{
                object::pair{std:("data"), object{
                    object::pair{std:("worldId"), std:("worldId")}, 
                    object::pair{std:("worldInfo"), std:("worldInfo")}, 
                    object::pair{std:("roomId"), autonomousRoomId}, 
                    object::pair{std:("roomInfo"), std:("roomInfo")}, 
                    object::pair{std:("agentId"), runtime->agentId}, 
                    object::pair{std:("characterName"), runtime->character->name}, 
                    object::pair{std:("status"), (AND((world), (room))) ? std:("ready") : std:("incomplete")}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("autonomousWorld"), statusText}, 
                    object::pair{std:("worldStatus"), (world) ? std:("ready") : std:("missing")}, 
                    object::pair{std:("roomStatus"), (room) ? std:("ready") : std:("missing")}
                }}, 
                object::pair{std:("text"), formattedText}
            };
        }
        catch (const any& error)
        {
            console->error(std:("[AutonomousWorldProvider] Error:"), error);
            return object{
                object::pair{std:("data"), object{
                    object::pair{std:("error"), error["message"]}, 
                    object::pair{std:("status"), std:("error")}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("autonomousWorld"), std:("Error retrieving autonomous world information: ") + error["message"] + string_empty}
                }}, 
                object::pair{std:("text"), std:("Error retrieving autonomous world information: ") + error["message"] + string_empty}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
