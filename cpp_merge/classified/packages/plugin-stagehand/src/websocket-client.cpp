#include "websocket-client.h"

StagehandWebSocketClient::StagehandWebSocketClient(string serverUrl_) : serverUrl(serverUrl_)  {
}

std::shared_ptr<Promise<void>> StagehandWebSocketClient::connect()
{
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        try
        {
            this->ws = std::make_shared<(as<any>(WebSocket))>(this->serverUrl);
            this->ws["on"](std:("open"), [=]() mutable
            {
                this->connected = true;
                this->reconnectAttempts = 0;
                logger->info(std:("[Stagehand] Connected to server at ") + this->serverUrl + string_empty);
                resolve();
            }
            );
            this->ws["on"](std:("message"), [=](auto data) mutable
            {
                try
                {
                    auto message = as<std::shared_ptr<StagehandMessage>>(JSON->parse(data["toString"]()));
                    if (AND((message->requestId), (this->messageHandlers->has(message->requestId)))) {
                        auto handler = this->messageHandlers->get(message->requestId);
                        this->messageHandlers->delete(message->requestId);
                        handler(message);
                    }
                    if (message->type == std:("connected")) {
                        logger->info(std:("[Stagehand] Server connected: ") + JSON->stringify(message) + string_empty);
                    }
                }
                catch (const any& error)
                {
                    logger->error(std:("[Stagehand] Error parsing message:"), error);
                }
            }
            );
            this->ws["on"](std:("error"), [=](auto error) mutable
            {
                logger->error(std:("[Stagehand] WebSocket error:"), error);
                if (!this->connected) {
                    reject(error);
                }
            }
            );
            this->ws["on"](std:("close"), [=]() mutable
            {
                this->connected = false;
                logger->info(std:("[Stagehand] Disconnected from server"));
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
    logger->info(std:("[Stagehand] Attempting reconnection ") + this->reconnectAttempts + std:("/") + this->maxReconnectAttempts + std:("..."));
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
        logger->error(std:("[Stagehand] Reconnection failed:"), error);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::sendMessage(string type, any data)
{
    if (OR((!this->ws), (!this->connected))) {
        throw any(std::make_shared<Error>(std:("Not connected to Stagehand server")));
    }
    shared requestId = std:("req-") + Date->now() + std:("-") + Math->random()->toString(36)->substring(7) + string_empty;
    shared message = utils::assign(object{
        object::pair{std:("type"), std:("type")}, 
        object::pair{std:("requestId"), std:("requestId")}
    }, data);
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        shared timeout = setTimeout([=]() mutable
        {
            this->messageHandlers->delete(requestId);
            reject(std::make_shared<Error>(std:("Request timeout for ") + type + string_empty));
        }
        , 30000);
        this->messageHandlers->set(requestId, [=](auto response) mutable
        {
            clearTimeout(timeout);
            if (response->type == std:("error")) {
                reject(std::make_shared<Error>(OR((response->error), (std:("Unknown error")))));
            } else {
                resolve(response);
            }
        }
        );
        this->ws["send"](JSON->stringify(message));
        logger->debug(std:("[Stagehand] Sent message: ") + type + std:(" (") + requestId + std:(")"));
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
    logger->info(std:("[Stagehand] Client disconnected"));
}

boolean StagehandWebSocketClient::isConnected()
{
    return this->connected;
}

std::shared_ptr<Promise<object>> StagehandWebSocketClient::navigate(string sessionId, string url)
{
    auto response = std::async([=]() { this->sendMessage(std:("navigate"), object{
        object::pair{std:("sessionId"), std:("sessionId")}, 
        object::pair{std:("data"), object{
            object::pair{std:("url"), std:("url")}
        }}
    }); });
    return OR((response->data), (object{
        object::pair{std:("url"), std:("url")}, 
        object::pair{std:("title"), string_empty}
    }));
}

std::shared_ptr<Promise<object>> StagehandWebSocketClient::getState(string sessionId)
{
    auto response = std::async([=]() { this->sendMessage(std:("getState"), object{
        object::pair{std:("sessionId"), std:("sessionId")}
    }); });
    return (OR((response->data), (object{
        object::pair{std:("url"), string_empty}, 
        object::pair{std:("title"), string_empty}, 
        object::pair{std:("sessionId"), std:("sessionId")}, 
        object::pair{std:("createdAt"), std::make_shared<Date>()}
    })));
}

std::shared_ptr<Promise<object>> StagehandWebSocketClient::goBack(string sessionId)
{
    auto response = std::async([=]() { this->sendMessage(std:("goBack"), object{
        object::pair{std:("sessionId"), std:("sessionId")}
    }); });
    return OR((response->data), (object{
        object::pair{std:("url"), string_empty}, 
        object::pair{std:("title"), string_empty}
    }));
}

std::shared_ptr<Promise<object>> StagehandWebSocketClient::goForward(string sessionId)
{
    auto response = std::async([=]() { this->sendMessage(std:("goForward"), object{
        object::pair{std:("sessionId"), std:("sessionId")}
    }); });
    return OR((response->data), (object{
        object::pair{std:("url"), string_empty}, 
        object::pair{std:("title"), string_empty}
    }));
}

std::shared_ptr<Promise<object>> StagehandWebSocketClient::refresh(string sessionId)
{
    auto response = std::async([=]() { this->sendMessage(std:("refresh"), object{
        object::pair{std:("sessionId"), std:("sessionId")}
    }); });
    return OR((response->data), (object{
        object::pair{std:("url"), string_empty}, 
        object::pair{std:("title"), string_empty}
    }));
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::click(string sessionId, string description)
{
    return std::async([=]() { this->sendMessage(std:("click"), object{
        object::pair{std:("sessionId"), std:("sessionId")}, 
        object::pair{std:("data"), object{
            object::pair{std:("description"), std:("description")}
        }}
    }); });
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::type(string sessionId, string text, string field)
{
    return std::async([=]() { this->sendMessage(std:("type"), object{
        object::pair{std:("sessionId"), std:("sessionId")}, 
        object::pair{std:("data"), object{
            object::pair{std:("text"), std:("text")}, 
            object::pair{std:("field"), std:("field")}
        }}
    }); });
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::select(string sessionId, string option, string dropdown)
{
    return std::async([=]() { this->sendMessage(std:("select"), object{
        object::pair{std:("sessionId"), std:("sessionId")}, 
        object::pair{std:("data"), object{
            object::pair{std:("option"), std:("option")}, 
            object::pair{std:("dropdown"), std:("dropdown")}
        }}
    }); });
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::extract(string sessionId, string instruction)
{
    return std::async([=]() { this->sendMessage(std:("extract"), object{
        object::pair{std:("sessionId"), std:("sessionId")}, 
        object::pair{std:("data"), object{
            object::pair{std:("instruction"), std:("instruction")}
        }}
    }); });
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::screenshot(string sessionId)
{
    return std::async([=]() { this->sendMessage(std:("screenshot"), object{
        object::pair{std:("sessionId"), std:("sessionId")}
    }); });
}

std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> StagehandWebSocketClient::solveCaptcha(string sessionId)
{
    return std::async([=]() { this->sendMessage(std:("solveCaptcha"), object{
        object::pair{std:("sessionId"), std:("sessionId")}
    }); });
}

std::shared_ptr<Promise<boolean>> StagehandWebSocketClient::health()
{
    try
    {
        auto response = std::async([=]() { this->sendMessage(std:("health"), object{}); });
        return AND((response->type == std:("health")), (response->data["status"] == std:("ok")));
    }
    catch (const any& error)
    {
        logger->error(std:("[Stagehand] Health check failed:"), error);
        return false;
    }
}

any WebSocket;

void Main(void)
{
    if (AND((type_of(window) != std:("undefined")), (type_of(window->WebSocket) != std:("undefined")))) {
        WebSocket = as<any>(window->WebSocket);
    } else {
        WebSocket = require(std:("ws"));
    }
}

MAIN
