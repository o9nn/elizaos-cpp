#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_MCP-GATEWAY_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_MCP-GATEWAY_SRC_INDEX_H
#include "core.h"
#include "@modelcontextprotocol/sdk/server/stdio.js.h"
#include "./core/gateway.js.h"
#include "./config/manager.js.h"
#include "./transports/http-wrapper.js.h"

typedef any TransportMode;


std::shared_ptr<Promise<void>> main();

std::shared_ptr<Console> createLogger(string logLevel);

void showHelp();

#endif
