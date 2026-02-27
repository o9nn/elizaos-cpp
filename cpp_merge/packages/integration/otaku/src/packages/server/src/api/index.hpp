#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_API_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_API_INDEX_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "cors.hpp"
// Using alias removed (invalid transpilation)
#include "express.hpp"
// Using alias removed (invalid transpilation)
#include "helmet.hpp"
// Using alias removed (invalid transpilation)
#include "node:http.h"
// Using alias removed (invalid transpilation)
#include "path-to-regexp.h"
#include "socket.io.h"
using SocketIOServer = Server;
#include "../index.h"
#include "./agents.h"
#include "./messaging.h"
#include "./media.h"
#include "./memory.h"
#include "./audio.h"
#include "./runtime.h"
#include "./tee.h"
#include "./system.h"
#include "./entities.h"
#include "./cdp.h"
#include "./auth.h"
#include "../socketio.h"
#include "../middleware.h"

std::shared_ptr<SocketIOServer> setupSocketIO(std::shared_ptr<http::Server> server, std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance);

void setupLogStreaming(std::shared_ptr<SocketIOServer> io, std::shared_ptr<SocketIORouter> router);

std::shared_ptr<express::RequestHandler> createPluginRouteHandler(std::shared_ptr<ElizaOS> elizaOS);

std::shared_ptr<express::Router> createApiRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance);

#endif
