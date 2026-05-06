#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_MCP-GATEWAY_EXAMPLE-AGENTS_AI16Z-PRICE-AGENT-SSE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_MCP-GATEWAY_EXAMPLE-AGENTS_AI16Z-PRICE-AGENT-SSE_H
#include "core.h"
#include "@anthropic-ai/sdk.h"
using Anthropic = _default;
#include "@modelcontextprotocol/sdk/client/index.js.h"
#include "@modelcontextprotocol/sdk/client/sse.js.h"
#include "viem.h"
#include "viem/accounts.h"
#include "x402-fetch.h"

extern string DEFAULT_WALLET_KEY;
extern string GATEWAY_SSE_URL;
extern string GATEWAY_MESSAGE_URL;
std::shared_ptr<Promise<void>> main();

#endif
