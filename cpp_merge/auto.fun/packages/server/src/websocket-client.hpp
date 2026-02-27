#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_WEBSOCKET_CLIENT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_WEBSOCKET_CLIENT_H
#include "core.hpp"
#include "./redis.h"

class WebSocketClient;

class WebSocketClient : public object, public std::enable_shared_from_this<WebSocketClient> {
public:
    using std::enable_shared_from_this<WebSocketClient>::shared_from_this;
    WebSocketClient();
    virtual std::shared_ptr<Promise<void>> emit(string room, string event, any data);
    virtual std::shared_ptr<Promise<void>> emitToClient(string clientId, string event, any data);
    virtual any to(string room);
};

extern any wsClient;
std::shared_ptr<WebSocketClient> getWebSocketClient();

#endif
