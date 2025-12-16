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

;

const StdioTransportSchema = z.object({
  type: z.literal('stdio'),
  command: z.string(),
  args: z.array(z.string()).optional(),
  env: z.record(z.string()).optional(),
  cwd: z.string().optional()
});

const HttpTransportSchema = z.object({
  type: z.literal('http'),
  url: z.string().url(),
  headers: z.record(z.string()).optional(),
  apiKey: z.string().optional()
});

const SseTransportSchema = z.object({
  type: z.literal('sse'),
  sseUrl: z.string().url(),
  postUrl: z.string().url(),
  headers: z.record(z.string()).optional(),
  apiKey: z.string().optional()
});

const WebSocketTransportSchema = z.object({
  type: z.literal('websocket'),
  url: z.string().url(),
  headers: z.record(z.string()).optional(),
  apiKey: z.string().optional()
});

const TransportConfigSchema = z.discriminatedUnion('type', [
  StdioTransportSchema,
  HttpTransportSchema,
  SseTransportSchema,
  WebSocketTransportSchema
]);

// Payment configuration schemas
const ToolPricingSchema = z.object({
  free: z.boolean().optional(),
  x402: z.string().optional(),
  apiKeyTiers: z.record(z.string()).optional()
});

const ToolConfigSchema = z.object({
  name: z.string(),
  pricing: ToolPricingSchema.optional()
});

const ApiKeyConfigSchema = z.object({
  key: z.string(),
  tier: z.string(),
  rateLimit: z.number().optional()
});

const PaymentConfigSchema = z.object({
  enabled: z.boolean().default(false),
  recipient: z.string().optional(),
  network: z.string().default('base-sepolia'),
  facilitator: z.string().default('https://x402.org/facilitator'),
  apiKeys: z.array(ApiKeyConfigSchema).optional(),
  // Outbound payment config (for paying downstream x402 APIs)
  outboundWallet: z.string().optional() // Private key for paying downstream
});

const McpServerConfigSchema = z.object({
  name: z.string(),
  transport: TransportConfigSchema.optional(),
  command: z.string().optional(),
  args: z.array(z.string()).optional(),
  env: z.record(z.string()).optional(),
  cwd: z.string().optional(),
  namespace: z.string().optional(),
  enabled: z.boolean().default(true),
  timeout: z.number().default(30000),
  retryAttempts: z.number().default(3),
  retryDelay: z.number().default(1000),
  tools: z.array(ToolConfigSchema).optional(),
  defaultPricing: ToolPricingSchema.optional(),
  // Outbound payment configuration
  paymentMode: z.enum(['passthrough', 'markup', 'absorb']).optional(),
  markup: z.string().optional(), // e.g., "20%" or "$0.01"
  paymentWallet: z.string().optional() // Override wallet for this server
});

const GatewayConfigSchema = z.object({
  name: z.string().default('MCP Gateway'),
  version: z.string().default('1.0.0'),
  description: z.string().optional(),
  servers: z.array(McpServerConfigSchema),
  payment: PaymentConfigSchema.optional(),
  settings: z.object({
    enableToolConflictResolution: z.boolean().default(true),
    enableResourceConflictResolution: z.boolean().default(true),
    enablePromptConflictResolution: z.boolean().default(true),
    logLevel: z.enum(['error', 'warn', 'info', 'debug']).default('info'),
    maxConcurrentConnections: z.number().default(10),
    healthCheckInterval: z.number().default(60000)
  }).optional().default({})
});

using TransportConfig = z.infer<typeof TransportConfigSchema>;
using McpServerConfig = z.infer<typeof McpServerConfigSchema>;
using GatewayConfig = z.infer<typeof GatewayConfigSchema>;
using ToolPricing = z.infer<typeof ToolPricingSchema>;
using ToolConfig = z.infer<typeof ToolConfigSchema>;
using ApiKeyConfig = z.infer<typeof ApiKeyConfigSchema>;
using PaymentConfig = z.infer<typeof PaymentConfigSchema>;

import type { Client } from '@modelcontextprotocol/sdk/client/index.js';

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
