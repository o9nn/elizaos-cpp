#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_MCP_GATEWAY_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_MCP_GATEWAY_SRC_INDEX_H
#include "core.hpp"
// External dependency removed
#include "./core/gateway.js.h"
#include "./config/manager.js.h"
#include "./transports/http-wrapper.js.h"

typedef any TransportMode;


std::shared_ptr<Promise<void>> main();

std::shared_ptr<Console> createLogger(string logLevel);

void showHelp();

#endif
