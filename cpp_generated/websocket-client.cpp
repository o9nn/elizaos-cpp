#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/websocket-client.h"

WebSocketClient::WebSocketClient() {
}

std::shared_ptr<Promise<void>> WebSocketClient::emit(string room, string event, any data)
{
    auto redis = std::async([=]() { getGlobalRedisCache(); });
    auto formattedRoom = (room == std::string("global")) ? any(std::string("global")) : any((room->startsWith(std::string("token-"))) ? room : std::string("token-") + room + string_empty);
    auto message = JSON->stringify(object{
        object::pair{std::string("room"), formattedRoom}, 
        object::pair{std::string("event"), std::string("event")}, 
        object::pair{std::string("data"), std::string("data")}
    });
    std::async([=]() { redis->publish(std::string("ws:broadcast"), message); });
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> WebSocketClient::emitToClient(string clientId, string event, any data)
{
    auto redis = std::async([=]() { getGlobalRedisCache(); });
    auto message = JSON->stringify(object{
        object::pair{std::string("clientId"), std::string("clientId")}, 
        object::pair{std::string("event"), std::string("event")}, 
        object::pair{std::string("data"), std::string("data")}
    });
    std::async([=]() { redis->publish(std::string("ws:direct"), message); });
    return std::shared_ptr<Promise<void>>();
}

any WebSocketClient::to(string room)
{
    return object{
        object::pair{std::string("emit"), [=](auto event, auto data) mutable
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
