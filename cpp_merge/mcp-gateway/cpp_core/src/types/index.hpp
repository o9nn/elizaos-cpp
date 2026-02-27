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
    std: status;
    std::optional<std::runtime_error> lastError;
    std::optional<std::chrono::system_clock::time_point> lastHealthCheck;
    std::optional<bool> tools;
    std::optional<bool> resources;
    std::optional<bool> prompts;
};

struct AggregatedTool {
    std: name;
    std: originalName;
    std: serverId;
    std::optional<std:> namespace;
    std::optional<std:> description;
    object inputSchema;
};

struct AggregatedResource {
    std: uri;
    std: originalUri;
    std: serverId;
    std::optional<std:> namespace;
    std::optional<std:> name;
    std::optional<std:> description;
    std::optional<std:> mimeType;
};

struct AggregatedPrompt {
    std: name;
    std: originalName;
    std: serverId;
    std::optional<std:> namespace;
    std::optional<std:> description;
    std: name;
    std::optional<std:> description;
    std::optional<bool> required;
};


} // namespace elizaos
