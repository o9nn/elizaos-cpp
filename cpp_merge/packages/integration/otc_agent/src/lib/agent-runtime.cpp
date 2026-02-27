#include "agent-runtime.h"

std::shared_ptr<AgentRuntimeManager> AgentRuntimeManager::instance;

AgentRuntimeManager::AgentRuntimeManager() {
    if (elizaLogger) {
        elizaLogger->log = console->log->bind(console);
        elizaLogger->info = console->info->bind(console);
        elizaLogger->warn = console->warn->bind(console);
        elizaLogger->error = console->error->bind(console);
        elizaLogger->debug = console->debug->bind(console);
        elizaLogger->success = [=](P0 msg) mutable
        {
            return console->log(std:("✓ ") + (type_of(msg) == std:("string")) ? any(msg) (JSON->stringify(msg)) + string_empty);
        };
        auto logger = as<any>(elizaLogger);
        logger["notice"] = console->info->bind(console);
    }
    if (!globalState->logger) {
        globalState->logger = object{
            object::pair{std:("log"), console->log->bind(console)}, 
            object::pair{std:("info"), console->info->bind(console)}, 
            object::pair{std:("warn"), console->warn->bind(console)}, 
            object::pair{std:("error"), console->error->bind(console)}, 
            object::pair{std:("debug"), console->debug->bind(console)}
        };
    }
    if (!globalState->__elizaManagerLogged) {
        globalState->__elizaManagerLogged = true;
    }
}

std::shared_ptr<AgentRuntimeManager> AgentRuntimeManager::getInstance()
{
    if (!AgentRuntimeManager::instance) {
        AgentRuntimeManager::instance = std::make_shared<AgentRuntimeManager>();
    }
    return AgentRuntimeManager::instance;
}

boolean AgentRuntimeManager::isReady()
{
    return true;
}

std::shared_ptr<Promise<std::shared_ptr<AgentRuntime>>> AgentRuntimeManager::getRuntime()
{
    if (this->runtime) {
        return this->runtime;
    }
    if (globalState->__elizaRuntime) {
        this->runtime = globalState->__elizaRuntime;
        return this->runtime;
    }
    if (this->initializationPromise) {
        return this->initializationPromise;
    }
    this->initializationPromise = this->createRuntime();
    try
    {
        auto runtime = std::async([=]() { this->initializationPromise; });
        return runtime;
    }
    catch (const any& error)
    {
        this->initializationPromise = nullptr;
        throw any(error);
    }
}

std::shared_ptr<Promise<std::shared_ptr<AgentRuntime>>> AgentRuntimeManager::createRuntime()
{
    console->log(std:("[AgentRuntime] Creating runtime instance"));
    auto dbPort = OR((OR((process->env->POSTGRES_DEV_PORT), (process->env->VENDOR_OTC_DESK_DB_PORT))), (5439));
    auto DEFAULT_POSTGRES_URL = std:("postgres://eliza:password@localhost:") + dbPort + std:("/eliza");
    auto dbEnvVars = object{
        object::pair{std:("DATABASE_POSTGRES_URL"), !!process->env->DATABASE_POSTGRES_URL}, 
        object::pair{std:("DATABASE_URL_UNPOOLED"), !!process->env->DATABASE_URL_UNPOOLED}, 
        object::pair{std:("POSTGRES_URL"), !!process->env->POSTGRES_URL}, 
        object::pair{std:("POSTGRES_DATABASE_URL"), !!process->env->POSTGRES_DATABASE_URL}
    };
    console->log(std:("[AgentRuntime] Database env vars present:"), dbEnvVars);
    auto postgresUrl = OR((OR((OR((OR((process->env->DATABASE_POSTGRES_URL), (process->env->DATABASE_URL_UNPOOLED))), (process->env->POSTGRES_URL))), (process->env->POSTGRES_DATABASE_URL))), (DEFAULT_POSTGRES_URL));
    if (OR((!postgresUrl), (postgresUrl == DEFAULT_POSTGRES_URL))) {
        auto isProduction = process->env->NODE_ENV == std:("production");
        if (AND((isProduction), (postgresUrl == DEFAULT_POSTGRES_URL))) {
            console->error(std:("[AgentRuntime] ERROR: No database URL found in production!"));
            console->error(std:("[AgentRuntime] Expected one of: DATABASE_POSTGRES_URL, DATABASE_URL_UNPOOLED, POSTGRES_URL, POSTGRES_DATABASE_URL"));
            throw any(std::make_shared<Error>(std:("Database connection failed: No database URL configured in production. ") + std:("Vercel Neon Storage should provide DATABASE_POSTGRES_URL automatically. ") + std:("Please check your Vercel project settings.")));
        }
    }
    if (AND((postgresUrl), (!postgresUrl->includes(std:("localhost"))))) {
        auto isValidFormat = OR((postgresUrl->startsWith(std:("postgres://"))), (postgresUrl->startsWith(std:("postgresql://"))));
        if (!isValidFormat) {
            console->warn(std:("[AgentRuntime] WARNING: Database URL doesn't start with postgres:// or postgresql://"));
        }
        auto url = std::make_shared<URL>(postgresUrl->replace((new RegExp(std:("^postgres(ql)?:\/\"))), std:("http://")));
        if (OR((!url->hostname), (url->hostname == string_empty))) {
            throw any(std::make_shared<Error>(std:("Database connection failed: Invalid database URL format (missing hostname)")));
        }
    }
    console->log(std:("[AgentRuntime] Database config: ") + (postgresUrl->includes(std:("localhost"))) ? any(std:("localhost:") + dbPort) (std:("remote (Vercel/Neon)")) + string_empty);
    auto RUNTIME_AGENT_ID = as<std::shared_ptr<UUID>>(std:("b850bc30-45f8-0041-a00a-83df46d8555d"));
    this->runtime = std::make_shared<AgentRuntime>(as<any>(utils::assign(object{
        , 
        object::pair{std:("agentId"), RUNTIME_AGENT_ID}, 
        object::pair{std:("settings"), utils::assign(object{
            object::pair{std:("GROQ_API_KEY"), OR((process->env->GROQ_API_KEY), (string_empty))}, 
            object::pair{std:("SMALL_GROQ_MODEL"), OR((process->env->SMALL_GROQ_MODEL), (std:("qwen/qwen3-32b")))}, 
            object::pair{std:("LARGE_GROQ_MODEL"), OR((process->env->LARGE_GROQ_MODEL), (std:("moonshotai/kimi-k2-instruct-0905")))}, 
            object::pair{std:("POSTGRES_URL"), postgresUrl}
        }, agent["character"]->settings)}
    }, agent)));
    globalState->__elizaRuntime = this->runtime;
    if (OR((!this->runtime["logger"]), (!this->runtime["logger"]["log"]))) {
        this->runtime["logger"] = as<any>(object{
            object::pair{std:("log"), console->log->bind(console)}, 
            object::pair{std:("info"), console->info->bind(console)}, 
            object::pair{std:("warn"), console->warn->bind(console)}, 
            object::pair{std:("error"), console->error->bind(console)}, 
            object::pair{std:("debug"), console->debug->bind(console)}, 
            object::pair{std:("success"), [=](auto message) mutable
            {
                return console->log(std:("✓ ") + message + string_empty);
            }
            }, 
            object::pair{std:("notice"), console->info->bind(console)}
        });
    }
    std::async([=]() { this->ensureBuiltInTables(); });
    std::async([=]() { this->runtime["initialize"](); });
    auto services = Array->from(this->runtime["getAllServices"]()["keys"]());
    console->log(std:("[AgentRuntime] Registered services:"), services);
    return this->runtime;
}

std::shared_ptr<Promise<void>> AgentRuntimeManager::ensureBuiltInTables()
{
    if (OR((this->hasRunMigrations), (globalState->__elizaMigrationsRan))) return std::shared_ptr<Promise<void>>();
    this->hasRunMigrations = true;
    globalState->__elizaMigrationsRan = true;
    console->log(std:("[AgentRuntime] Using Eliza cache system for quote storage"));
}

std::shared_ptr<Promise<std::shared_ptr<Memory>>> AgentRuntimeManager::handleMessage(string roomId, string entityId, object content)
{
    auto runtime = std::async([=]() { this->getRuntime(); });
    auto entityUuid = as<std::shared_ptr<UUID>>(stringToUuid(entityId));
    std::async([=]() { runtime->ensureConnection(as<any>(object{
        object::pair{std:("entityId"), entityUuid}, 
        object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(roomId)}, 
        object::pair{std:("worldId"), stringToUuid(std:("otc-desk-world"))}, 
        object::pair{std:("source"), std:("web")}, 
        object::pair{std:("type"), ChannelType->DM}, 
        object::pair{std:("channelId"), roomId}, 
        object::pair{std:("serverId"), std:("otc-desk-server")}, 
        object::pair{std:("userName"), entityId}
    })); });
    auto userMessage = object{
        object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(roomId)}, 
        object::pair{std:("entityId"), entityUuid}, 
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(runtime->agentId)}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), OR((content["text"]), (string_empty))}, 
            object::pair{std:("attachments"), OR((content["attachments"]), (array<any>()))}
        }}
    };
    std::async([=]() { runtime->emitEvent(EventType->MESSAGE_RECEIVED, object{
        object::pair{std:("runtime"), std:("runtime")}, 
        object::pair{std:("message"), object{
            object::pair{std:("id"), userMessage->id}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), OR((content["text"]), (string_empty))}, 
                object::pair{std:("attachments"), OR((content["attachments"]), (array<any>()))}
            }}, 
            object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(stringToUuid(entityId))}, 
            object::pair{std:("agentId"), runtime->agentId}, 
            object::pair{std:("roomId"), roomId}, 
            object::pair{std:("createdAt"), Date->now()}
        }}, 
        object::pair{std:("callback"), [=]() mutable
        {
            console->log(std:("[AgentRuntime] Message handler completed"));
        }
        }
    }); });
    return userMessage;
}

std::shared_ptr<GlobalElizaState> globalState = as<std::shared_ptr<GlobalElizaState>>(as<any>(globalThis));
std::shared_ptr<AgentRuntimeManager> agentRuntime = AgentRuntimeManager::getInstance();

void Main(void)
{
    if (type_of(globalState->__elizaMigrationsRan) == std:("undefined")) globalState->__elizaMigrationsRan = false;
    if (type_of(globalState->__elizaManagerLogged) == std:("undefined")) globalState->__elizaManagerLogged = false;
}

MAIN
