#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/lib/agent-runtime.h"

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
            return console->log(std::string("✓ ") + (type_of(msg) == std::string("string")) ? any(msg) : any(JSON->stringify(msg)) + string_empty);
        };
        auto logger = as<any>(elizaLogger);
        logger["notice"] = console->info->bind(console);
    }
    if (!globalState->logger) {
        globalState->logger = object{
            object::pair{std::string("log"), console->log->bind(console)}, 
            object::pair{std::string("info"), console->info->bind(console)}, 
            object::pair{std::string("warn"), console->warn->bind(console)}, 
            object::pair{std::string("error"), console->error->bind(console)}, 
            object::pair{std::string("debug"), console->debug->bind(console)}
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
    console->log(std::string("[AgentRuntime] Creating runtime instance"));
    auto dbPort = OR((OR((process->env->POSTGRES_DEV_PORT), (process->env->VENDOR_OTC_DESK_DB_PORT))), (5439));
    auto DEFAULT_POSTGRES_URL = std::string("postgres://eliza:password@localhost:") + dbPort + std::string("/eliza");
    auto dbEnvVars = object{
        object::pair{std::string("DATABASE_POSTGRES_URL"), !!process->env->DATABASE_POSTGRES_URL}, 
        object::pair{std::string("DATABASE_URL_UNPOOLED"), !!process->env->DATABASE_URL_UNPOOLED}, 
        object::pair{std::string("POSTGRES_URL"), !!process->env->POSTGRES_URL}, 
        object::pair{std::string("POSTGRES_DATABASE_URL"), !!process->env->POSTGRES_DATABASE_URL}
    };
    console->log(std::string("[AgentRuntime] Database env vars present:"), dbEnvVars);
    auto postgresUrl = OR((OR((OR((OR((process->env->DATABASE_POSTGRES_URL), (process->env->DATABASE_URL_UNPOOLED))), (process->env->POSTGRES_URL))), (process->env->POSTGRES_DATABASE_URL))), (DEFAULT_POSTGRES_URL));
    if (OR((!postgresUrl), (postgresUrl == DEFAULT_POSTGRES_URL))) {
        auto isProduction = process->env->NODE_ENV == std::string("production");
        if (AND((isProduction), (postgresUrl == DEFAULT_POSTGRES_URL))) {
            console->error(std::string("[AgentRuntime] ERROR: No database URL found in production!"));
            console->error(std::string("[AgentRuntime] Expected one of: DATABASE_POSTGRES_URL, DATABASE_URL_UNPOOLED, POSTGRES_URL, POSTGRES_DATABASE_URL"));
            throw any(std::make_shared<Error>(std::string("Database connection failed: No database URL configured in production. ") + std::string("Vercel Neon Storage should provide DATABASE_POSTGRES_URL automatically. ") + std::string("Please check your Vercel project settings.")));
        }
    }
    if (AND((postgresUrl), (!postgresUrl->includes(std::string("localhost"))))) {
        auto isValidFormat = OR((postgresUrl->startsWith(std::string("postgres://"))), (postgresUrl->startsWith(std::string("postgresql://"))));
        if (!isValidFormat) {
            console->warn(std::string("[AgentRuntime] WARNING: Database URL doesn't start with postgres:// or postgresql://"));
        }
        auto url = std::make_shared<URL>(postgresUrl->replace((new RegExp(std::string("^postgres(ql)?:\/\"))), std::string("http://")));
        if (OR((!url->hostname), (url->hostname == string_empty))) {
            throw any(std::make_shared<Error>(std::string("Database connection failed: Invalid database URL format (missing hostname)")));
        }
    }
    console->log(std::string("[AgentRuntime] Database config: ") + (postgresUrl->includes(std::string("localhost"))) ? any(std::string("localhost:") + dbPort) : any(std::string("remote (Vercel/Neon)")) + string_empty);
    auto RUNTIME_AGENT_ID = as<std::shared_ptr<UUID>>(std::string("b850bc30-45f8-0041-a00a-83df46d8555d"));
    this->runtime = std::make_shared<AgentRuntime>(as<any>(utils::assign(object{
        , 
        object::pair{std::string("agentId"), RUNTIME_AGENT_ID}, 
        object::pair{std::string("settings"), utils::assign(object{
            object::pair{std::string("GROQ_API_KEY"), OR((process->env->GROQ_API_KEY), (string_empty))}, 
            object::pair{std::string("SMALL_GROQ_MODEL"), OR((process->env->SMALL_GROQ_MODEL), (std::string("qwen/qwen3-32b")))}, 
            object::pair{std::string("LARGE_GROQ_MODEL"), OR((process->env->LARGE_GROQ_MODEL), (std::string("moonshotai/kimi-k2-instruct-0905")))}, 
            object::pair{std::string("POSTGRES_URL"), postgresUrl}
        }, agent["character"]->settings)}
    }, agent)));
    globalState->__elizaRuntime = this->runtime;
    if (OR((!this->runtime["logger"]), (!this->runtime["logger"]["log"]))) {
        this->runtime["logger"] = as<any>(object{
            object::pair{std::string("log"), console->log->bind(console)}, 
            object::pair{std::string("info"), console->info->bind(console)}, 
            object::pair{std::string("warn"), console->warn->bind(console)}, 
            object::pair{std::string("error"), console->error->bind(console)}, 
            object::pair{std::string("debug"), console->debug->bind(console)}, 
            object::pair{std::string("success"), [=](auto message) mutable
            {
                return console->log(std::string("✓ ") + message + string_empty);
            }
            }, 
            object::pair{std::string("notice"), console->info->bind(console)}
        });
    }
    std::async([=]() { this->ensureBuiltInTables(); });
    std::async([=]() { this->runtime["initialize"](); });
    auto services = Array->from(this->runtime["getAllServices"]()["keys"]());
    console->log(std::string("[AgentRuntime] Registered services:"), services);
    return this->runtime;
}

std::shared_ptr<Promise<void>> AgentRuntimeManager::ensureBuiltInTables()
{
    if (OR((this->hasRunMigrations), (globalState->__elizaMigrationsRan))) return std::shared_ptr<Promise<void>>();
    this->hasRunMigrations = true;
    globalState->__elizaMigrationsRan = true;
    console->log(std::string("[AgentRuntime] Using Eliza cache system for quote storage"));
}

std::shared_ptr<Promise<std::shared_ptr<Memory>>> AgentRuntimeManager::handleMessage(string roomId, string entityId, object content)
{
    auto runtime = std::async([=]() { this->getRuntime(); });
    auto entityUuid = as<std::shared_ptr<UUID>>(stringToUuid(entityId));
    std::async([=]() { runtime->ensureConnection(as<any>(object{
        object::pair{std::string("entityId"), entityUuid}, 
        object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(roomId)}, 
        object::pair{std::string("worldId"), stringToUuid(std::string("otc-desk-world"))}, 
        object::pair{std::string("source"), std::string("web")}, 
        object::pair{std::string("type"), ChannelType->DM}, 
        object::pair{std::string("channelId"), roomId}, 
        object::pair{std::string("serverId"), std::string("otc-desk-server")}, 
        object::pair{std::string("userName"), entityId}
    })); });
    auto userMessage = object{
        object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(roomId)}, 
        object::pair{std::string("entityId"), entityUuid}, 
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(runtime->agentId)}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), OR((content["text"]), (string_empty))}, 
            object::pair{std::string("attachments"), OR((content["attachments"]), (array<any>()))}
        }}
    };
    std::async([=]() { runtime->emitEvent(EventType->MESSAGE_RECEIVED, object{
        object::pair{std::string("runtime"), std::string("runtime")}, 
        object::pair{std::string("message"), object{
            object::pair{std::string("id"), userMessage->id}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), OR((content["text"]), (string_empty))}, 
                object::pair{std::string("attachments"), OR((content["attachments"]), (array<any>()))}
            }}, 
            object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(stringToUuid(entityId))}, 
            object::pair{std::string("agentId"), runtime->agentId}, 
            object::pair{std::string("roomId"), roomId}, 
            object::pair{std::string("createdAt"), Date->now()}
        }}, 
        object::pair{std::string("callback"), [=]() mutable
        {
            console->log(std::string("[AgentRuntime] Message handler completed"));
        }
        }
    }); });
    return userMessage;
}

std::shared_ptr<GlobalElizaState> globalState = as<std::shared_ptr<GlobalElizaState>>(as<any>(globalThis));
std::shared_ptr<AgentRuntimeManager> agentRuntime = AgentRuntimeManager::getInstance();

void Main(void)
{
    if (type_of(globalState->__elizaMigrationsRan) == std::string("undefined")) globalState->__elizaMigrationsRan = false;
    if (type_of(globalState->__elizaManagerLogged) == std::string("undefined")) globalState->__elizaManagerLogged = false;
}

MAIN
