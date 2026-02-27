#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-bootstrap/src/actions/unmuteRoom.h"

string shouldUnmuteTemplate = std::string("# Task: Decide if {{agentName}} should unmute this previously muted room and start considering it for responses again.\
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
    object::pair{std::string("name"), std::string("UNMUTE_ROOM")}, 
    object::pair{std::string("similes"), array<string>{ std::string("UNMUTE_CHAT"), std::string("UNMUTE_CONVERSATION"), std::string("UNMUTE_ROOM"), std::string("UNMUTE_THREAD") }}, 
    object::pair{std::string("description"), std::string("Unmutes a room, allowing the agent to consider responding to messages again.")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        auto roomId = message->roomId;
        auto roomState = std::async([=]() { runtime->getParticipantUserState(roomId, runtime->agentId); });
        return roomState == std::string("MUTED");
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto _options = undefined, auto _callback = undefined, auto _responses = undefined) mutable
    {
        auto _shouldUnmute = [=](auto state) mutable
        {
            auto shouldUnmutePrompt = composePromptFromState(object{
                object::pair{std::string("state"), std::string("state")}, 
                object::pair{std::string("template"), shouldUnmuteTemplate}
            });
            auto response = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
                object::pair{std::string("runtime"), std::string("runtime")}, 
                object::pair{std::string("prompt"), shouldUnmutePrompt}, 
                object::pair{std::string("stopSequences"), array<any>()}
            }); });
            auto cleanedResponse = response->trim()->toLowerCase();
            if (OR((OR((OR((OR((cleanedResponse == std::string("true")), (cleanedResponse == std::string("yes")))), (cleanedResponse == std::string("y")))), (cleanedResponse->includes(std::string("true"))))), (cleanedResponse->includes(std::string("yes"))))) {
                std::async([=]() { runtime->createMemory(object{
                    object::pair{std::string("entityId"), message->entityId}, 
                    object::pair{std::string("agentId"), message->agentId}, 
                    object::pair{std::string("roomId"), message->roomId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("source"), message->content->source}, 
                        object::pair{std::string("thought"), std::string("I will now unmute this room and start considering it for responses again")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UNMUTE_ROOM_STARTED") }}
                    }}
                }, std::string("messages")); });
                return true;
            }
            if (OR((OR((OR((OR((cleanedResponse == std::string("false")), (cleanedResponse == std::string("no")))), (cleanedResponse == std::string("n")))), (cleanedResponse->includes(std::string("false"))))), (cleanedResponse->includes(std::string("no"))))) {
                std::async([=]() { runtime->createMemory(object{
                    object::pair{std::string("entityId"), message->entityId}, 
                    object::pair{std::string("agentId"), message->agentId}, 
                    object::pair{std::string("roomId"), message->roomId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("source"), message->content->source}, 
                        object::pair{std::string("thought"), std::string("I tried to unmute a room but I decided not to")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UNMUTE_ROOM_FAILED") }}
                    }}
                }, std::string("messages")); });
                return false;
            }
            logger->warn(std::string("Unclear boolean response: ") + response + std::string(", defaulting to false"));
            return false;
        };

        if (!state) {
            return object{
                object::pair{std::string("text"), std::string("State is required for unmute room action")}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("STATE_REQUIRED")}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("UNMUTE_ROOM")}, 
                    object::pair{std::string("error"), std::string("State is required")}
                }}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), std::make_shared<Error>(std::string("State is required for unmute room action"))}
            };
        }
        auto shouldUnmute = std::async([=]() { _shouldUnmute(state); });
        if (shouldUnmute) {
            try
            {
                std::async([=]() { runtime->setParticipantUserState(message->roomId, runtime->agentId, nullptr); });
                auto room = std::async([=]() { runtime->getRoom(message->roomId); });
                if (!room) {
                    logger->warn(std::string("Room not found: ") + message->roomId + string_empty);
                    return object{
                        object::pair{std::string("text"), std::string("Room not found: ") + message->roomId + string_empty}, 
                        object::pair{std::string("values"), object{
                            object::pair{std::string("success"), false}, 
                            object::pair{std::string("error"), std::string("ROOM_NOT_FOUND")}, 
                            object::pair{std::string("roomId"), message->roomId}
                        }}, 
                        object::pair{std::string("data"), object{
                            object::pair{std::string("actionName"), std::string("UNMUTE_ROOM")}, 
                            object::pair{std::string("error"), std::string("Room not found")}, 
                            object::pair{std::string("roomId"), message->roomId}
                        }}, 
                        object::pair{std::string("success"), false}
                    };
                }
                std::async([=]() { runtime->createMemory(object{
                    object::pair{std::string("entityId"), message->entityId}, 
                    object::pair{std::string("agentId"), message->agentId}, 
                    object::pair{std::string("roomId"), message->roomId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("thought"), std::string("I unmuted the room ") + room->name + string_empty}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UNMUTE_ROOM_START") }}
                    }}
                }, std::string("messages")); });
                return object{
                    object::pair{std::string("text"), std::string("Room unmuted: ") + room->name + string_empty}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), true}, 
                        object::pair{std::string("roomUnmuted"), true}, 
                        object::pair{std::string("roomId"), message->roomId}, 
                        object::pair{std::string("roomName"), room->name}, 
                        object::pair{std::string("newState"), nullptr}
                    }}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("UNMUTE_ROOM")}, 
                        object::pair{std::string("roomId"), message->roomId}, 
                        object::pair{std::string("roomName"), room->name}, 
                        object::pair{std::string("unmuted"), true}
                    }}, 
                    object::pair{std::string("success"), true}
                };
            }
            catch (const any& error)
            {
                logger->error(std::string("Error unmuting room:"), error);
                return object{
                    object::pair{std::string("text"), std::string("Failed to unmute room")}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("UNMUTE_FAILED")}
                    }}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("UNMUTE_ROOM")}, 
                        object::pair{std::string("error"), (is<Error>(error)) ? error->message : String(error)}, 
                        object::pair{std::string("roomId"), message->roomId}
                    }}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), (is<Error>(error)) ? error : std::make_shared<Error>(String(error))}
                };
            }
        } else {
            return object{
                object::pair{std::string("text"), std::string("Decided not to unmute room")}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("roomUnmuted"), false}, 
                    object::pair{std::string("roomId"), message->roomId}, 
                    object::pair{std::string("reason"), std::string("CRITERIA_NOT_MET")}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("UNMUTE_ROOM")}, 
                    object::pair{std::string("roomId"), message->roomId}, 
                    object::pair{std::string("unmuted"), false}, 
                    object::pair{std::string("reason"), std::string("Decision criteria not met")}
                }}, 
                object::pair{std::string("success"), true}
            };
        }
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("{{name3}}, you can unmute this channel now")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name3}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Done")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UNMUTE_ROOM") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I could use some help troubleshooting this bug.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name3}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Can you post the specific error message")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("{{name2}}, please unmute this room. We could use your input again.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Sounds good")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UNMUTE_ROOM") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("{{name2}} wait you should come back and chat in here")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("im back")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UNMUTE_ROOM") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("unmute urself {{name2}}")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("unmuted")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UNMUTE_ROOM") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("ay {{name2}} get back in here")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("sup yall")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UNMUTE_ROOM") }}
        }}
    } } })}
});

void Main(void)
{
}

MAIN
