#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autonomy/src/provider.h"

std::shared_ptr<Provider> adminChatProvider = object{
    object::pair{std:("name"), std:("ADMIN_CHAT_HISTORY")}, 
    object::pair{std:("description"), std:("Provides recent conversation history with the admin user for autonomous context")}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto autonomyService = runtime->getService<std::shared_ptr<AutonomyService>>(std:("AUTONOMY"));
        if (!autonomyService) {
            console->error(std:("Autonomy service not available"));
            return object{
                object::pair{std:("text"), string_empty}
            };
        }
        auto autonomousRoomId = autonomyService->getAutonomousRoomId();
        if (OR((!autonomousRoomId), (message->roomId != autonomousRoomId))) {
            return object{
                object::pair{std:("text"), string_empty}
            };
        }
        auto adminUserId = as<string>(runtime->getSetting(std:("ADMIN_USER_ID")));
        if (!adminUserId) {
            return object{
                object::pair{std:("text"), std:("[ADMIN_CHAT_HISTORY]\
No admin user configured. Set ADMIN_USER_ID in character settings.\
[/ADMIN_CHAT_HISTORY]")}, 
                object::pair{std:("data"), object{
                    object::pair{std:("adminConfigured"), false}
                }}
            };
        }
        shared adminUUID = asUUID(adminUserId);
        auto adminMessages = std::async([=]() { runtime->getMemories(object{
            object::pair{std:("entityId"), adminUUID}, 
            object::pair{std:("count"), 15}, 
            object::pair{std:("unique"), false}, 
            object::pair{std:("tableName"), std:("memories")}
        }); });
        if (OR((!adminMessages), (adminMessages->length == 0))) {
            return object{
                object::pair{std:("text"), std:("[ADMIN_CHAT_HISTORY]\
No recent messages found with admin user.\
[/ADMIN_CHAT_HISTORY]")}, 
                object::pair{std:("data"), object{
                    object::pair{std:("adminConfigured"), true}, 
                    object::pair{std:("messageCount"), 0}, 
                    object::pair{std:("adminUserId"), std:("adminUserId")}
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
            auto sender = (isFromAdmin) ? any(std:("Admin")) ((isFromAgent) ? std:("Agent") : std:("Other"));
            auto text = OR((msg["content"]["text"]), (std:("[No text content]")));
            auto timestamp = ((std::make_shared<Date>(OR((msg["createdAt"]), (0)))))->toLocaleTimeString();
            return string_empty + timestamp + std:(" ") + sender + std:(": ") + text + string_empty;
        }
        )->join(std:("\n"));
        auto recentAdminMessages = adminMessages->filter([=](auto msg) mutable
        {
            return msg["entityId"] == adminUUID;
        }
        )->slice(-3);
        auto lastAdminMessage = const_(recentAdminMessages)[recentAdminMessages->length - 1];
        auto adminMoodContext = (recentAdminMessages->length > 0) ? any(std:("Last admin message: "") + (OR((lastAdminMessage->content->text), (std:("N/A")))) + std:(""")) (std:("No recent admin messages"));
        return object{
            object::pair{std:("text"), std:("[ADMIN_CHAT_HISTORY]\
Recent conversation with admin user (") + adminMessages->length + std:(" total messages):\
\
") + conversationHistory + std:("\
\
") + adminMoodContext + std:("\
[/ADMIN_CHAT_HISTORY]")}, 
            object::pair{std:("data"), object{
                object::pair{std:("adminConfigured"), true}, 
                object::pair{std:("messageCount"), adminMessages->length}, 
                object::pair{std:("adminUserId"), std:("adminUserId")}, 
                object::pair{std:("recentMessageCount"), recentAdminMessages->length}, 
                object::pair{std:("lastAdminMessage"), OR((lastAdminMessage->content->text), (string_empty))}, 
                object::pair{std:("conversationActive"), adminMessages->some([=](auto m) mutable
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
