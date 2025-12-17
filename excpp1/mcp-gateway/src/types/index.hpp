#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Payment configuration schemas

  // Outbound payment config (for paying downstream x402 APIs)

  // Outbound payment configuration

using TransportConfig = z::infer<typeof TransportConfigSchema>;
using McpServerConfig = z::infer<typeof McpServerConfigSchema>;
using GatewayConfig = z::infer<typeof GatewayConfigSchema>;
using ToolPricing = z::infer<typeof ToolPricingSchema>;
using ToolConfig = z::infer<typeof ToolConfigSchema>;
using ApiKeyConfig = z::infer<typeof ApiKeyConfigSchema>;
using PaymentConfig = z::infer<typeof PaymentConfigSchema>;

struct ServerConnection {
    McpServerConfig config;
    std::optional<Client> client;
    std::variant<'connecting', 'connected', 'disconnected', 'error'> status;
    std::optional<Error> lastError;
    std::optional<Date> lastHealthCheck;
    std::optional<bool> tools;
    std::optional<bool> resources;
    std::optional<bool> prompts;
};

struct AggregatedTool {
    std::string name;
    std::string originalName;
    std::string serverId;
    std::optional<std::string> namespace;
    std::optional<std::string> description;
    object inputSchema;
};

struct AggregatedResource {
    std::string uri;
    std::string originalUri;
    std::string serverId;
    std::optional<std::string> namespace;
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<std::string> mimeType;
};

struct AggregatedPrompt {
    std::string name;
    std::string originalName;
    std::string serverId;
    std::optional<std::string> namespace;
    std::optional<std::string> description;
    std::string name;
    std::optional<std::string> description;
    std::optional<bool> required;
};


} // namespace elizaos
