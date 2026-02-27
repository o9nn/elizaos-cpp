#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-bootstrap/src/providers/world.h"

std::shared_ptr<Provider> worldProvider = object{
    object::pair{std:("name"), std:("WORLD")}, 
    object::pair{std:("description"), std:("World and environment information")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto message) mutable
    {
        try
        {
            logger->debug(std:("[🌐] World provider activated for roomId:"), message->roomId);
            auto currentRoom = std::async([=]() { runtime->getRoom(message->roomId); });
            if (!currentRoom) {
                logger->warn(std:("World provider: Room not found for roomId ") + message->roomId + string_empty);
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("world"), object{
                            object::pair{std:("info"), std:("Unable to retrieve world information - room not found")}
                        }}
                    }}, 
                    object::pair{std:("text"), std:("Unable to retrieve world information - room not found")}
                };
            }
            logger->debug(std:("[🌐] World provider: Found room "") + currentRoom->name + std:("" (") + currentRoom->type + std:(")"));
            auto worldId = currentRoom->worldId;
            if (!worldId) {
                logger->warn(std:("World provider: World ID not found for roomId ") + message->roomId + string_empty);
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("world"), object{
                            object::pair{std:("info"), std:("Unable to retrieve world information - world ID not found")}
                        }}
                    }}, 
                    object::pair{std:("text"), std:("Unable to retrieve world information - world ID not found")}
                };
            }
            auto world = std::async([=]() { runtime->getWorld(worldId); });
            if (!world) {
                logger->warn(std:("World provider: World not found for worldId ") + worldId + string_empty);
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("world"), object{
                            object::pair{std:("info"), std:("Unable to retrieve world information - world not found")}
                        }}
                    }}, 
                    object::pair{std:("text"), std:("Unable to retrieve world information - world not found")}
                };
            }
            logger->debug(std:("[🌐] World provider: Found world "") + world->name + std:("" (ID: ") + world->id + std:(")"));
            auto worldRooms = std::async([=]() { runtime->getRooms(worldId); });
            logger->debug(std:("[🌐] World provider: Found ") + worldRooms->length + std:(" rooms in world "") + world->name + std:("""));
            auto participants = std::async([=]() { runtime->getParticipantsForRoom(message->roomId); });
            logger->debug(std:("[🌐] World provider: Found ") + participants->length + std:(" participants in room "") + currentRoom->name + std:("""));
            auto channelsByType = object{
                object::pair{std:("text"), array<any>()}, 
                object::pair{std:("voice"), array<any>()}, 
                object::pair{std:("dm"), array<any>()}, 
                object::pair{std:("feed"), array<any>()}, 
                object::pair{std:("thread"), array<any>()}, 
                object::pair{std:("other"), array<any>()}
            };
            for (auto& room : worldRooms)
            {
                if (OR((!room->id), (!room->name))) {
                    logger->warn(std:("World provider: Room ID or name is missing for room ") + room->id + string_empty);
                    continue;
                }
                auto roomInfo = object{
                    object::pair{std:("id"), room->id}, 
                    object::pair{std:("name"), room->name}, 
                    object::pair{std:("isCurrentChannel"), room->id == message->roomId}
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
                        object::pair{std:("type"), room->type}
                    }, roomInfo));
                }
            }
            auto worldInfoText = (array<string>{ std:("# World: ") + world->name + string_empty, std:("Current Channel: ") + currentRoom->name + std:(" (") + currentRoom->type + std:(")"), std:("Total Channels: ") + worldRooms->length + string_empty, std:("Participants in current channel: ") + participants->length + string_empty, string_empty, std:("Text channels: ") + channelsByType->text->get_length() + string_empty, std:("Voice channels: ") + channelsByType->voice->get_length() + string_empty, std:("DM channels: ") + channelsByType->dm->get_length() + string_empty, std:("Feed channels: ") + channelsByType->feed->get_length() + string_empty, std:("Thread channels: ") + channelsByType->thread->get_length() + string_empty, std:("Other channels: ") + channelsByType->other->get_length() + string_empty })->join(std:("\
"));
            auto data = object{
                object::pair{std:("world"), object{
                    object::pair{std:("id"), world->id}, 
                    object::pair{std:("name"), world->name}, 
                    object::pair{std:("serverId"), world->serverId}, 
                    object::pair{std:("metadata"), OR((world->metadata), (object{}))}, 
                    object::pair{std:("currentRoom"), object{
                        object::pair{std:("id"), currentRoom->id}, 
                        object::pair{std:("name"), currentRoom->name}, 
                        object::pair{std:("type"), currentRoom->type}, 
                        object::pair{std:("channelId"), currentRoom->channelId}, 
                        object::pair{std:("participantCount"), participants->length}
                    }}, 
                    object::pair{std:("channels"), channelsByType}, 
                    object::pair{std:("channelStats"), object{
                        object::pair{std:("total"), worldRooms->length}, 
                        object::pair{std:("text"), channelsByType->text->get_length()}, 
                        object::pair{std:("voice"), channelsByType->voice->get_length()}, 
                        object::pair{std:("dm"), channelsByType->dm->get_length()}, 
                        object::pair{std:("feed"), channelsByType->feed->get_length()}, 
                        object::pair{std:("thread"), channelsByType->thread->get_length()}, 
                        object::pair{std:("other"), channelsByType->other->get_length()}
                    }}
                }}
            };
            auto values = object{
                object::pair{std:("worldName"), world->name}, 
                object::pair{std:("currentChannelName"), currentRoom->name}, 
                object::pair{std:("worldInfo"), worldInfoText}
            };
            auto formattedText = addHeader(std:("# World Information"), worldInfoText);
            logger->debug(std:("[🌐] World provider completed successfully"));
            return object{
                object::pair{std:("data"), std:("data")}, 
                object::pair{std:("values"), std:("values")}, 
                object::pair{std:("text"), formattedText}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error in world provider: ") + (is<Error>(error)) ? error->message : String(error) + string_empty);
            return object{
                object::pair{std:("data"), object{
                    object::pair{std:("world"), object{
                        object::pair{std:("info"), std:("Error retrieving world information")}, 
                        object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}
                    }}
                }}, 
                object::pair{std:("text"), std:("Error retrieving world information")}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
