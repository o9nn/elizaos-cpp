#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-elizaos-services/src/auth/CLIAuthCommands.h"

CLIAuthCommands::CLIAuthCommands(std::shared_ptr<IAgentRuntime> runtime) {
    this->authService = std::make_shared<AuthenticationService>(runtime);
}

array<std::shared_ptr<CLICommand>> CLIAuthCommands::getCommands()
{
    return array<object>{ object{
        object::pair{std:("name"), std:("auth:status")}, 
        object::pair{std:("description"), std:("Check authentication status for all providers")}, 
        object::pair{std:("handler"), std::bind(&CLIAuthCommands::handleAuthStatus, this)->bind(shared_from_this())}
    }, object{
        object::pair{std:("name"), std:("auth:test")}, 
        object::pair{std:("description"), std:("Test API functionality for all configured providers")}, 
        object::pair{std:("handler"), std::bind(&CLIAuthCommands::handleAuthTest, this)->bind(shared_from_this())}
    }, object{
        object::pair{std:("name"), std:("auth:validate")}, 
        object::pair{std:("description"), std:("Validate specific API key")}, 
        object::pair{std:("options"), array<object>{ object{
            object::pair{std:("name"), std:("provider")}, 
            object::pair{std:("description"), std:("Provider name (openai, groq, anthropic)")}, 
            object::pair{std:("required"), true}, 
            object::pair{std:("type"), std:("string")}
        }, object{
            object::pair{std:("name"), std:("key")}, 
            object::pair{std:("description"), std:("API key to validate")}, 
            object::pair{std:("required"), true}, 
            object::pair{std:("type"), std:("string")}
        } }}, 
        object::pair{std:("handler"), std::bind(&CLIAuthCommands::handleAuthValidate, this, std::placeholders::_1)->bind(shared_from_this())}
    }, object{
        object::pair{std:("name"), std:("auth:test-keys")}, 
        object::pair{std:("description"), std:("Show available test keys for development")}, 
        object::pair{std:("handler"), std::bind(&CLIAuthCommands::handleTestKeys, this)->bind(shared_from_this())}
    }, object{
        object::pair{std:("name"), std:("auth:clear-cache")}, 
        object::pair{std:("description"), std:("Clear authentication cache")}, 
        object::pair{std:("handler"), std::bind(&CLIAuthCommands::handleClearCache, this)->bind(shared_from_this())}
    }, object{
        object::pair{std:("name"), std:("auth:setup")}, 
        object::pair{std:("description"), std:("Interactive API key setup wizard")}, 
        object::pair{std:("handler"), std::bind(&CLIAuthCommands::handleSetup, this)->bind(shared_from_this())}
    } };
}

std::shared_ptr<Promise<void>> CLIAuthCommands::handleAuthStatus()
{
    console->log(std:("🔍 Checking Authentication Status...\
"));
    try
    {
        auto status = std::async([=]() { this->authService->getAuthStatus(); });
        console->log(std:("📊 Overall Status: ") + this->getStatusIcon(status->overall) + std:(" ") + status->overall->toUpperCase() + string_empty);
        console->log(std:("🕒 Last Checked: ") + status->lastChecked->toLocaleString() + string_empty);
        console->log(std:("⚡ Available Capabilities: ") + status->capabilities->join(std:(", ")) + std:("\
"));
        console->log(std:("📋 Provider Details:"));
        console->log(std:("═")->repeat(80));
        for (auto& [provider, result] : Object->entries(status->providers))
        {
            auto icon = (result->isValid) ? std:("✅") : std:("❌");
            auto keyType = (result->keyType == std:("test")) ? any(std:("🧪 TEST")) ((result->keyType == std:("production")) ? std:("🚀 PROD") : std:("❌ INVALID"));
            console->log(string_empty + icon + std:(" ") + provider->toUpperCase() + string_empty);
            console->log(std:("   Type: ") + keyType + string_empty);
            console->log(std:("   Capabilities: ") + (OR((result->capabilities->join(std:(", "))), (std:("None")))) + string_empty);
            if (result->errorMessage) {
                console->log(std:("   Error: ") + result->errorMessage + string_empty);
            }
            if (result->rateLimits) {
                console->log(std:("   Rate Limits: ") + result->rateLimits["remaining"] + std:(" remaining"));
            }
            console->log(string_empty);
        }
        if (status->overall == std:("failed")) {
            console->log(std:("⚠️  No valid API keys configured. Use "elizaos auth:setup" to configure keys."));
        } else if (status->overall == std:("degraded")) {
            console->log(std:("⚠️  Some providers are not configured. Consider adding more API keys for redundancy."));
        } else {
            console->log(std:("🎉 All systems operational!"));
        }
    }
    catch (const any& error)
    {
        console->error(std:("❌ Failed to check authentication status:"), (is<Error>(error)) ? any(error->message) (error));
        process->exit(1);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> CLIAuthCommands::handleAuthTest()
{
    console->log(std:("🧪 Testing API Functionality...\
"));
    auto providers = array<string>{ std:("openai"), std:("groq"), std:("anthropic") };
    auto results = object{};
    for (auto& provider : providers)
    {
        console->log(std:("Testing ") + provider + std:("..."));
        try
        {
            auto result = std::async([=]() { this->authService->testApiFunctionality(provider); });
            results[provider] = result;
            if (result["success"]) {
                console->log(std:("✅ ") + provider + std:(": ") + (OR((result["response"]->substring(0, 50)), (std:("SUCCESS")))) + std:("..."));
                console->log(std:("   Latency: ") + result["latency"] + std:("ms, Tokens: ") + (OR((result["tokenUsage"]), (std:("N/A")))) + std:("\
"));
            } else {
                console->log(std:("❌ ") + provider + std:(": ") + result["error"] + std:("\
"));
            }
        }
        catch (const any& error)
        {
            console->log(std:("💥 ") + provider + std:(": Test crashed - ") + (is<Error>(error)) ? any(error->message) (error) + std:("\
"));
            results[provider] = object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std:("Test crashed")}
            };
        }
    }
    auto successful = Object->values(results)->filter([=](auto r) mutable
    {
        return r["success"];
    }
    )->get_length();
    auto total = Object->keys(results)->get_length();
    console->log(std:("═")->repeat(50));
    console->log(std:("📊 Test Results: ") + successful + std:("/") + total + std:(" providers working"));
    if (successful == total) {
        console->log(std:("🎉 All API tests passed!"));
    } else if (successful > 0) {
        console->log(std:("⚠️  Some APIs are not working. Check your configuration."));
    } else {
        console->log(std:("❌ No APIs are working. Please check your API keys."));
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> CLIAuthCommands::handleAuthValidate(object args)
{
    console->log(std:("🔍 Validating ") + args["provider"] + std:(" API key...\
"));
    try
    {
        auto result = std::async([=]() { this->authService->validateApiKey(args["provider"], args["key"]); });
        if (result->isValid) {
            console->log(std:("✅ API Key Valid"));
            console->log(std:("   Provider: ") + result->provider + string_empty);
            console->log(std:("   Type: ") + result->keyType + string_empty);
            console->log(std:("   Capabilities: ") + result->capabilities->join(std:(", ")) + string_empty);
            if (result->rateLimits) {
                console->log(std:("   Rate Limits: ") + result->rateLimits["remaining"] + std:(" remaining"));
            }
        } else {
            console->log(std:("❌ API Key Invalid"));
            console->log(std:("   Error: ") + (OR((result->errorMessage), (std:("Unknown error")))) + string_empty);
        }
    }
    catch (const any& error)
    {
        console->error(std:("❌ Validation failed:"), (is<Error>(error)) ? any(error->message) (error));
        process->exit(1);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> CLIAuthCommands::handleTestKeys()
{
    console->log(std:("🧪 Available Test Keys for Development\
"));
    console->log(std:("═")->repeat(60));
    console->log(std:("These keys are safe for development and testing:"));
    console->log(string_empty);
    for (auto& [name, key] : Object->entries(TEST_KEYS))
    {
        auto provider = name->replace(std:("_TEST_KEY"), string_empty)->toLowerCase();
        console->log(string_empty + provider->toUpperCase() + std:(":"));
        console->log(std:("  Key: ") + key + string_empty);
        console->log(std:("  Usage: ") + name->replace(std:("_TEST_KEY"), std:("_API_KEY")) + std:("="") + key + std:("""));
        console->log(string_empty);
    }
    console->log(std:("💡 Test keys provide simulated responses without real API costs."));
    console->log(std:("💡 Use them for development, testing, and CI/CD pipelines."));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> CLIAuthCommands::handleClearCache()
{
    console->log(std:("🧹 Clearing authentication cache..."));
    this->authService->clearCache();
    console->log(std:("✅ Authentication cache cleared."));
    console->log(std:("💡 Next validation calls will perform fresh API checks."));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> CLIAuthCommands::handleSetup()
{
    console->log(std:("🚀 ElizaOS Services API Key Setup Wizard\
"));
    console->log(std:("This wizard will help you configure API keys for all providers."));
    console->log(std:("═")->repeat(70));
    auto providers = array<object>{ object{
        object::pair{std:("name"), std:("OpenAI")}, 
        object::pair{std:("envVar"), std:("OPENAI_API_KEY")}, 
        object::pair{std:("description"), std:("Required for text generation, embeddings, and image description")}, 
        object::pair{std:("getUrl"), std:("https://platform.openai.com/account/api-keys")}
    }, object{
        object::pair{std:("name"), std:("Groq")}, 
        object::pair{std:("envVar"), std:("GROQ_API_KEY")}, 
        object::pair{std:("description"), std:("Optional for fast text generation with Llama models")}, 
        object::pair{std:("getUrl"), std:("https://console.groq.com/keys")}
    }, object{
        object::pair{std:("name"), std:("Anthropic")}, 
        object::pair{std:("envVar"), std:("ANTHROPIC_API_KEY")}, 
        object::pair{std:("description"), std:("Optional for Claude text generation and image description")}, 
        object::pair{std:("getUrl"), std:("https://console.anthropic.com/account/keys")}
    } };
    console->log(std:("\
📋 Provider Information:"));
    console->log(string_empty);
    for (auto& provider : providers)
    {
        console->log(string_empty + provider["name"] + std:(":"));
        console->log(std:("  Environment Variable: ") + provider["envVar"] + string_empty);
        console->log(std:("  Description: ") + provider["description"] + string_empty);
        console->log(std:("  Get API Key: ") + provider["getUrl"] + string_empty);
        console->log(string_empty);
    }
    console->log(std:("💡 Setup Instructions:"));
    console->log(std:("1. Get API keys from the URLs above"));
    console->log(std:("2. Add them to your .env file:"));
    console->log(std:("   OPENAI_API_KEY=sk-..."));
    console->log(std:("   GROQ_API_KEY=gsk_..."));
    console->log(std:("   ANTHROPIC_API_KEY=sk-ant-..."));
    console->log(std:("3. Run "elizaos auth:status" to verify configuration"));
    console->log(string_empty);
    console->log(std:("🧪 For Development/Testing:"));
    console->log(std:("Use "elizaos auth:test-keys" to see test keys that work without real API costs."));
    return std::shared_ptr<Promise<void>>();
}

string CLIAuthCommands::getStatusIcon(string status)
{
    static switch_type __switch9751_9941 = {
        { any(std:("healthy")), 1 },
        { any(std:("degraded")), 2 },
        { any(std:("failed")), 3 }
    };
    switch (__switch9751_9941[status])
    {
    case 1:
        return std:("✅");
    case 2:
        return std:("⚠️");
    case 3:
        return std:("❌");
    default:
        return std:("❓");
    }
}

array<std::shared_ptr<CLICommand>> registerAuthCommands(std::shared_ptr<IAgentRuntime> runtime)
{
    auto authCommands = std::make_shared<CLIAuthCommands>(runtime);
    return authCommands->getCommands();
};


