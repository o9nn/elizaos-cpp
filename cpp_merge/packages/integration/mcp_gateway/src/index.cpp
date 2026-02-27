#include "index.hpp"
#include <string>

std::shared_ptr<Promise<void>> main()
{
    auto args = process->argv->slice(2);
    auto configFile = args->find([=](auto arg) mutable
    {
        return arg->startsWith(std::string("--config="));
    }
    )->replace(std::string("--config="), string_empty);
    auto modeArg = as<any>(args->find([=](auto arg) mutable
    {
        return arg->startsWith(std::string("--mode="));
    }
    )->replace(std::string("--mode="), string_empty));
    auto portArg = args->find([=](auto arg) mutable
    {
        return arg->startsWith(std::string("--port="));
    }
    )->replace(std::string("--port="), string_empty);
    auto mode = OR((modeArg), (std::string("sse")));
    auto port = (portArg) ? any(parseInt(portArg, 10)) (8000);
    auto logLevel = OR((process->env->MCP_LOG_LEVEL), (std::string("info")));
    auto logger = createLogger(logLevel);
    try
    {
        if (mode == std::string("sse")) {
            if (!configFile) {
                logger->error(std::string("SSE mode requires --config flag"));
                logger->info(std::string("Usage: bun run src/index.ts --config=path/to/config.yaml --mode=sse --port=8000"));
                process->exit(1);
            }
            logger->info(std::string("Starting MCP Gateway in SSE mode on port ") + port + string_empty);
            auto wrapper = std::make_shared<HTTPGatewayWrapper>(configFile, port, logger);
            wrapper->start();
            return std::shared_ptr<Promise<void>>();
        }
        logger->info(std::string("Starting MCP Gateway in STDIO mode"));
        any config;
        if (configFile) {
            logger->info(std::string("Loading configuration from file: ") + configFile + string_empty);
            config = configManager->loadFromFile(configFile);
        } else {
            logger->info(std::string("Loading configuration from environment variables"));
            config = configManager->loadFromEnv();
        }
        shared configLogger = createLogger(OR((config["settings"]["logLevel"]), (std::string("info"))));
        shared gateway = std::make_shared<GatewayServer>(config, configLogger);
        std::async([=]() { gateway->start(); });
        auto transport = std::make_shared<StdioServerTransport>();
        std::async([=]() { gateway->connect(transport); });
        configLogger->info(std::string("MCP Gateway is now serving on stdio"));
        configLogger->info(std::string("Listening on stdio"));
        shared shutdown = [=](auto signal) mutable
        {
            configLogger->info(std::string("Received ") + signal + std::string(", shutting down gracefully..."));
            try
            {
                std::async([=]() { gateway->stop(); });
                process->exit(0);
            }
            catch (const any& error)
            {
                configLogger->error(std::string("Error during shutdown: ") + error + string_empty);
                process->exit(1);
            }
        };
        process->on(std::string("SIGINT"), [=]() mutable
        {
            return shutdown(std::string("SIGINT"));
        }
        );
        process->on(std::string("SIGTERM"), [=]() mutable
        {
            return shutdown(std::string("SIGTERM"));
        }
        );
        process->on(std::string("uncaughtException"), [=](auto error) mutable
        {
            configLogger->error(std::string("Uncaught exception: ") + error + string_empty);
            shutdown(std::string("uncaughtException"));
        }
        );
        process->on(std::string("unhandledRejection"), [=](auto reason) mutable
        {
            configLogger->error(std::string("Unhandled rejection: ") + reason + string_empty);
            shutdown(std::string("unhandledRejection"));
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std::string("Failed to start MCP Gateway: ") + error + string_empty);
        process->exit(1);
    }
};


std::shared_ptr<Console> createLogger(string logLevel)
{
    auto levels = array<string>{ std::string("error"), std::string("warn"), std::string("info"), std::string("debug") };
    shared levelIndex = levels->indexOf(logLevel->toLowerCase());
    return as<std::shared_ptr<Console>>(object{
        object::pair{std::string("error"), [=](Args... args_) mutable
        {
            array<any> args = array<any>{args_...};
            if (levelIndex >= 0) console->error(std::string("[ERROR]"), const_(args)[0], const_(args)[1]);
        }
        }, 
        object::pair{std::string("warn"), [=](Args... args_) mutable
        {
            array<any> args = array<any>{args_...};
            if (levelIndex >= 1) console->warn(std::string("[WARN]"), const_(args)[0], const_(args)[1]);
        }
        }, 
        object::pair{std::string("info"), [=](Args... args_) mutable
        {
            array<any> args = array<any>{args_...};
            if (levelIndex >= 2) console->info(std::string("[INFO]"), const_(args)[0], const_(args)[1]);
        }
        }, 
        object::pair{std::string("log"), [=](Args... args_) mutable
        {
            array<any> args = array<any>{args_...};
            if (levelIndex >= 2) console->log(std::string("[INFO]"), const_(args)[0], const_(args)[1]);
        }
        }, 
        object::pair{std::string("debug"), [=](Args... args_) mutable
        {
            array<any> args = array<any>{args_...};
            if (levelIndex >= 3) console->debug(std::string("[DEBUG]"), const_(args)[0], const_(args)[1]);
        }
        }, 
        object::pair{std::string("trace"), [=](Args... args_) mutable
        {
            array<any> args = array<any>{args_...};
            if (levelIndex >= 3) console->trace(std::string("[TRACE]"), const_(args)[0], const_(args)[1]);
        }
        }, 
        object::pair{std::string("assert"), console->assert}, 
        object::pair{std::string("clear"), console->clear}, 
        object::pair{std::string("count"), console->count}, 
        object::pair{std::string("countReset"), console->countReset}, 
        object::pair{std::string("dir"), console->dir}, 
        object::pair{std::string("dirxml"), console->dirxml}, 
        object::pair{std::string("group"), console->group}, 
        object::pair{std::string("groupCollapsed"), console->groupCollapsed}, 
        object::pair{std::string("groupEnd"), console->groupEnd}, 
        object::pair{std::string("table"), console->table}, 
        object::pair{std::string("time"), console->time}, 
        object::pair{std::string("timeEnd"), console->timeEnd}, 
        object::pair{std::string("timeLog"), console->timeLog}, 
        object::pair{std::string("timeStamp"), console->timeStamp}, 
        object::pair{std::string("profile"), console->profile}, 
        object::pair{std::string("profileEnd"), console->profileEnd}
    });
};


void showHelp()
{
    console->log(std::string("\
MCP Gateway Server\
\
USAGE:\
  mcp-gateway [OPTIONS]\
\
OPTIONS:\
  --config=<path>    Path to configuration file (JSON or YAML)\
  --mode=<mode>      Transport mode: sse (default) or stdio\
  --port=<port>      Port for SSE mode (default: 8000)\
  --help            Show this help message\
\
TRANSPORT MODES:\
  sse               HTTP/SSE server mode - exposes gateway over HTTP with x402 payment support\
                    Endpoints: GET /sse (Server-Sent Events), POST /message\
  stdio             Standard I/O mode - communicates via stdin/stdout (used by Claude Desktop)\
\
ENVIRONMENT VARIABLES:\
  MCP_GATEWAY_NAME                       Name of the gateway (default: "MCP Gateway")\
  MCP_GATEWAY_VERSION                    Version of the gateway (default: "1.0.0")\
  MCP_GATEWAY_DESCRIPTION                Description of the gateway\
  MCP_SERVERS                           Semicolon-separated server specs (name:command:args)\
  MCP_LOG_LEVEL                         Log level: error, warn, info, debug (default: info)\
  MCP_ENABLE_TOOL_CONFLICT_RESOLUTION   Enable tool name conflict resolution (default: true)\
  MCP_ENABLE_RESOURCE_CONFLICT_RESOLUTION Enable resource conflict resolution (default: true)\
  MCP_ENABLE_PROMPT_CONFLICT_RESOLUTION Enable prompt conflict resolution (default: true)\
  MCP_MAX_CONCURRENT_CONNECTIONS        Maximum concurrent connections (default: 10)\
  MCP_HEALTH_CHECK_INTERVAL             Health check interval in ms (default: 60000)\
\
EXAMPLES:\
  # Run in SSE mode (default) - HTTP server with x402 payments\
  mcp-gateway --config=config.yaml --port=8000\
  \
  # Explicitly specify SSE mode\
  mcp-gateway --config=config.yaml --mode=sse --port=8000\
\
  # Run in STDIO mode (for Claude Desktop integration)\
  mcp-gateway --config=config.yaml --mode=stdio\
\
  # Run with environment variables (STDIO mode)\
  MCP_SERVERS="weather:node:weather.js;filesystem:python:fs_server.py" mcp-gateway --mode=stdio\
\
For more information, visit: https://github.com/studio/mcp-gateway\
"));
};



void Main(void)
{
    if (OR((process->argv->includes(std::string("--help"))), (process->argv->includes(std::string("-h"))))) {
        showHelp();
        process->exit(0);
    }
    main()->_catch([=](auto error) mutable
    {
        console->error(std::string("Fatal error:"), error);
        process->exit(1);
    }
    );
}

MAIN
