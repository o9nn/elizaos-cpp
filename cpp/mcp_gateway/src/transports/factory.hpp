#include ".types/index.hpp"
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



class TransportFactory {
  static create(config: McpServerConfig): Transport {
    const transportConfig = this.normalizeTransportConfig(config);

    switch (transportConfig.type) {
      case 'stdio': {
        const stdioParams = {
          command: transportConfig.command,
          args: transportConfig.args || [],
          env: {
            ...process.env,
            ...(transportConfig.env || {})
          } as Record<string, string>,
          ...(transportConfig.cwd ? { cwd: transportConfig.cwd } : {})
        } as { command: string; args?: string[]; env?: Record<string, string>; cwd?: string };
        return new StdioClientTransport(stdioParams) as unknown as Transport;
      }
      case 'http':
        return new StreamableHTTPClientTransport(
          new URL(transportConfig.url)
        ) as unknown as Transport;
      case 'sse':
        return new SSEClientTransport(
          new URL(transportConfig.sseUrl)
        ) as unknown as Transport;
      case 'websocket':
        return new WebSocketClientTransport(
          new URL(transportConfig.url)
        ) as unknown as Transport;
      default:
        throw new Error(`Unsupported transport type: ${(transportConfig as { type: string }).type}`);
    }
  }


} // namespace elizaos
