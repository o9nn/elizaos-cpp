#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_STAGEHAND_SRC_WEBSOCKET_CLIENT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_STAGEHAND_SRC_WEBSOCKET_CLIENT_H
#include "core.hpp"
// External dependency removed

class StagehandMessage;
class StagehandWebSocketClient;

extern any WebSocket;
class StagehandMessage : public object, public std::enable_shared_from_this<StagehandMessage> {
public:
    using std::enable_shared_from_this<StagehandMessage>::shared_from_this;
    string type;

    string requestId;
};

class StagehandWebSocketClient : public object, public std::enable_shared_from_this<StagehandWebSocketClient> {
public:
    using std::enable_shared_from_this<StagehandWebSocketClient>::shared_from_this;
    string serverUrl;

    any ws = nullptr;

    std::shared_ptr<Map<string, std::function<void(std::shared_ptr<StagehandMessage>)>>> messageHandlers = std::make_shared<Map<string, std::function<void(std::shared_ptr<StagehandMessage>)>>>();

    boolean connected = false;

    double reconnectAttempts = 0;

    double maxReconnectAttempts = 5;

    double reconnectDelay = 1000;

    StagehandWebSocketClient(string serverUrl_);
    virtual std::shared_ptr<Promise<void>> connect();
    virtual std::shared_ptr<Promise<void>> attemptReconnect();
    virtual std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> sendMessage(string type, any data);
    virtual void disconnect();
    virtual boolean isConnected();
    virtual std::shared_ptr<Promise<object>> navigate(string sessionId, string url);
    virtual std::shared_ptr<Promise<object>> getState(string sessionId);
    virtual std::shared_ptr<Promise<object>> goBack(string sessionId);
    virtual std::shared_ptr<Promise<object>> goForward(string sessionId);
    virtual std::shared_ptr<Promise<object>> refresh(string sessionId);
    virtual std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> click(string sessionId, string description);
    virtual std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> type(string sessionId, string text, string field);
    virtual std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> select(string sessionId, string option, string dropdown);
    virtual std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> extract(string sessionId, string instruction);
    virtual std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> screenshot(string sessionId);
    virtual std::shared_ptr<Promise<std::shared_ptr<StagehandMessage>>> solveCaptcha(string sessionId);
    virtual std::shared_ptr<Promise<boolean>> health();
};

#endif
