#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto args = process.argv.slice(2);

    // Parse command line arguments
    const auto configFile = args.find(arg => arg.startsWith("--config=")).replace("--config=", "");
    const auto modeArg = args.find(arg => arg.startsWith("--mode=")).replace("--mode=", "") | std::nullopt;
    const auto portArg = args.find(arg => arg.startsWith("--port=")).replace("--port=", "");

    // Default to SSE mode
    const TransportMode mode = modeArg || "sse";
    const auto port = portArg ? parseInt(portArg, 10) : 8000;

    // Setup logging based on config (will be overridden by config later)
    const auto logLevel = process.env.MCP_LOG_LEVEL || "info";
    const auto logger = createLogger(logLevel);

    try {
        // SSE mode - start HTTP/SSE wrapper
        if (mode == 'sse') {
            if (!configFile) {
                std::cerr << "SSE mode requires --config flag" << std::endl;
                std::cout << "Usage: bun run src/index.ts --config=path/to/config.yaml --mode=sse --port=8000" << std::endl;
                process.exit(1);
            }

            std::cout << "Starting MCP Gateway in SSE mode on port " + port << std::endl;
            const auto wrapper = new HTTPGatewayWrapper(configFile, port, logger);
            wrapper.start();
            return;
        }

        // STDIO mode - run gateway directly
        std::cout << "Starting MCP Gateway in STDIO mode" << std::endl;

        // Load configuration
        auto config;
        if (configFile) {
            std::cout << "Loading configuration from file: " + configFile << std::endl;
            config = configManager.loadFromFile(configFile);
            } else {
                std::cout << "Loading configuration from environment variables" << std::endl;
                config = configManager.loadFromEnv();
            }

            // Update logger with config log level
            const auto configLogger = createLogger(config.settings.logLevel || "info");

            // Create and start the gateway server
            const auto gateway = new GatewayServer(config, configLogger);
            gateway.start();

            // Create transport and connect
            const auto transport = new StdioServerTransport();
            gateway.connect(transport);

            configLogger.info("MCP Gateway is now serving on stdio");
            // Compatibility message for older tests looking for this exact phrase
            configLogger.info("Listening on stdio");

            // Handle graceful shutdown
            const auto shutdown = async (signal: string) => {;
                "configLogger.info(" + "Received " + signal + ", shutting down gracefully...";
                try {
                    gateway.stop();
                    process.exit(0);
                    } catch (error) {
                        "configLogger.error(" + "Error during shutdown: " + error
                        process.exit(1);
                    }
                    };

                    process.on("SIGINT", () => shutdown("SIGINT"));
                    process.on("SIGTERM", () => shutdown("SIGTERM"));
                    process.on("uncaughtException", (error) => {
                        "configLogger.error(" + "Uncaught exception: " + error
                        shutdown("uncaughtException");
                        });
                        process.on("unhandledRejection", (reason) => {
                            "configLogger.error(" + "Unhandled rejection: " + reason
                            shutdown("unhandledRejection");
                            });

                            } catch (error) {
                                std::cerr << "Failed to start MCP Gateway: " + error << std::endl;
                                process.exit(1);
                            }

}

Console createLogger(const std::string& logLevel) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto levels = ["error", "warn", "info", "debug"];
    const auto levelIndex = levels.indexOf(logLevel.toLowerCase());

    return {
        error: (...args: unknown[]) => {
            if (levelIndex >= 0) console.error('[ERROR]', ...args);
            },
            warn: (...args: unknown[]) => {
                if (levelIndex >= 1) console.warn('[WARN]', ...args);
                },
                info: (...args: unknown[]) => {
                    if (levelIndex >= 2) console.info('[INFO]', ...args);
                    },
                    log: (...args: unknown[]) => {
                        if (levelIndex >= 2) console.log('[INFO]', ...args);
                        },
                        debug: (...args: unknown[]) => {
                            if (levelIndex >= 3) console.debug('[DEBUG]', ...args);
                            },
                            trace: (...args: unknown[]) => {
                                if (levelIndex >= 3) console.trace('[TRACE]', ...args);
                                },
                                // Add other console methods to satisfy the Console interface
                                assert: console.assert,
                                clear: console.clear,
                                count: console.count,
                                countReset: console.countReset,
                                dir: console.dir,
                                dirxml: console.dirxml,
                                group: console.group,
                                groupCollapsed: console.groupCollapsed,
                                groupEnd: console.groupEnd,
                                table: console.table,
                                time: console.time,
                                timeEnd: console.timeEnd,
                                timeLog: console.timeLog,
                                timeStamp: console.timeStamp,
                                profile: console.profile,
                                profileEnd: console.profileEnd
                                }

}

void showHelp() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    console.log(`
    MCP Gateway Server;

    USAGE:
    mcp-gateway [OPTIONS];

    OPTIONS:
    --config=<path>    Path to configuration file (JSON or YAML);
    --mode=<mode>      Transport mode: sse (default) or stdio
    --port=<port>      Port for SSE mode (default: 8000)
    --help            Show this help message;

    TRANSPORT MODES:
    sse               HTTP/SSE server mode - exposes gateway over HTTP with x402 payment support;
    Endpoints: GET /sse (Server-Sent Events), POST /message
    stdio             Standard I/O mode - communicates via stdin/stdout (used by Claude Desktop);

    ENVIRONMENT VARIABLES:
    MCP_GATEWAY_NAME                       Name of the gateway (default: "MCP Gateway")
    MCP_GATEWAY_VERSION                    Version of the gateway (default: "1.0.0")
    MCP_GATEWAY_DESCRIPTION                Description of the gateway;
    MCP_SERVERS                           Semicolon-separated server specs (name:command:args)
    MCP_LOG_LEVEL                         Log level: error, warn, info, debug (default: info)
    MCP_ENABLE_TOOL_CONFLICT_RESOLUTION   Enable tool name conflict resolution (default: true)
    MCP_ENABLE_RESOURCE_CONFLICT_RESOLUTION Enable resource conflict resolution (default: true)
    MCP_ENABLE_PROMPT_CONFLICT_RESOLUTION Enable prompt conflict resolution (default: true)
    MCP_MAX_CONCURRENT_CONNECTIONS        Maximum concurrent connections (default: 10)
    MCP_HEALTH_CHECK_INTERVAL             Health check interval in ms (default: 60000)

    EXAMPLES:
    # Run in SSE mode (default) - HTTP server with x402 payments;
    mcp-gateway --config=config.yaml --port=8000;

    # Explicitly specify SSE mode;
    mcp-gateway --config=config.yaml --mode=sse --port=8000;

    # Run in STDIO mode (for Claude Desktop integration);
    mcp-gateway --config=config.yaml --mode=stdio;

    # Run with environment variables (STDIO mode);
    MCP_SERVERS="weather:node:weather.js;filesystem:python:fs_server.py" mcp-gateway --mode=stdio

    For more information, visit: https://github.com/studio/mcp-gateway
    `);

}

} // namespace elizaos
