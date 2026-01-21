#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_WEBSOCKET-CLIENT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_WEBSOCKET-CLIENT_H
#include "core.h"
#include "./redis.h"

class WebSocketClient;

class WebSocketClient : public object, public std::enable_shared_from_this<WebSocketClient> {
public:
    using std::enable_shared_from_this<WebSocketClient>::shared_from_this;
    WebSocketClient();
    virtual std::shared_ptr<Promise<void>> emit(std::string room, std::string event, std::any data);
    virtual std::shared_ptr<Promise<void>> emitToClient(std::string clientId, std::string event, std::any data);
    virtual std::any to(std::string room);
};

extern std::any wsClient;
std::shared_ptr<WebSocketClient> getWebSocketClient();

#endif
