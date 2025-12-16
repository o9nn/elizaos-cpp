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

;
;
;
;
import type { Transport } from '@modelcontextprotocol/sdk/shared/transport.js';
;

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

  private static normalizeTransportConfig(config: McpServerConfig): TransportConfig {
    if (config.transport) {
      return config.transport;
    }

    if (config.command) {
      return {
        type: 'stdio' as const,
        command: config.command,
        args: config.args,
        env: config.env,
        cwd: config.cwd
      };
    }

    throw new Error(`Server ${config.name} has no transport configuration`);
  }

  static validateConfig(config: McpServerConfig): string[] {
    const errors: string[] = [];
    
    try {
      const transportConfig = this.normalizeTransportConfig(config);
      
      switch (transportConfig.type) {
        case 'stdio':
          if (!transportConfig.command) {
            errors.push(`STDIO transport requires 'command' field`);
          }
          break;
        case 'http':
          if (!transportConfig.url) {
            errors.push(`HTTP transport requires 'url' field`);
          }
          break;
        case 'sse':
          if (!transportConfig.sseUrl || !transportConfig.postUrl) {
            errors.push(`SSE transport requires both 'sseUrl' and 'postUrl' fields`);
          }
          break;
        case 'websocket':
          if (!transportConfig.url) {
            errors.push(`WebSocket transport requires 'url' field`);
          }
          break;
      }
      
    } catch (error) {
      errors.push(`Transport configuration error: ${error}`);
    }
    
    return errors;
  }

  static getTransportDisplayName(config: McpServerConfig): string {
    try {
      const transportConfig = this.normalizeTransportConfig(config);
      switch (transportConfig.type) {
        case 'stdio':
          return `STDIO (${transportConfig.command})`;
        case 'http':
          return `HTTP (${transportConfig.url})`;
        case 'sse':
          return `SSE (${transportConfig.sseUrl})`;
        case 'websocket':
          return `WebSocket (${transportConfig.url})`;
        default:
          return 'Unknown';
      }
    } catch {
      return 'Invalid';
    }
  }
}



} // namespace elizaos
