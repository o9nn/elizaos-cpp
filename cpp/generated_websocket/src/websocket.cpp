#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/realtime/websocket.h"

RealtimeService::RealtimeService(std::shared_ptr<HTTPServer> server) {
    this->wss = std::make_shared<WebSocketServer>(object{
        object::pair{std::string("server"), std::string("server")}
    });
    this->clients = std::make_shared<Map>();
    this->setupWebSocket();
    this->setupRedisSubscriber();
}

void RealtimeService::setupWebSocket()
{
    this->wss->on(std::string("connection"), [=](auto ws, auto req) mutable
    {
        shared projectId = ((std::make_shared<URL>(req["url"], std::string("http://") + req["headers"]["host"] + string_empty)))->searchParams->get(std::string("projectId"));
        if (projectId) {
            if (!this->clients->has(projectId)) {
                this->clients->set(projectId, std::make_shared<Set>());
            }
            this->clients->get(projectId)->add(ws);
            ws["on"](std::string("close"), [=]() mutable
            {
                this->clients->get(projectId)->delete(ws);
            }
            );
        }
    }
    );
}

void RealtimeService::setupRedisSubscriber()
{
    subClient->subscribe(std::string("project-events"));
    subClient->on(std::string("message"), [=](auto channel, auto message) mutable
    {
        auto event = JSON->parse(message);
        this->broadcastToProject(event["projectId"], event);
    }
    );
}

void RealtimeService::broadcastToProject(string projectId, any data)
{
    auto projectClients = this->clients->get(projectId);
    if (projectClients) {
        projectClients->forEach([=](auto client) mutable
        {
            if (client->readyState == WebSocket["OPEN"]) {
                client->send(JSON->stringify(data));
            }
        }
        );
    }
}

void RealtimeService::publishEvent(string projectId, string eventType, any data)
{
    std::async([=]() { pubClient->publish(std::string("project-events"), JSON->stringify(object{
        object::pair{std::string("projectId"), std::string("projectId")}, 
        object::pair{std::string("type"), eventType}, 
        object::pair{std::string("data"), std::string("data")}, 
        object::pair{std::string("timestamp"), std::make_shared<Date>()}
    })); });
}

any pubClient = std::make_shared<Redis>(process->env->REDIS_URL);
any subClient = std::make_shared<Redis>(process->env->REDIS_URL);

void Main(void)
{
}

MAIN
