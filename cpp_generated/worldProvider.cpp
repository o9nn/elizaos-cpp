#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-auto/worldProvider.h"

string AUTO_WORLD_SEED = std::string("autonomous_world_singleton");
string AUTO_ROOM_SEED = std::string("autonomous_room_singleton");
std::shared_ptr<Provider> autonomousWorldProvider = object{
    object::pair{std::string("name"), std::string("AUTONOMOUS_WORLD")}, 
    object::pair{std::string("description"), std::string("Information about the autonomous world and room setup")}, 
    object::pair{std::string("position"), 50}, 
    object::pair{std::string("get"), [=](auto runtime, auto message) mutable
    {
        try
        {
            auto worldId = as<std::shared_ptr<UUID>>(runtime->getSetting(std::string("WORLD_ID")));
            auto autonomousRoomId = createUniqueUuid(runtime, AUTO_ROOM_SEED);
            if (!worldId) {
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("worldId"), nullptr}, 
                        object::pair{std::string("roomId"), autonomousRoomId}, 
                        object::pair{std::string("status"), std::string("no_world_id")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("autonomousWorld"), std::string("No WORLD_ID configured - autonomous system may not be fully initialized.")}
                    }}, 
                    object::pair{std::string("text"), std::string("No WORLD_ID configured - autonomous system may not be fully initialized.")}
                };
            }
            auto [world, room] = std::async([=]() { Promise->all(std::tuple<any, any>{ runtime->getWorld(worldId), runtime->getRoom(autonomousRoomId) }); });
            auto worldInfo = (world) ? any(object{
                object::pair{std::string("id"), world->id}, 
                object::pair{std::string("name"), world->name}, 
                object::pair{std::string("serverId"), world->serverId}, 
                object::pair{std::string("agentId"), world->agentId}
            }) : any(nullptr);
            auto roomInfo = (room) ? any(object{
                object::pair{std::string("id"), room->id}, 
                object::pair{std::string("name"), room->name}, 
                object::pair{std::string("type"), room->type}, 
                object::pair{std::string("worldId"), room->worldId}, 
                object::pair{std::string("source"), room->source}
            }) : any(nullptr);
            auto statusText = (array<string>{ std::string("# Autonomous World Status"), string_empty, std::string("**World ID:** ") + worldId + string_empty, std::string("**World Status:** ") + (world) ? std::string("Found") : std::string("Not Found") + string_empty, (world) ? any(std::string("**World Name:** ") + world->name + string_empty) : any(string_empty), string_empty, std::string("**Autonomous Room ID:** ") + autonomousRoomId + string_empty, std::string("**Room Status:** ") + (room) ? std::string("Found") : std::string("Not Found") + string_empty, (room) ? any(std::string("**Room Name:** ") + room->name + string_empty) : any(string_empty), (room) ? any(std::string("**Room Type:** ") + room->type + string_empty) : any(string_empty), (room) ? any(std::string("**Room World ID:** ") + room->worldId + string_empty) : any(string_empty), string_empty, std::string("**Agent ID:** ") + runtime->agentId + string_empty, std::string("**Character Name:** ") + runtime->character->name + string_empty })->filter(Boolean)->join(std::string("\
"));
            auto formattedText = addHeader(std::string("# Autonomous World Information"), statusText);
            return object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("worldId"), std::string("worldId")}, 
                    object::pair{std::string("worldInfo"), std::string("worldInfo")}, 
                    object::pair{std::string("roomId"), autonomousRoomId}, 
                    object::pair{std::string("roomInfo"), std::string("roomInfo")}, 
                    object::pair{std::string("agentId"), runtime->agentId}, 
                    object::pair{std::string("characterName"), runtime->character->name}, 
                    object::pair{std::string("status"), (AND((world), (room))) ? std::string("ready") : std::string("incomplete")}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("autonomousWorld"), statusText}, 
                    object::pair{std::string("worldStatus"), (world) ? std::string("ready") : std::string("missing")}, 
                    object::pair{std::string("roomStatus"), (room) ? std::string("ready") : std::string("missing")}
                }}, 
                object::pair{std::string("text"), formattedText}
            };
        }
        catch (const any& error)
        {
            console->error(std::string("[AutonomousWorldProvider] Error:"), error);
            return object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("error"), error["message"]}, 
                    object::pair{std::string("status"), std::string("error")}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("autonomousWorld"), std::string("Error retrieving autonomous world information: ") + error["message"] + string_empty}
                }}, 
                object::pair{std::string("text"), std::string("Error retrieving autonomous world information: ") + error["message"] + string_empty}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
