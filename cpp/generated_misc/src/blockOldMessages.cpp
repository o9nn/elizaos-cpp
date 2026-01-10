#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/src/utils/blockOldMessages.h"

void blockOldMessages()
{
    console->log(std::string("🛡️ Installing WebSocket message blocker..."));
    shared originalSend = WebSocket["prototype"]->send;
    WebSocket["prototype"]->send = [=](P0 data) mutable
    {
        try
        {
            auto message = nullptr;
            if (type_of(data) == std::string("string")) {
                message = as<std::shared_ptr<WebSocketMessage>>(JSON->parse(data));
            }
            if (AND((message), (message->type == std::string("send_message")))) {
                auto messageText = OR((OR((message->message["text"]), (message->message["content"]))), (string_empty));
                if (messageText->includes(std::string("admin has opened the terminal"))) {
                    console->error(std::string("🚫 BLOCKED problematic message!"), message);
                    console->trace();
                    return any();
                }
            }
        }
        catch (const any& _e)
        {
        }
        return originalSend->apply(shared_from_this(), array<any>{ data });
    };
    console->log(std::string("✅ WebSocket message blocker installed"));
};


