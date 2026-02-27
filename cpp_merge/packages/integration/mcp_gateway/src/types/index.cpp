#include "index.hpp"
#include <string>

any StdioTransportSchema = z->object(object{
    object::pair{std::string("type"), z->literal(std::string("stdio"))}, 
    object::pair{std::string("command"), z->string()}, 
    object::pair{std::string("args"), z->array(z->string())->optional()}, 
    object::pair{std::string("env"), z->record(z->string())->optional()}, 
    object::pair{std::string("cwd"), z->string()->optional()}
});
any HttpTransportSchema = z->object(object{
    object::pair{std::string("type"), z->literal(std::string("http"))}, 
    object::pair{std::string("url"), z->string()->url()}, 
    object::pair{std::string("headers"), z->record(z->string())->optional()}, 
    object::pair{std::string("apiKey"), z->string()->optional()}
});
any SseTransportSchema = z->object(object{
    object::pair{std::string("type"), z->literal(std::string("sse"))}, 
    object::pair{std::string("sseUrl"), z->string()->url()}, 
    object::pair{std::string("postUrl"), z->string()->url()}, 
    object::pair{std::string("headers"), z->record(z->string())->optional()}, 
    object::pair{std::string("apiKey"), z->string()->optional()}
});
any WebSocketTransportSchema = z->object(object{
    object::pair{std::string("type"), z->literal(std::string("websocket"))}, 
    object::pair{std::string("url"), z->string()->url()}, 
    object::pair{std::string("headers"), z->record(z->string())->optional()}, 
    object::pair{std::string("apiKey"), z->string()->optional()}
});
any TransportConfigSchema = z->discriminatedUnion(std::string("type"), array<any>{ StdioTransportSchema, HttpTransportSchema, SseTransportSchema, WebSocketTransportSchema });
any ToolPricingSchema = z->object(object{
    object::pair{std::string("free"), z->boolean()->optional()}, 
    object::pair{std::string("x402"), z->string()->optional()}, 
    object::pair{std::string("apiKeyTiers"), z->record(z->string())->optional()}
});
any ToolConfigSchema = z->object(object{
    object::pair{std::string("name"), z->string()}, 
    object::pair{std::string("pricing"), ToolPricingSchema->optional()}
});
any ApiKeyConfigSchema = z->object(object{
    object::pair{std::string("key"), z->string()}, 
    object::pair{std::string("tier"), z->string()}, 
    object::pair{std::string("rateLimit"), z->number()->optional()}
});
any PaymentConfigSchema = z->object(object{
    object::pair{std::string("enabled"), z->boolean()->default(false)}, 
    object::pair{std::string("recipient"), z->string()->optional()}, 
    object::pair{std::string("network"), z->string()->default(std::string("base-sepolia"))}, 
    object::pair{std::string("facilitator"), z->string()->default(std::string("https://x402.org/facilitator"))}, 
    object::pair{std::string("apiKeys"), z->array(ApiKeyConfigSchema)->optional()}, 
    object::pair{std::string("outboundWallet"), z->string()->optional()}
});
any McpServerConfigSchema = z->object(object{
    object::pair{std::string("name"), z->string()}, 
    object::pair{std::string("transport"), TransportConfigSchema->optional()}, 
    object::pair{std::string("command"), z->string()->optional()}, 
    object::pair{std::string("args"), z->array(z->string())->optional()}, 
    object::pair{std::string("env"), z->record(z->string())->optional()}, 
    object::pair{std::string("cwd"), z->string()->optional()}, 
    object::pair{std::string("namespace"), z->string()->optional()}, 
    object::pair{std::string("enabled"), z->boolean()->default(true)}, 
    object::pair{std::string("timeout"), z->number()->default(30000)}, 
    object::pair{std::string("retryAttempts"), z->number()->default(3)}, 
    object::pair{std::string("retryDelay"), z->number()->default(1000)}, 
    object::pair{std::string("tools"), z->array(ToolConfigSchema)->optional()}, 
    object::pair{std::string("defaultPricing"), ToolPricingSchema->optional()}, 
    object::pair{std::string("paymentMode"), z->enum(array<string>{ std::string("passthrough"), std::string("markup"), std::string("absorb") })->optional()}, 
    object::pair{std::string("markup"), z->string()->optional()}, 
    object::pair{std::string("paymentWallet"), z->string()->optional()}
});
any GatewayConfigSchema = z->object(object{
    object::pair{std::string("name"), z->string()->default(std::string("MCP Gateway"))}, 
    object::pair{std::string("version"), z->string()->default(std::string("1.0.0"))}, 
    object::pair{std::string("description"), z->string()->optional()}, 
    object::pair{std::string("servers"), z->array(McpServerConfigSchema)}, 
    object::pair{std::string("payment"), PaymentConfigSchema->optional()}, 
    object::pair{std::string("settings"), z->object(object{
        object::pair{std::string("enableToolConflictResolution"), z->boolean()->default(true)}, 
        object::pair{std::string("enableResourceConflictResolution"), z->boolean()->default(true)}, 
        object::pair{std::string("enablePromptConflictResolution"), z->boolean()->default(true)}, 
        object::pair{std::string("logLevel"), z->enum(array<string>{ std::string("error"), std::string("warn"), std::string("info"), std::string("debug") })->default(std::string("info"))}, 
        object::pair{std::string("maxConcurrentConnections"), z->number()->default(10)}, 
        object::pair{std::string("healthCheckInterval"), z->number()->default(60000)}
    })->optional()->default(object{})}
});

void Main(void)
{
}

MAIN
