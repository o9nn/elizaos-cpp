#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autonomy/src/action.h"

std::shared_ptr<Action> sendToAdminAction = object{
    object::pair{std:("name"), std:("SEND_TO_ADMIN")}, 
    object::pair{std:("description"), std:("Send a message directly to the admin user from autonomous context")}, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("Agent")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I need to update the admin about my progress on the task.")}, 
            object::pair{std:("action"), std:("SEND_TO_ADMIN")}
        }}
    }, object{
        object::pair{std:("name"), std:("Agent")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Message sent to admin successfully.")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("Agent")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I should let the admin know I completed the analysis.")}, 
            object::pair{std:("action"), std:("SEND_TO_ADMIN")}
        }}
    }, object{
        object::pair{std:("name"), std:("Agent")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Admin has been notified of the analysis completion.")}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        auto autonomyService = runtime->getService(std:("autonomy"));
        if (!autonomyService) {
            return false;
        }
        auto autonomousRoomId = (as<any>(autonomyService))["getAutonomousRoomId"]();
        if (OR((!autonomousRoomId), (message->roomId != autonomousRoomId))) {
            return false;
        }
        auto adminUserId = as<string>(runtime->getSetting(std:("ADMIN_USER_ID")));
        if (!adminUserId) {
            return false;
        }
        shared text = OR((message->content->text->toLowerCase()), (string_empty));
        auto adminKeywords = array<string>{ std:("admin"), std:("user"), std:("tell"), std:("notify"), std:("inform"), std:("update"), std:("message"), std:("send"), std:("communicate"), std:("report"), std:("alert") };
        return adminKeywords->some([=](auto keyword) mutable
        {
            return text["includes"](keyword);
        }
        );
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        auto autonomyService = runtime->getService(std:("autonomy"));
        if (!autonomyService) {
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("text"), std:("Autonomy service not available")}, 
                object::pair{std:("data"), object{
                    object::pair{std:("error"), std:("Service unavailable")}
                }}
            };
        }
        auto autonomousRoomId = (as<any>(autonomyService))["getAutonomousRoomId"]();
        if (OR((!autonomousRoomId), (message->roomId != autonomousRoomId))) {
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("text"), std:("Send to admin only available in autonomous context")}, 
                object::pair{std:("data"), object{
                    object::pair{std:("error"), std:("Invalid context")}
                }}
            };
        }
        auto adminUserId = as<string>(runtime->getSetting(std:("ADMIN_USER_ID")));
        if (!adminUserId) {
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("text"), std:("No admin user configured. Set ADMIN_USER_ID in settings.")}, 
                object::pair{std:("data"), object{
                    object::pair{std:("error"), std:("No admin configured")}
                }}
            };
        }
        auto _adminUUID = asUUID(adminUserId);
        auto adminMessages = std::async([=]() { runtime->getMemories(object{
            object::pair{std:("roomId"), runtime->agentId}, 
            object::pair{std:("count"), 10}, 
            object::pair{std:("tableName"), std:("memories")}
        }); });
        std::shared_ptr<UUID> targetRoomId;
        if (AND((adminMessages), (adminMessages->length > 0))) {
            targetRoomId = const_(adminMessages)[adminMessages->length - 1]->roomId;
        } else {
            targetRoomId = runtime->agentId;
        }
        auto autonomousThought = OR((message->content->text), (string_empty));
        string messageToAdmin;
        if (OR((autonomousThought["includes"](std:("completed"))), (autonomousThought["includes"](std:("finished"))))) {
            messageToAdmin = std:("I've completed a task and wanted to update you. My thoughts: ") + autonomousThought + string_empty;
        } else if (OR((OR((autonomousThought["includes"](std:("problem"))), (autonomousThought["includes"](std:("issue"))))), (autonomousThought["includes"](std:("error"))))) {
            messageToAdmin = std:("I encountered something that might need your attention: ") + autonomousThought + string_empty;
        } else if (OR((autonomousThought["includes"](std:("question"))), (autonomousThought["includes"](std:("unsure"))))) {
            messageToAdmin = std:("I have a question and would appreciate your guidance: ") + autonomousThought + string_empty;
        } else {
            messageToAdmin = std:("Autonomous update: ") + autonomousThought + string_empty;
        }
        auto adminMessage = object{
            object::pair{std:("id"), asUUID(uuidv4())}, 
            object::pair{std:("entityId"), runtime->agentId}, 
            object::pair{std:("roomId"), targetRoomId}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), messageToAdmin}, 
                object::pair{std:("source"), std:("autonomy-to-admin")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("type"), std:("autonomous-to-admin-message")}, 
                    object::pair{std:("originalThought"), autonomousThought}, 
                    object::pair{std:("timestamp"), Date->now()}
                }}
            }}, 
            object::pair{std:("createdAt"), Date->now()}
        };
        std::async([=]() { runtime->createMemory(adminMessage, std:("memories")); });
        auto successMessage = std:("Message sent to admin in room ") + targetRoomId->slice(0, 8) + std:("...");
        if (callback) {
            std::async([=]() { callback(object{
                object::pair{std:("text"), successMessage}, 
                object::pair{std:("data"), object{
                    object::pair{std:("adminUserId"), std:("adminUserId")}, 
                    object::pair{std:("targetRoomId"), std:("targetRoomId")}, 
                    object::pair{std:("messageContent"), messageToAdmin}
                }}
            }); });
        }
        return object{
            object::pair{std:("success"), true}, 
            object::pair{std:("text"), successMessage}, 
            object::pair{std:("data"), object{
                object::pair{std:("adminUserId"), std:("adminUserId")}, 
                object::pair{std:("targetRoomId"), std:("targetRoomId")}, 
                object::pair{std:("messageContent"), messageToAdmin}, 
                object::pair{std:("sent"), true}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
