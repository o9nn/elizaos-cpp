#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
    std::string status;
    std::optional<std::runtime_error> lastError;
    std::optional<std::chrono::system_clock::time_point> lastHealthCheck;
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
