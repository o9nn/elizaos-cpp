#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autonomy/src/provider.h"

std::shared_ptr<Provider> adminChatProvider = object{
    object::pair{std::string("name"), std::string("ADMIN_CHAT_HISTORY")}, 
    object::pair{std::string("description"), std::string("Provides recent conversation history with the admin user for autonomous context")}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto autonomyService = runtime->getService<std::shared_ptr<AutonomyService>>(std::string("AUTONOMY"));
        if (!autonomyService) {
            console->error(std::string("Autonomy service not available"));
            return object{
                object::pair{std::string("text"), string_empty}
            };
        }
        auto autonomousRoomId = autonomyService->getAutonomousRoomId();
        if (OR((!autonomousRoomId), (message->roomId != autonomousRoomId))) {
            return object{
                object::pair{std::string("text"), string_empty}
            };
        }
        auto adminUserId = as<string>(runtime->getSetting(std::string("ADMIN_USER_ID")));
        if (!adminUserId) {
            return object{
                object::pair{std::string("text"), std::string("[ADMIN_CHAT_HISTORY]\
No admin user configured. Set ADMIN_USER_ID in character settings.\
[/ADMIN_CHAT_HISTORY]")}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("adminConfigured"), false}
                }}
            };
        }
        shared adminUUID = asUUID(adminUserId);
        auto adminMessages = std::async([=]() { runtime->getMemories(object{
            object::pair{std::string("entityId"), adminUUID}, 
            object::pair{std::string("count"), 15}, 
            object::pair{std::string("unique"), false}, 
            object::pair{std::string("tableName"), std::string("memories")}
        }); });
        if (OR((!adminMessages), (adminMessages->length == 0))) {
            return object{
                object::pair{std::string("text"), std::string("[ADMIN_CHAT_HISTORY]\
No recent messages found with admin user.\
[/ADMIN_CHAT_HISTORY]")}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("adminConfigured"), true}, 
                    object::pair{std::string("messageCount"), 0}, 
                    object::pair{std::string("adminUserId"), std::string("adminUserId")}
                }}
            };
        }
        auto conversationHistory = adminMessages->sort([=](auto a, auto b) mutable
        {
            return (OR((a["createdAt"]), (0))) - (OR((b["createdAt"]), (0)));
        }
        )->slice(-10)->map([=](auto msg) mutable
        {
            auto isFromAdmin = msg["entityId"] == adminUUID;
            auto isFromAgent = msg["entityId"] == runtime->agentId;
            auto sender = (isFromAdmin) ? any(std::string("Admin")) : any((isFromAgent) ? std::string("Agent") : std::string("Other"));
            auto text = OR((msg["content"]["text"]), (std::string("[No text content]")));
            auto timestamp = ((std::make_shared<Date>(OR((msg["createdAt"]), (0)))))->toLocaleTimeString();
            return string_empty + timestamp + std::string(" ") + sender + std::string(": ") + text + string_empty;
        }
        )->join(std::string("\n"));
        auto recentAdminMessages = adminMessages->filter([=](auto msg) mutable
        {
            return msg["entityId"] == adminUUID;
        }
        )->slice(-3);
        auto lastAdminMessage = const_(recentAdminMessages)[recentAdminMessages->length - 1];
        auto adminMoodContext = (recentAdminMessages->length > 0) ? any(std::string("Last admin message: "") + (OR((lastAdminMessage->content->text), (std::string("N/A")))) + std::string(""")) : any(std::string("No recent admin messages"));
        return object{
            object::pair{std::string("text"), std::string("[ADMIN_CHAT_HISTORY]\
Recent conversation with admin user (") + adminMessages->length + std::string(" total messages):\
\
") + conversationHistory + std::string("\
\
") + adminMoodContext + std::string("\
[/ADMIN_CHAT_HISTORY]")}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("adminConfigured"), true}, 
                object::pair{std::string("messageCount"), adminMessages->length}, 
                object::pair{std::string("adminUserId"), std::string("adminUserId")}, 
                object::pair{std::string("recentMessageCount"), recentAdminMessages->length}, 
                object::pair{std::string("lastAdminMessage"), OR((lastAdminMessage->content->text), (string_empty))}, 
                object::pair{std::string("conversationActive"), adminMessages->some([=](auto m) mutable
                {
                    return Date->now() - (OR((m["createdAt"]), (0))) < 3600000;
                }
                )}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
