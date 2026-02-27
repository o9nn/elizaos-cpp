#include "websocket-client.h"

WebSocketClient::WebSocketClient() {
}

std::shared_ptr<Promise<void>> WebSocketClient::emit(string room, string event, any data)
{
    auto redis = std::async([=]() { getGlobalRedisCache(); });
    auto formattedRoom = (room == std:("global")) ? any(std:("global")) ((room->startsWith(std:("token-"))) ? room : std:("token-") + room + string_empty);
    auto message = JSON->stringify(object{
        object::pair{std:("room"), formattedRoom}, 
        object::pair{std:("event"), std:("event")}, 
        object::pair{std:("data"), std:("data")}
    });
    std::async([=]() { redis->publish(std:("ws:broadcast"), message); });
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> WebSocketClient::emitToClient(string clientId, string event, any data)
{
    auto redis = std::async([=]() { getGlobalRedisCache(); });
    auto message = JSON->stringify(object{
        object::pair{std:("clientId"), std:("clientId")}, 
        object::pair{std:("event"), std:("event")}, 
        object::pair{std:("data"), std:("data")}
    });
    std::async([=]() { redis->publish(std:("ws:direct"), message); });
    return std::shared_ptr<Promise<void>>();
}

any WebSocketClient::to(string room)
{
    return object{
        object::pair{std:("emit"), [=](auto event, auto data) mutable
        {
            return this->emit(room, event, data);
        }
        }
    };
}

std::shared_ptr<WebSocketClient> getWebSocketClient()
{
    if (!wsClient) {
        wsClient = std::make_shared<WebSocketClient>();
    }
    return wsClient;
};


any wsClient = nullptr;

void Main(void)
{
}

MAIN
