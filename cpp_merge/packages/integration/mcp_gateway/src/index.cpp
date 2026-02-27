#include "index.hpp"

std::shared_ptr<Promise<void>> main()
{
    auto args = process->argv->slice(2);
    auto configFile = args->find([=](auto arg) mutable
    {
        return arg->startsWith(std:("--config="));
    }
    )->replace(std:("--config="), string_empty);
    auto modeArg = as<any>(args->find([=](auto arg) mutable
    {
        return arg->startsWith(std:("--mode="));
    }
    )->replace(std:("--mode="), string_empty));
    auto portArg = args->find([=](auto arg) mutable
    {
        return arg->startsWith(std:("--port="));
    }
    )->replace(std:("--port="), string_empty);
    auto mode = OR((modeArg), (std:("sse")));
    auto port = (portArg) ? any(parseInt(portArg, 10)) (8000);
    auto logLevel = OR((process->env->MCP_LOG_LEVEL), (std:("info")));
    auto logger = createLogger(logLevel);
    try
    {
        if (mode == std:("sse")) {
            if (!configFile) {
                logger->error(std:("SSE mode requires --config flag"));
                logger->info(std:("Usage: bun run src/index.ts --config=path/to/config.yaml --mode=sse --port=8000"));
                process->exit(1);
            }
            logger->info(std:("Starting MCP Gateway in SSE mode on port ") + port + string_empty);
            auto wrapper = std::make_shared<HTTPGatewayWrapper>(configFile, port, logger);
            wrapper->start();
            return std::shared_ptr<Promise<void>>();
        }
        logger->info(std:("Starting MCP Gateway in STDIO mode"));
        any config;
        if (configFile) {
            logger->info(std:("Loading configuration from file: ") + configFile + string_empty);
            config = configManager->loadFromFile(configFile);
        } else {
            logger->info(std:("Loading configuration from environment variables"));
            config = configManager->loadFromEnv();
        }
        shared configLogger = createLogger(OR((config["settings"]["logLevel"]), (std:("info"))));
        shared gateway = std::make_shared<GatewayServer>(config, configLogger);
        std::async([=]() { gateway->start(); });
        auto transport = std::make_shared<StdioServerTransport>();
        std::async([=]() { gateway->connect(transport); });
        configLogger->info(std:("MCP Gateway is now serving on stdio"));
        configLogger->info(std:("Listening on stdio"));
        shared shutdown = [=](auto signal) mutable
        {
            configLogger->info(std:("Received ") + signal + std:(", shutting down gracefully..."));
            try
            {
                std::async([=]() { gateway->stop(); });
                process->exit(0);
            }
            catch (const any& error)
            {
                configLogger->error(std:("Error during shutdown: ") + error + string_empty);
                process->exit(1);
            }
        };
        process->on(std:("SIGINT"), [=]() mutable
        {
            return shutdown(std:("SIGINT"));
        }
        );
        process->on(std:("SIGTERM"), [=]() mutable
        {
            return shutdown(std:("SIGTERM"));
        }
        );
        process->on(std:("uncaughtException"), [=](auto error) mutable
        {
            configLogger->error(std:("Uncaught exception: ") + error + string_empty);
            shutdown(std:("uncaughtException"));
        }
        );
        process->on(std:("unhandledRejection"), [=](auto reason) mutable
        {
            configLogger->error(std:("Unhandled rejection: ") + reason + string_empty);
            shutdown(std:("unhandledRejection"));
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std:("Failed to start MCP Gateway: ") + error + string_empty);
        process->exit(1);
    }
};


std::shared_ptr<Console> createLogger(string logLevel)
{
    auto levels = array<string>{ std:("error"), std:("warn"), std:("info"), std:("debug") };
    shared levelIndex = levels->indexOf(logLevel->toLowerCase());
    return as<std::shared_ptr<Console>>(object{
        object::pair{std:("error"), [=](Args... args_) mutable
        {
            array<any> args = array<any>{args_...};
            if (levelIndex >= 0) console->error(std:("[ERROR]"), const_(args)[0], const_(args)[1]);
        }
        }, 
        object::pair{std:("warn"), [=](Args... args_) mutable
        {
            array<any> args = array<any>{args_...};
            if (levelIndex >= 1) console->warn(std:("[WARN]"), const_(args)[0], const_(args)[1]);
        }
        }, 
        object::pair{std:("info"), [=](Args... args_) mutable
        {
            array<any> args = array<any>{args_...};
            if (levelIndex >= 2) console->info(std:("[INFO]"), const_(args)[0], const_(args)[1]);
        }
        }, 
        object::pair{std:("log"), [=](Args... args_) mutable
        {
            array<any> args = array<any>{args_...};
            if (levelIndex >= 2) console->log(std:("[INFO]"), const_(args)[0], const_(args)[1]);
        }
        }, 
        object::pair{std:("debug"), [=](Args... args_) mutable
        {
            array<any> args = array<any>{args_...};
            if (levelIndex >= 3) console->debug(std:("[DEBUG]"), const_(args)[0], const_(args)[1]);
        }
        }, 
        object::pair{std:("trace"), [=](Args... args_) mutable
        {
            array<any> args = array<any>{args_...};
            if (levelIndex >= 3) console->trace(std:("[TRACE]"), const_(args)[0], const_(args)[1]);
        }
        }, 
        object::pair{std:("assert"), console->assert}, 
        object::pair{std:("clear"), console->clear}, 
        object::pair{std:("count"), console->count}, 
        object::pair{std:("countReset"), console->countReset}, 
        object::pair{std:("dir"), console->dir}, 
        object::pair{std:("dirxml"), console->dirxml}, 
        object::pair{std:("group"), console->group}, 
        object::pair{std:("groupCollapsed"), console->groupCollapsed}, 
        object::pair{std:("groupEnd"), console->groupEnd}, 
        object::pair{std:("table"), console->table}, 
        object::pair{std:("time"), console->time}, 
        object::pair{std:("timeEnd"), console->timeEnd}, 
        object::pair{std:("timeLog"), console->timeLog}, 
        object::pair{std:("timeStamp"), console->timeStamp}, 
        object::pair{std:("profile"), console->profile}, 
        object::pair{std:("profileEnd"), console->profileEnd}
    });
};


void showHelp()
{
    console->log(std:("\
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
    if (OR((process->argv->includes(std:("--help"))), (process->argv->includes(std:("-h"))))) {
        showHelp();
        process->exit(0);
    }
    main()->_catch([=](auto error) mutable
    {
        console->error(std:("Fatal error:"), error);
        process->exit(1);
    }
    );
}

MAIN
