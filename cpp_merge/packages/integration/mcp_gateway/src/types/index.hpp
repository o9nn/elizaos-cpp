#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_MCP_GATEWAY_SRC_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_MCP_GATEWAY_SRC_TYPES_INDEX_H
#include "core.hpp"
#include "zod.hpp"
// External dependency removed

typedef z::infer<TransportConfigSchema> TransportConfig;

typedef z::infer<McpServerConfigSchema> McpServerConfig;

typedef z::infer<GatewayConfigSchema> GatewayConfig;

typedef z::infer<ToolPricingSchema> ToolPricing;

typedef z::infer<ToolConfigSchema> ToolConfig;

typedef z::infer<ApiKeyConfigSchema> ApiKeyConfig;

typedef z::infer<PaymentConfigSchema> PaymentConfig;

class ServerConnection;
class AggregatedTool;
class AggregatedResource;
class AggregatedPrompt;

extern any StdioTransportSchema;
extern any HttpTransportSchema;
extern any SseTransportSchema;
extern any WebSocketTransportSchema;
extern any TransportConfigSchema;
extern any ToolPricingSchema;
extern any ToolConfigSchema;
extern any ApiKeyConfigSchema;
extern any PaymentConfigSchema;
extern any McpServerConfigSchema;
extern any GatewayConfigSchema;
class ServerConnection : public object, public std::enable_shared_from_this<ServerConnection> {
public:
    using std::enable_shared_from_this<ServerConnection>::shared_from_this;
    McpServerConfig config;

    any client;

    any status;

    std::shared_ptr<Error> lastError;

    std::shared_ptr<Date> lastHealthCheck;

    object capabilities;
};

class AggregatedTool : public object, public std::enable_shared_from_this<AggregatedTool> {
public:
    using std::enable_shared_from_this<AggregatedTool>::shared_from_this;
    string name;

    string originalName;

    string serverId;

    any namespace;

    any description;

    any inputSchema;
};

class AggregatedResource : public object, public std::enable_shared_from_this<AggregatedResource> {
public:
    using std::enable_shared_from_this<AggregatedResource>::shared_from_this;
    string uri;

    string originalUri;

    string serverId;

    any namespace;

    any name;

    any description;

    any mimeType;
};

class AggregatedPrompt : public object, public std::enable_shared_from_this<AggregatedPrompt> {
public:
    using std::enable_shared_from_this<AggregatedPrompt>::shared_from_this;
    string name;

    string originalName;

    string serverId;

    any namespace;

    any description;

    any arguments;
};

#endif
