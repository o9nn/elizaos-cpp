#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_REALTIME_WEBSOCKET_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_REALTIME_WEBSOCKET_H
#include "core.hpp"
#include "ws.hpp"
using WebSocketServer = Server;
#include "http.hpp"
using HTTPServer = Server;
#include "../monitoring/logger.h"
#include "ioredis.hpp"

class RealtimeService;

extern any pubClient;
extern any subClient;
class RealtimeService : public object, public std::enable_shared_from_this<RealtimeService> {
public:
    using std::enable_shared_from_this<RealtimeService>::shared_from_this;
    std::shared_ptr<WebSocketServer> wss;

    std::shared_ptr<Map<string, std::shared_ptr<Set<std::shared_ptr<WebSocket>>>>> clients;

    RealtimeService(std::shared_ptr<HTTPServer> server);
    virtual void setupWebSocket();
    virtual void setupRedisSubscriber();
    virtual void broadcastToProject(string projectId, any data);
    static void publishEvent(string projectId, string eventType, any data);
};

#endif
