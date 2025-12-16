#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Payment configuration schemas

  // Outbound payment config (for paying downstream x402 APIs)

  // Outbound payment configuration

using TransportConfig = z.infer<typeof TransportConfigSchema>;
using McpServerConfig = z.infer<typeof McpServerConfigSchema>;
using GatewayConfig = z.infer<typeof GatewayConfigSchema>;
using ToolPricing = z.infer<typeof ToolPricingSchema>;
using ToolConfig = z.infer<typeof ToolConfigSchema>;
using ApiKeyConfig = z.infer<typeof ApiKeyConfigSchema>;
using PaymentConfig = z.infer<typeof PaymentConfigSchema>;

struct ServerConnection {
    McpServerConfig config;
    Client | null client;
    'connecting' | 'connected' | 'disconnected' | 'error' status;
    std::optional<Error> lastError;
    std::optional<Date> lastHealthCheck;
    std::optional<{> capabilities;
    std::optional<bool> tools;
    std::optional<bool> resources;
    std::optional<bool> prompts;
};

struct AggregatedTool {
    std::string name;
    std::string originalName;
    std::string serverId;
    std::optional<string | undefined> namespace;
    std::optional<string | undefined> description;
    object inputSchema;
};

struct AggregatedResource {
    std::string uri;
    std::string originalUri;
    std::string serverId;
    std::optional<string | undefined> namespace;
    std::optional<string | undefined> name;
    std::optional<string | undefined> description;
    std::optional<string | undefined> mimeType;
};

struct AggregatedPrompt {
    std::string name;
    std::string originalName;
    std::string serverId;
    std::optional<string | undefined> namespace;
    std::optional<string | undefined> description;
    std::optional<Array<{> arguments;
    std::string name;
    std::optional<string | undefined> description;
    std::optional<boolean | undefined> required;
};


} // namespace elizaos
