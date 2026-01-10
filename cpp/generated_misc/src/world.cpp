#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-bootstrap/providers/world.h"

std::shared_ptr<Provider> worldProvider = object{
    object::pair{std::string("name"), std::string("WORLD")}, 
    object::pair{std::string("description"), std::string("World and environment information")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto message) mutable
    {
        try
        {
            logger->debug(std::string("🌐 World provider activated for roomId:"), message->roomId);
            auto currentRoom = std::async([=]() { runtime->getRoom(message->roomId); });
            if (!currentRoom) {
                logger->warn(std::string("World provider: Room not found for roomId ") + message->roomId + string_empty);
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("world"), object{
                            object::pair{std::string("info"), std::string("Unable to retrieve world information - room not found")}
                        }}
                    }}, 
                    object::pair{std::string("text"), std::string("Unable to retrieve world information - room not found")}
                };
            }
            logger->debug(std::string("🌐 World provider: Found room "") + currentRoom->name + std::string("" (") + currentRoom->type + std::string(")"));
            auto worldId = currentRoom->worldId;
            auto world = std::async([=]() { runtime->getWorld(worldId); });
            if (!world) {
                logger->warn(std::string("World provider: World not found for worldId ") + worldId + string_empty);
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("world"), object{
                            object::pair{std::string("info"), std::string("Unable to retrieve world information - world not found")}
                        }}
                    }}, 
                    object::pair{std::string("text"), std::string("Unable to retrieve world information - world not found")}
                };
            }
            logger->debug(std::string("🌐 World provider: Found world "") + world->name + std::string("" (ID: ") + world->id + std::string(")"));
            auto worldRooms = std::async([=]() { runtime->getRooms(worldId); });
            logger->debug(std::string("🌐 World provider: Found ") + worldRooms->length + std::string(" rooms in world "") + world->name + std::string("""));
            auto participants = std::async([=]() { runtime->getParticipantsForRoom(message->roomId); });
            logger->debug(std::string("🌐 World provider: Found ") + participants->length + std::string(" participants in room "") + currentRoom->name + std::string("""));
            auto channelsByType = object{
                object::pair{std::string("text"), array<any>()}, 
                object::pair{std::string("voice"), array<any>()}, 
                object::pair{std::string("dm"), array<any>()}, 
                object::pair{std::string("feed"), array<any>()}, 
                object::pair{std::string("thread"), array<any>()}, 
                object::pair{std::string("other"), array<any>()}
            };
            for (auto& room : worldRooms)
            {
                auto roomInfo = object{
                    object::pair{std::string("id"), room->id}, 
                    object::pair{std::string("name"), room->name}, 
                    object::pair{std::string("isCurrentChannel"), room->id == message->roomId}
                };
                if (OR((OR((room->type == ChannelType->GROUP), (room->type == ChannelType->WORLD))), (room->type == ChannelType->FORUM))) {
                    channelsByType->text->push(roomInfo);
                } else if (OR((room->type == ChannelType->VOICE_GROUP), (room->type == ChannelType->VOICE_DM))) {
                    channelsByType->voice->push(roomInfo);
                } else if (OR((room->type == ChannelType->DM), (room->type == ChannelType->SELF))) {
                    channelsByType->dm->push(roomInfo);
                } else if (room->type == ChannelType->FEED) {
                    channelsByType->feed->push(roomInfo);
                } else if (room->type == ChannelType->THREAD) {
                    channelsByType->thread->push(roomInfo);
                } else {
                    channelsByType->other->push(utils::assign(object{
                        , 
                        object::pair{std::string("type"), room->type}
                    }, roomInfo));
                }
            }
            auto worldInfoText = (array<string>{ std::string("# World: ") + world->name + string_empty, std::string("Current Channel: ") + currentRoom->name + std::string(" (") + currentRoom->type + std::string(")"), std::string("Total Channels: ") + worldRooms->length + string_empty, std::string("Participants in current channel: ") + participants->length + string_empty, string_empty, std::string("Text channels: ") + channelsByType->text->get_length() + string_empty, std::string("Voice channels: ") + channelsByType->voice->get_length() + string_empty, std::string("DM channels: ") + channelsByType->dm->get_length() + string_empty, std::string("Feed channels: ") + channelsByType->feed->get_length() + string_empty, std::string("Thread channels: ") + channelsByType->thread->get_length() + string_empty, std::string("Other channels: ") + channelsByType->other->get_length() + string_empty })->join(std::string("\
"));
            auto data = object{
                object::pair{std::string("world"), object{
                    object::pair{std::string("id"), world->id}, 
                    object::pair{std::string("name"), world->name}, 
                    object::pair{std::string("serverId"), world->serverId}, 
                    object::pair{std::string("metadata"), OR((world->metadata), (object{}))}, 
                    object::pair{std::string("currentRoom"), object{
                        object::pair{std::string("id"), currentRoom->id}, 
                        object::pair{std::string("name"), currentRoom->name}, 
                        object::pair{std::string("type"), currentRoom->type}, 
                        object::pair{std::string("channelId"), currentRoom->channelId}, 
                        object::pair{std::string("participantCount"), participants->length}
                    }}, 
                    object::pair{std::string("channels"), channelsByType}, 
                    object::pair{std::string("channelStats"), object{
                        object::pair{std::string("total"), worldRooms->length}, 
                        object::pair{std::string("text"), channelsByType->text->get_length()}, 
                        object::pair{std::string("voice"), channelsByType->voice->get_length()}, 
                        object::pair{std::string("dm"), channelsByType->dm->get_length()}, 
                        object::pair{std::string("feed"), channelsByType->feed->get_length()}, 
                        object::pair{std::string("thread"), channelsByType->thread->get_length()}, 
                        object::pair{std::string("other"), channelsByType->other->get_length()}
                    }}
                }}
            };
            auto values = object{
                object::pair{std::string("worldName"), world->name}, 
                object::pair{std::string("currentChannelName"), currentRoom->name}, 
                object::pair{std::string("worldInfo"), worldInfoText}
            };
            auto formattedText = addHeader(std::string("# World Information"), worldInfoText);
            logger->debug(std::string("🌐 World provider completed successfully"));
            return object{
                object::pair{std::string("data"), std::string("data")}, 
                object::pair{std::string("values"), std::string("values")}, 
                object::pair{std::string("text"), formattedText}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in world provider: ") + (is<Error>(error)) ? error->message : String(error) + string_empty);
            return object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("world"), object{
                        object::pair{std::string("info"), std::string("Error retrieving world information")}, 
                        object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"))}
                    }}
                }}, 
                object::pair{std::string("text"), std::string("Error retrieving world information")}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
