#include "index.hpp"

any StdioTransportSchema = z->object(object{
    object::pair{std:("type"), z->literal(std:("stdio"))}, 
    object::pair{std:("command"), z->string()}, 
    object::pair{std:("args"), z->array(z->string())->optional()}, 
    object::pair{std:("env"), z->record(z->string())->optional()}, 
    object::pair{std:("cwd"), z->string()->optional()}
});
any HttpTransportSchema = z->object(object{
    object::pair{std:("type"), z->literal(std:("http"))}, 
    object::pair{std:("url"), z->string()->url()}, 
    object::pair{std:("headers"), z->record(z->string())->optional()}, 
    object::pair{std:("apiKey"), z->string()->optional()}
});
any SseTransportSchema = z->object(object{
    object::pair{std:("type"), z->literal(std:("sse"))}, 
    object::pair{std:("sseUrl"), z->string()->url()}, 
    object::pair{std:("postUrl"), z->string()->url()}, 
    object::pair{std:("headers"), z->record(z->string())->optional()}, 
    object::pair{std:("apiKey"), z->string()->optional()}
});
any WebSocketTransportSchema = z->object(object{
    object::pair{std:("type"), z->literal(std:("websocket"))}, 
    object::pair{std:("url"), z->string()->url()}, 
    object::pair{std:("headers"), z->record(z->string())->optional()}, 
    object::pair{std:("apiKey"), z->string()->optional()}
});
any TransportConfigSchema = z->discriminatedUnion(std:("type"), array<any>{ StdioTransportSchema, HttpTransportSchema, SseTransportSchema, WebSocketTransportSchema });
any ToolPricingSchema = z->object(object{
    object::pair{std:("free"), z->boolean()->optional()}, 
    object::pair{std:("x402"), z->string()->optional()}, 
    object::pair{std:("apiKeyTiers"), z->record(z->string())->optional()}
});
any ToolConfigSchema = z->object(object{
    object::pair{std:("name"), z->string()}, 
    object::pair{std:("pricing"), ToolPricingSchema->optional()}
});
any ApiKeyConfigSchema = z->object(object{
    object::pair{std:("key"), z->string()}, 
    object::pair{std:("tier"), z->string()}, 
    object::pair{std:("rateLimit"), z->number()->optional()}
});
any PaymentConfigSchema = z->object(object{
    object::pair{std:("enabled"), z->boolean()->default(false)}, 
    object::pair{std:("recipient"), z->string()->optional()}, 
    object::pair{std:("network"), z->string()->default(std:("base-sepolia"))}, 
    object::pair{std:("facilitator"), z->string()->default(std:("https://x402.org/facilitator"))}, 
    object::pair{std:("apiKeys"), z->array(ApiKeyConfigSchema)->optional()}, 
    object::pair{std:("outboundWallet"), z->string()->optional()}
});
any McpServerConfigSchema = z->object(object{
    object::pair{std:("name"), z->string()}, 
    object::pair{std:("transport"), TransportConfigSchema->optional()}, 
    object::pair{std:("command"), z->string()->optional()}, 
    object::pair{std:("args"), z->array(z->string())->optional()}, 
    object::pair{std:("env"), z->record(z->string())->optional()}, 
    object::pair{std:("cwd"), z->string()->optional()}, 
    object::pair{std:("namespace"), z->string()->optional()}, 
    object::pair{std:("enabled"), z->boolean()->default(true)}, 
    object::pair{std:("timeout"), z->number()->default(30000)}, 
    object::pair{std:("retryAttempts"), z->number()->default(3)}, 
    object::pair{std:("retryDelay"), z->number()->default(1000)}, 
    object::pair{std:("tools"), z->array(ToolConfigSchema)->optional()}, 
    object::pair{std:("defaultPricing"), ToolPricingSchema->optional()}, 
    object::pair{std:("paymentMode"), z->enum(array<string>{ std:("passthrough"), std:("markup"), std:("absorb") })->optional()}, 
    object::pair{std:("markup"), z->string()->optional()}, 
    object::pair{std:("paymentWallet"), z->string()->optional()}
});
any GatewayConfigSchema = z->object(object{
    object::pair{std:("name"), z->string()->default(std:("MCP Gateway"))}, 
    object::pair{std:("version"), z->string()->default(std:("1.0.0"))}, 
    object::pair{std:("description"), z->string()->optional()}, 
    object::pair{std:("servers"), z->array(McpServerConfigSchema)}, 
    object::pair{std:("payment"), PaymentConfigSchema->optional()}, 
    object::pair{std:("settings"), z->object(object{
        object::pair{std:("enableToolConflictResolution"), z->boolean()->default(true)}, 
        object::pair{std:("enableResourceConflictResolution"), z->boolean()->default(true)}, 
        object::pair{std:("enablePromptConflictResolution"), z->boolean()->default(true)}, 
        object::pair{std:("logLevel"), z->enum(array<string>{ std:("error"), std:("warn"), std:("info"), std:("debug") })->default(std:("info"))}, 
        object::pair{std:("maxConcurrentConnections"), z->number()->default(10)}, 
        object::pair{std:("healthCheckInterval"), z->number()->default(60000)}
    })->optional()->default(object{})}
});

void Main(void)
{
}

MAIN
