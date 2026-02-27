#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/src/utils/blockOldMessages.h"

void blockOldMessages()
{
    console->log(std:("🛡️ Installing WebSocket message blocker..."));
    shared originalSend = WebSocket["prototype"]->send;
    WebSocket["prototype"]->send = [=](P0 data) mutable
    {
        try
        {
            auto message = nullptr;
            if (type_of(data) == std:("string")) {
                message = as<std::shared_ptr<WebSocketMessage>>(JSON->parse(data));
            }
            if (AND((message), (message->type == std:("send_message")))) {
                auto messageText = OR((OR((message->message["text"]), (message->message["content"]))), (string_empty));
                if (messageText->includes(std:("admin has opened the terminal"))) {
                    console->error(std:("🚫 BLOCKED problematic message!"), message);
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
    console->log(std:("✅ WebSocket message blocker installed"));
};


