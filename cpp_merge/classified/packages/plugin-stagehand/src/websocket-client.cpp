#include "websocket-client.h"
#include <string>

StagehandWebSocketClient::StagehandWebSocketClient(string serverUrl_) : serverUrl(serverUrl_)  {
}

std::shared_ptr<Promise<void>> StagehandWebSocketClient::connect()
{
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        try
        {
            this->ws = std::make_shared<(as<any>(WebSocket))>(this->serverUrl);
            this->ws["on"](std::string("open"), [=]() mutable
            {
                this->connected = true;
                this->reconnectAttempts = 0;
                logger->info(std::string("[Stagehand] Connected to server at ") + this->serverUrl + string_empty);
                resolve();
            }
            );
            this->ws["on"](std::string("message"), [=](auto data) mutable
            {
                try
                {
                    auto message = as<std::shared_ptr<StagehandMessage>>(JSON->parse(data["toString"]()));
                    if (AND((message->requestId), (this->messageHandlers->has(message->requestId)))) {
                        auto handler = this->messageHandlers->get(message->requestId);
                        this->messageHandlers->delete(message->requestId);
                        handler(message);
                    }
                    if (message->type == std::string("connected")) {
                        logger->info(std::string("[Stagehand] Server connected: ") + JSON->stringify(message) + string_empty);
                    }
                }
                catch (const any& error)
                {
                    logger->error(std::string("[Stagehand] Error parsing message:"), error);
                }
            }
            );
            this->ws["on"](std::string("error"), [=](auto error) mutable
            {
                logger->error(std::string("[Stagehand] WebSocket error:"), error);
                if (!this->connected) {
                    reject(error);
                }
            }
            );
            this->ws["on"](std::string("close"), [=]() mutable
            {
                this->connected = false;
                logger->info(std::string("[Stagehand] Disconnected from server"));
                if (AND((this->ws), (this->reconnectAttempts < this->maxReconnectAttempts))) {
                    this->attemptReconnect();
                }
            }
            );
        }
        catch (const any& error)
        {
            reject(error);
        }
    }
    );
}

std::shared_ptr<Promise<void>> StagehandWebSocketClient::attemptReconnect()
{
    this->reconnectAttempts++;
    logger->info(std::string("[Stagehand] Attempting reconnection ") + this->reconnectAttempts + std::string("/") + this->maxReconnectAttempts + std::string("..."));
    std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
    {
        return setTimeout(resolve, this->reconnectDelay * this->reconnectAttempts);
    }
    ); });
    try
    {
        std::async([=]() { this->connect(); });
    }
    catch (const any& error)
    {
        logger->error(std::string("[Stagehand] Reconnection failed:"), error);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::sendMessage(string type, any data)
{
    if (OR((!this->ws), (!this->connected))) {
        throw any(std::make_shared<Error>(std::string("Not connected to Stagehand server")));
    }
    shared requestId = std::string("req-") + Date->now() + std::string("-") + Math->random()->toString(36)->substring(7) + string_empty;
    shared message = utils::assign(object{
        object::pair{std::string("type"), std::string("type")}, 
        object::pair{std::string("requestId"), std::string("requestId")}
    }, data);
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        shared timeout = setTimeout([=]() mutable
        {
            this->messageHandlers->delete(requestId);
            reject(std::make_shared<Error>(std::string("Request timeout for ") + type + string_empty));
        }
        , 30000);
        this->messageHandlers->set(requestId, [=](auto response) mutable
        {
            clearTimeout(timeout);
            if (response->type == std::string("error")) {
                reject(std::make_shared<Error>(OR((response->error), (std::string("Unknown error")))));
            } else {
                resolve(response);
            }
        }
        );
        this->ws["send"](JSON->stringify(message));
        logger->debug(std::string("[Stagehand] Sent message: ") + type + std::string(" (") + requestId + std::string(")"));
    }
    );
}

void StagehandWebSocketClient::disconnect()
{
    this->reconnectAttempts = this->maxReconnectAttempts;
    if (this->ws) {
        this->ws["close"]();
        this->ws = nullptr;
    }
    this->connected = false;
    logger->info(std::string("[Stagehand] Client disconnected"));
}

boolean StagehandWebSocketClient::isConnected()
{
    return this->connected;
}

std::shared_ptr<Promise<object>> StagehandWebSocketClient::navigate(string sessionId, string url)
{
    auto response = std::async([=]() { this->sendMessage(std::string("navigate"), object{
        object::pair{std::string("sessionId"), std::string("sessionId")}, 
        object::pair{std::string("data"), object{
            object::pair{std::string("url"), std::string("url")}
        }}
    }); });
    return OR((response->data), (object{
        object::pair{std::string("url"), std::string("url")}, 
        object::pair{std::string("title"), string_empty}
    }));
}

std::shared_ptr<Promise<object>> StagehandWebSocketClient::getState(string sessionId)
{
    auto response = std::async([=]() { this->sendMessage(std::string("getState"), object{
        object::pair{std::string("sessionId"), std::string("sessionId")}
    }); });
    return (OR((response->data), (object{
        object::pair{std::string("url"), string_empty}, 
        object::pair{std::string("title"), string_empty}, 
        object::pair{std::string("sessionId"), std::string("sessionId")}, 
        object::pair{std::string("createdAt"), std::make_shared<Date>()}
    })));
}

std::shared_ptr<Promise<object>> StagehandWebSocketClient::goBack(string sessionId)
{
    auto response = std::async([=]() { this->sendMessage(std::string("goBack"), object{
        object::pair{std::string("sessionId"), std::string("sessionId")}
    }); });
    return OR((response->data), (object{
        object::pair{std::string("url"), string_empty}, 
        object::pair{std::string("title"), string_empty}
    }));
}

std::shared_ptr<Promise<object>> StagehandWebSocketClient::goForward(string sessionId)
{
    auto response = std::async([=]() { this->sendMessage(std::string("goForward"), object{
        object::pair{std::string("sessionId"), std::string("sessionId")}
    }); });
    return OR((response->data), (object{
        object::pair{std::string("url"), string_empty}, 
        object::pair{std::string("title"), string_empty}
    }));
}

std::shared_ptr<Promise<object>> StagehandWebSocketClient::refresh(string sessionId)
{
    auto response = std::async([=]() { this->sendMessage(std::string("refresh"), object{
        object::pair{std::string("sessionId"), std::string("sessionId")}
    }); });
    return OR((response->data), (object{
        object::pair{std::string("url"), string_empty}, 
        object::pair{std::string("title"), string_empty}
    }));
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::click(string sessionId, string description)
{
    return std::async([=]() { this->sendMessage(std::string("click"), object{
        object::pair{std::string("sessionId"), std::string("sessionId")}, 
        object::pair{std::string("data"), object{
            object::pair{std::string("description"), std::string("description")}
        }}
    }); });
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::type(string sessionId, string text, string field)
{
    return std::async([=]() { this->sendMessage(std::string("type"), object{
        object::pair{std::string("sessionId"), std::string("sessionId")}, 
        object::pair{std::string("data"), object{
            object::pair{std::string("text"), std::string("text")}, 
            object::pair{std::string("field"), std::string("field")}
        }}
    }); });
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::select(string sessionId, string option, string dropdown)
{
    return std::async([=]() { this->sendMessage(std::string("select"), object{
        object::pair{std::string("sessionId"), std::string("sessionId")}, 
        object::pair{std::string("data"), object{
            object::pair{std::string("option"), std::string("option")}, 
            object::pair{std::string("dropdown"), std::string("dropdown")}
        }}
    }); });
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::extract(string sessionId, string instruction)
{
    return std::async([=]() { this->sendMessage(std::string("extract"), object{
        object::pair{std::string("sessionId"), std::string("sessionId")}, 
        object::pair{std::string("data"), object{
            object::pair{std::string("instruction"), std::string("instruction")}
        }}
    }); });
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::screenshot(string sessionId)
{
    return std::async([=]() { this->sendMessage(std::string("screenshot"), object{
        object::pair{std::string("sessionId"), std::string("sessionId")}
    }); });
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::solveCaptcha(string sessionId)
{
    return std::async([=]() { this->sendMessage(std::string("solveCaptcha"), object{
        object::pair{std::string("sessionId"), std::string("sessionId")}
    }); });
}

std::shared_ptr<Promise<boolean>> StagehandWebSocketClient::health()
{
    try
    {
        auto response = std::async([=]() { this->sendMessage(std::string("health"), object{}); });
        return AND((response->type == std::string("health")), (response->data["status"] == std::string("ok")));
    }
    catch (const any& error)
    {
        logger->error(std::string("[Stagehand] Health check failed:"), error);
        return false;
    }
}

any WebSocket;

void Main(void)
{
    if (AND((type_of(window) != std::string("std::nullopt")), (type_of(window->WebSocket) != std::string("std::nullopt")))) {
        WebSocket = as<any>(window->WebSocket);
    } else {
        WebSocket = require(std::string("ws"));
    }
}

MAIN
