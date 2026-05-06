#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autonomy/src/action.h"

std::shared_ptr<Action> sendToAdminAction = object{
    object::pair{std::string("name"), std::string("SEND_TO_ADMIN")}, 
    object::pair{std::string("description"), std::string("Send a message directly to the admin user from autonomous context")}, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("Agent")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I need to update the admin about my progress on the task.")}, 
            object::pair{std::string("action"), std::string("SEND_TO_ADMIN")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("Agent")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Message sent to admin successfully.")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("Agent")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I should let the admin know I completed the analysis.")}, 
            object::pair{std::string("action"), std::string("SEND_TO_ADMIN")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("Agent")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Admin has been notified of the analysis completion.")}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        auto autonomyService = runtime->getService(std::string("autonomy"));
        if (!autonomyService) {
            return false;
        }
        auto autonomousRoomId = (as<any>(autonomyService))["getAutonomousRoomId"]();
        if (OR((!autonomousRoomId), (message->roomId != autonomousRoomId))) {
            return false;
        }
        auto adminUserId = as<string>(runtime->getSetting(std::string("ADMIN_USER_ID")));
        if (!adminUserId) {
            return false;
        }
        shared text = OR((message->content->text->toLowerCase()), (string_empty));
        auto adminKeywords = array<string>{ std::string("admin"), std::string("user"), std::string("tell"), std::string("notify"), std::string("inform"), std::string("update"), std::string("message"), std::string("send"), std::string("communicate"), std::string("report"), std::string("alert") };
        return adminKeywords->some([=](auto keyword) mutable
        {
            return text["includes"](keyword);
        }
        );
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        auto autonomyService = runtime->getService(std::string("autonomy"));
        if (!autonomyService) {
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("text"), std::string("Autonomy service not available")}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("error"), std::string("Service unavailable")}
                }}
            };
        }
        auto autonomousRoomId = (as<any>(autonomyService))["getAutonomousRoomId"]();
        if (OR((!autonomousRoomId), (message->roomId != autonomousRoomId))) {
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("text"), std::string("Send to admin only available in autonomous context")}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("error"), std::string("Invalid context")}
                }}
            };
        }
        auto adminUserId = as<string>(runtime->getSetting(std::string("ADMIN_USER_ID")));
        if (!adminUserId) {
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("text"), std::string("No admin user configured. Set ADMIN_USER_ID in settings.")}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("error"), std::string("No admin configured")}
                }}
            };
        }
        auto _adminUUID = asUUID(adminUserId);
        auto adminMessages = std::async([=]() { runtime->getMemories(object{
            object::pair{std::string("roomId"), runtime->agentId}, 
            object::pair{std::string("count"), 10}, 
            object::pair{std::string("tableName"), std::string("memories")}
        }); });
        std::shared_ptr<UUID> targetRoomId;
        if (AND((adminMessages), (adminMessages->length > 0))) {
            targetRoomId = const_(adminMessages)[adminMessages->length - 1]->roomId;
        } else {
            targetRoomId = runtime->agentId;
        }
        auto autonomousThought = OR((message->content->text), (string_empty));
        string messageToAdmin;
        if (OR((autonomousThought["includes"](std::string("completed"))), (autonomousThought["includes"](std::string("finished"))))) {
            messageToAdmin = std::string("I've completed a task and wanted to update you. My thoughts: ") + autonomousThought + string_empty;
        } else if (OR((OR((autonomousThought["includes"](std::string("problem"))), (autonomousThought["includes"](std::string("issue"))))), (autonomousThought["includes"](std::string("error"))))) {
            messageToAdmin = std::string("I encountered something that might need your attention: ") + autonomousThought + string_empty;
        } else if (OR((autonomousThought["includes"](std::string("question"))), (autonomousThought["includes"](std::string("unsure"))))) {
            messageToAdmin = std::string("I have a question and would appreciate your guidance: ") + autonomousThought + string_empty;
        } else {
            messageToAdmin = std::string("Autonomous update: ") + autonomousThought + string_empty;
        }
        auto adminMessage = object{
            object::pair{std::string("id"), asUUID(uuidv4())}, 
            object::pair{std::string("entityId"), runtime->agentId}, 
            object::pair{std::string("roomId"), targetRoomId}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), messageToAdmin}, 
                object::pair{std::string("source"), std::string("autonomy-to-admin")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("type"), std::string("autonomous-to-admin-message")}, 
                    object::pair{std::string("originalThought"), autonomousThought}, 
                    object::pair{std::string("timestamp"), Date->now()}
                }}
            }}, 
            object::pair{std::string("createdAt"), Date->now()}
        };
        std::async([=]() { runtime->createMemory(adminMessage, std::string("memories")); });
        auto successMessage = std::string("Message sent to admin in room ") + targetRoomId->slice(0, 8) + std::string("...");
        if (callback) {
            std::async([=]() { callback(object{
                object::pair{std::string("text"), successMessage}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("adminUserId"), std::string("adminUserId")}, 
                    object::pair{std::string("targetRoomId"), std::string("targetRoomId")}, 
                    object::pair{std::string("messageContent"), messageToAdmin}
                }}
            }); });
        }
        return object{
            object::pair{std::string("success"), true}, 
            object::pair{std::string("text"), successMessage}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("adminUserId"), std::string("adminUserId")}, 
                object::pair{std::string("targetRoomId"), std::string("targetRoomId")}, 
                object::pair{std::string("messageContent"), messageToAdmin}, 
                object::pair{std::string("sent"), true}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
