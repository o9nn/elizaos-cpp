#include "unmuteRoom.hpp"

string shouldUnmuteTemplate = std:("# Task: Decide if {{agentName}} should unmute this previously muted room and start considering it for responses again.\
\
{{recentMessages}}\
\
Should {{agentName}} unmute this previously muted room and start considering it for responses again?\
Respond with YES if:\
- The user has explicitly asked {{agentName}} to start responding again\
- The user seems to want to re-engage with {{agentName}} in a respectful manner\
- The tone of the conversation has improved and {{agentName}}'s input would be welcome\
\
Otherwise, respond with NO.\
") + booleanFooter + string_empty;
std::shared_ptr<Action> unmuteRoomAction = as<std::shared_ptr<Action>>(object{
    object::pair{std:("name"), std:("UNMUTE_ROOM")}, 
    object::pair{std:("similes"), array<string>{ std:("UNMUTE_CHAT"), std:("UNMUTE_CONVERSATION"), std:("UNMUTE_ROOM"), std:("UNMUTE_THREAD") }}, 
    object::pair{std:("description"), std:("Unmutes a room, allowing the agent to consider responding to messages again.")}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        auto roomId = message->roomId;
        auto roomState = std::async([=]() { runtime->getParticipantUserState(roomId, runtime->agentId); });
        return roomState == std:("MUTED");
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto _options = undefined, auto _callback = undefined, auto _responses = undefined) mutable
    {
        auto _shouldUnmute = [=](auto state) mutable
        {
            auto shouldUnmutePrompt = composePromptFromState(object{
                object::pair{std:("state"), std:("state")}, 
                object::pair{std:("template"), shouldUnmuteTemplate}
            });
            auto response = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
                object::pair{std:("runtime"), std:("runtime")}, 
                object::pair{std:("prompt"), shouldUnmutePrompt}, 
                object::pair{std:("stopSequences"), array<any>()}
            }); });
            auto cleanedResponse = response->trim()->toLowerCase();
            if (OR((OR((OR((OR((cleanedResponse == std:("true")), (cleanedResponse == std:("yes")))), (cleanedResponse == std:("y")))), (cleanedResponse->includes(std:("true"))))), (cleanedResponse->includes(std:("yes"))))) {
                std::async([=]() { runtime->createMemory(object{
                    object::pair{std:("entityId"), message->entityId}, 
                    object::pair{std:("agentId"), message->agentId}, 
                    object::pair{std:("roomId"), message->roomId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("source"), message->content->source}, 
                        object::pair{std:("thought"), std:("I will now unmute this room and start considering it for responses again")}, 
                        object::pair{std:("actions"), array<string>{ std:("UNMUTE_ROOM_STARTED") }}
                    }}
                }, std:("messages")); });
                return true;
            }
            if (OR((OR((OR((OR((cleanedResponse == std:("false")), (cleanedResponse == std:("no")))), (cleanedResponse == std:("n")))), (cleanedResponse->includes(std:("false"))))), (cleanedResponse->includes(std:("no"))))) {
                std::async([=]() { runtime->createMemory(object{
                    object::pair{std:("entityId"), message->entityId}, 
                    object::pair{std:("agentId"), message->agentId}, 
                    object::pair{std:("roomId"), message->roomId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("source"), message->content->source}, 
                        object::pair{std:("thought"), std:("I tried to unmute a room but I decided not to")}, 
                        object::pair{std:("actions"), array<string>{ std:("UNMUTE_ROOM_FAILED") }}
                    }}
                }, std:("messages")); });
                return false;
            }
            logger->warn(std:("Unclear boolean response: ") + response + std:(", defaulting to false"));
            return false;
        };

        if (!state) {
            return object{
                object::pair{std:("text"), std:("State is required for unmute room action")}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("STATE_REQUIRED")}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("UNMUTE_ROOM")}, 
                    object::pair{std:("error"), std:("State is required")}
                }}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std::make_shared<Error>(std:("State is required for unmute room action"))}
            };
        }
        auto shouldUnmute = std::async([=]() { _shouldUnmute(state); });
        if (shouldUnmute) {
            try
            {
                std::async([=]() { runtime->setParticipantUserState(message->roomId, runtime->agentId, nullptr); });
                auto room = std::async([=]() { runtime->getRoom(message->roomId); });
                if (!room) {
                    logger->warn(std:("Room not found: ") + message->roomId + string_empty);
                    return object{
                        object::pair{std:("text"), std:("Room not found: ") + message->roomId + string_empty}, 
                        object::pair{std:("values"), object{
                            object::pair{std:("success"), false}, 
                            object::pair{std:("error"), std:("ROOM_NOT_FOUND")}, 
                            object::pair{std:("roomId"), message->roomId}
                        }}, 
                        object::pair{std:("data"), object{
                            object::pair{std:("actionName"), std:("UNMUTE_ROOM")}, 
                            object::pair{std:("error"), std:("Room not found")}, 
                            object::pair{std:("roomId"), message->roomId}
                        }}, 
                        object::pair{std:("success"), false}
                    };
                }
                std::async([=]() { runtime->createMemory(object{
                    object::pair{std:("entityId"), message->entityId}, 
                    object::pair{std:("agentId"), message->agentId}, 
                    object::pair{std:("roomId"), message->roomId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("thought"), std:("I unmuted the room ") + room->name + string_empty}, 
                        object::pair{std:("actions"), array<string>{ std:("UNMUTE_ROOM_START") }}
                    }}
                }, std:("messages")); });
                return object{
                    object::pair{std:("text"), std:("Room unmuted: ") + room->name + string_empty}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), true}, 
                        object::pair{std:("roomUnmuted"), true}, 
                        object::pair{std:("roomId"), message->roomId}, 
                        object::pair{std:("roomName"), room->name}, 
                        object::pair{std:("newState"), nullptr}
                    }}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("UNMUTE_ROOM")}, 
                        object::pair{std:("roomId"), message->roomId}, 
                        object::pair{std:("roomName"), room->name}, 
                        object::pair{std:("unmuted"), true}
                    }}, 
                    object::pair{std:("success"), true}
                };
            }
            catch (const any& error)
            {
                logger->error(std:("Error unmuting room:"), error);
                return object{
                    object::pair{std:("text"), std:("Failed to unmute room")}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), std:("UNMUTE_FAILED")}
                    }}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("UNMUTE_ROOM")}, 
                        object::pair{std:("error"), (is<Error>(error)) ? error->message : String(error)}, 
                        object::pair{std:("roomId"), message->roomId}
                    }}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), (is<Error>(error)) ? error : std::make_shared<Error>(String(error))}
                };
            }
        } else {
            return object{
                object::pair{std:("text"), std:("Decided not to unmute room")}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("roomUnmuted"), false}, 
                    object::pair{std:("roomId"), message->roomId}, 
                    object::pair{std:("reason"), std:("CRITERIA_NOT_MET")}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("UNMUTE_ROOM")}, 
                    object::pair{std:("roomId"), message->roomId}, 
                    object::pair{std:("unmuted"), false}, 
                    object::pair{std:("reason"), std:("Decision criteria not met")}
                }}, 
                object::pair{std:("success"), true}
            };
        }
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("{{name3}}, you can unmute this channel now")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name3}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Done")}, 
            object::pair{std:("actions"), array<string>{ std:("UNMUTE_ROOM") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I could use some help troubleshooting this bug.")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name3}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Can you post the specific error message")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("{{name2}}, please unmute this room. We could use your input again.")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Sounds good")}, 
            object::pair{std:("actions"), array<string>{ std:("UNMUTE_ROOM") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("{{name2}} wait you should come back and chat in here")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("im back")}, 
            object::pair{std:("actions"), array<string>{ std:("UNMUTE_ROOM") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("unmute urself {{name2}}")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("unmuted")}, 
            object::pair{std:("actions"), array<string>{ std:("UNMUTE_ROOM") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("ay {{name2}} get back in here")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("sup yall")}, 
            object::pair{std:("actions"), array<string>{ std:("UNMUTE_ROOM") }}
        }}
    } } })}
});

void Main(void)
{
}

MAIN
