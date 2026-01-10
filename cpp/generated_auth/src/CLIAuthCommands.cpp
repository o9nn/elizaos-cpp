#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-elizaos-services/src/auth/CLIAuthCommands.h"

CLIAuthCommands::CLIAuthCommands(std::shared_ptr<IAgentRuntime> runtime) {
    this->authService = std::make_shared<AuthenticationService>(runtime);
}

array<std::shared_ptr<CLICommand>> CLIAuthCommands::getCommands()
{
    return array<object>{ object{
        object::pair{std::string("name"), std::string("auth:status")}, 
        object::pair{std::string("description"), std::string("Check authentication status for all providers")}, 
        object::pair{std::string("handler"), std::bind(&CLIAuthCommands::handleAuthStatus, this)->bind(shared_from_this())}
    }, object{
        object::pair{std::string("name"), std::string("auth:test")}, 
        object::pair{std::string("description"), std::string("Test API functionality for all configured providers")}, 
        object::pair{std::string("handler"), std::bind(&CLIAuthCommands::handleAuthTest, this)->bind(shared_from_this())}
    }, object{
        object::pair{std::string("name"), std::string("auth:validate")}, 
        object::pair{std::string("description"), std::string("Validate specific API key")}, 
        object::pair{std::string("options"), array<object>{ object{
            object::pair{std::string("name"), std::string("provider")}, 
            object::pair{std::string("description"), std::string("Provider name (openai, groq, anthropic)")}, 
            object::pair{std::string("required"), true}, 
            object::pair{std::string("type"), std::string("string")}
        }, object{
            object::pair{std::string("name"), std::string("key")}, 
            object::pair{std::string("description"), std::string("API key to validate")}, 
            object::pair{std::string("required"), true}, 
            object::pair{std::string("type"), std::string("string")}
        } }}, 
        object::pair{std::string("handler"), std::bind(&CLIAuthCommands::handleAuthValidate, this, std::placeholders::_1)->bind(shared_from_this())}
    }, object{
        object::pair{std::string("name"), std::string("auth:test-keys")}, 
        object::pair{std::string("description"), std::string("Show available test keys for development")}, 
        object::pair{std::string("handler"), std::bind(&CLIAuthCommands::handleTestKeys, this)->bind(shared_from_this())}
    }, object{
        object::pair{std::string("name"), std::string("auth:clear-cache")}, 
        object::pair{std::string("description"), std::string("Clear authentication cache")}, 
        object::pair{std::string("handler"), std::bind(&CLIAuthCommands::handleClearCache, this)->bind(shared_from_this())}
    }, object{
        object::pair{std::string("name"), std::string("auth:setup")}, 
        object::pair{std::string("description"), std::string("Interactive API key setup wizard")}, 
        object::pair{std::string("handler"), std::bind(&CLIAuthCommands::handleSetup, this)->bind(shared_from_this())}
    } };
}

std::shared_ptr<Promise<void>> CLIAuthCommands::handleAuthStatus()
{
    console->log(std::string("🔍 Checking Authentication Status...\
"));
    try
    {
        auto status = std::async([=]() { this->authService->getAuthStatus(); });
        console->log(std::string("📊 Overall Status: ") + this->getStatusIcon(status->overall) + std::string(" ") + status->overall->toUpperCase() + string_empty);
        console->log(std::string("🕒 Last Checked: ") + status->lastChecked->toLocaleString() + string_empty);
        console->log(std::string("⚡ Available Capabilities: ") + status->capabilities->join(std::string(", ")) + std::string("\
"));
        console->log(std::string("📋 Provider Details:"));
        console->log(std::string("═")->repeat(80));
        for (auto& [provider, result] : Object->entries(status->providers))
        {
            auto icon = (result->isValid) ? std::string("✅") : std::string("❌");
            auto keyType = (result->keyType == std::string("test")) ? any(std::string("🧪 TEST")) : any((result->keyType == std::string("production")) ? std::string("🚀 PROD") : std::string("❌ INVALID"));
            console->log(string_empty + icon + std::string(" ") + provider->toUpperCase() + string_empty);
            console->log(std::string("   Type: ") + keyType + string_empty);
            console->log(std::string("   Capabilities: ") + (OR((result->capabilities->join(std::string(", "))), (std::string("None")))) + string_empty);
            if (result->errorMessage) {
                console->log(std::string("   Error: ") + result->errorMessage + string_empty);
            }
            if (result->rateLimits) {
                console->log(std::string("   Rate Limits: ") + result->rateLimits["remaining"] + std::string(" remaining"));
            }
            console->log(string_empty);
        }
        if (status->overall == std::string("failed")) {
            console->log(std::string("⚠️  No valid API keys configured. Use `elizaos auth:setup` to configure keys."));
        } else if (status->overall == std::string("degraded")) {
            console->log(std::string("⚠️  Some providers are not configured. Consider adding more API keys for redundancy."));
        } else {
            console->log(std::string("🎉 All systems operational!"));
        }
    }
    catch (const any& error)
    {
        console->error(std::string("❌ Failed to check authentication status:"), (is<Error>(error)) ? any(error->message) : any(error));
        process->exit(1);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> CLIAuthCommands::handleAuthTest()
{
    console->log(std::string("🧪 Testing API Functionality...\
"));
    auto providers = array<string>{ std::string("openai"), std::string("groq"), std::string("anthropic") };
    auto results = object{};
    for (auto& provider : providers)
    {
        console->log(std::string("Testing ") + provider + std::string("..."));
        try
        {
            auto result = std::async([=]() { this->authService->testApiFunctionality(provider); });
            results[provider] = result;
            if (result["success"]) {
                console->log(std::string("✅ ") + provider + std::string(": ") + (OR((result["response"]->substring(0, 50)), (std::string("SUCCESS")))) + std::string("..."));
                console->log(std::string("   Latency: ") + result["latency"] + std::string("ms, Tokens: ") + (OR((result["tokenUsage"]), (std::string("N/A")))) + std::string("\
"));
            } else {
                console->log(std::string("❌ ") + provider + std::string(": ") + result["error"] + std::string("\
"));
            }
        }
        catch (const any& error)
        {
            console->log(std::string("💥 ") + provider + std::string(": Test crashed - ") + (is<Error>(error)) ? any(error->message) : any(error) + std::string("\
"));
            results[provider] = object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), std::string("Test crashed")}
            };
        }
    }
    auto successful = Object->values(results)->filter([=](auto r) mutable
    {
        return r["success"];
    }
    )->get_length();
    auto total = Object->keys(results)->get_length();
    console->log(std::string("═")->repeat(50));
    console->log(std::string("📊 Test Results: ") + successful + std::string("/") + total + std::string(" providers working"));
    if (successful == total) {
        console->log(std::string("🎉 All API tests passed!"));
    } else if (successful > 0) {
        console->log(std::string("⚠️  Some APIs are not working. Check your configuration."));
    } else {
        console->log(std::string("❌ No APIs are working. Please check your API keys."));
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> CLIAuthCommands::handleAuthValidate(object args)
{
    console->log(std::string("🔍 Validating ") + args["provider"] + std::string(" API key...\
"));
    try
    {
        auto result = std::async([=]() { this->authService->validateApiKey(args["provider"], args["key"]); });
        if (result->isValid) {
            console->log(std::string("✅ API Key Valid"));
            console->log(std::string("   Provider: ") + result->provider + string_empty);
            console->log(std::string("   Type: ") + result->keyType + string_empty);
            console->log(std::string("   Capabilities: ") + result->capabilities->join(std::string(", ")) + string_empty);
            if (result->rateLimits) {
                console->log(std::string("   Rate Limits: ") + result->rateLimits["remaining"] + std::string(" remaining"));
            }
        } else {
            console->log(std::string("❌ API Key Invalid"));
            console->log(std::string("   Error: ") + (OR((result->errorMessage), (std::string("Unknown error")))) + string_empty);
        }
    }
    catch (const any& error)
    {
        console->error(std::string("❌ Validation failed:"), (is<Error>(error)) ? any(error->message) : any(error));
        process->exit(1);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> CLIAuthCommands::handleTestKeys()
{
    console->log(std::string("🧪 Available Test Keys for Development\
"));
    console->log(std::string("═")->repeat(60));
    console->log(std::string("These keys are safe for development and testing:"));
    console->log(string_empty);
    for (auto& [name, key] : Object->entries(TEST_KEYS))
    {
        auto provider = name->replace(std::string("_TEST_KEY"), string_empty)->toLowerCase();
        console->log(string_empty + provider->toUpperCase() + std::string(":"));
        console->log(std::string("  Key: ") + key + string_empty);
        console->log(std::string("  Usage: export ") + name->replace(std::string("_TEST_KEY"), std::string("_API_KEY")) + std::string("="") + key + std::string("""));
        console->log(string_empty);
    }
    console->log(std::string("💡 Test keys provide simulated responses without real API costs."));
    console->log(std::string("💡 Use them for development, testing, and CI/CD pipelines."));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> CLIAuthCommands::handleClearCache()
{
    console->log(std::string("🧹 Clearing authentication cache..."));
    this->authService->clearCache();
    console->log(std::string("✅ Authentication cache cleared."));
    console->log(std::string("💡 Next validation calls will perform fresh API checks."));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> CLIAuthCommands::handleSetup()
{
    console->log(std::string("🚀 ElizaOS Services API Key Setup Wizard\
"));
    console->log(std::string("This wizard will help you configure API keys for all providers."));
    console->log(std::string("═")->repeat(70));
    auto providers = array<object>{ object{
        object::pair{std::string("name"), std::string("OpenAI")}, 
        object::pair{std::string("envVar"), std::string("OPENAI_API_KEY")}, 
        object::pair{std::string("description"), std::string("Required for text generation, embeddings, and image description")}, 
        object::pair{std::string("getUrl"), std::string("https://platform.openai.com/account/api-keys")}
    }, object{
        object::pair{std::string("name"), std::string("Groq")}, 
        object::pair{std::string("envVar"), std::string("GROQ_API_KEY")}, 
        object::pair{std::string("description"), std::string("Optional for fast text generation with Llama models")}, 
        object::pair{std::string("getUrl"), std::string("https://console.groq.com/keys")}
    }, object{
        object::pair{std::string("name"), std::string("Anthropic")}, 
        object::pair{std::string("envVar"), std::string("ANTHROPIC_API_KEY")}, 
        object::pair{std::string("description"), std::string("Optional for Claude text generation and image description")}, 
        object::pair{std::string("getUrl"), std::string("https://console.anthropic.com/account/keys")}
    } };
    console->log(std::string("\
📋 Provider Information:"));
    console->log(string_empty);
    for (auto& provider : providers)
    {
        console->log(string_empty + provider["name"] + std::string(":"));
        console->log(std::string("  Environment Variable: ") + provider["envVar"] + string_empty);
        console->log(std::string("  Description: ") + provider["description"] + string_empty);
        console->log(std::string("  Get API Key: ") + provider["getUrl"] + string_empty);
        console->log(string_empty);
    }
    console->log(std::string("💡 Setup Instructions:"));
    console->log(std::string("1. Get API keys from the URLs above"));
    console->log(std::string("2. Add them to your .env file:"));
    console->log(std::string("   OPENAI_API_KEY=sk-..."));
    console->log(std::string("   GROQ_API_KEY=gsk_..."));
    console->log(std::string("   ANTHROPIC_API_KEY=sk-ant-..."));
    console->log(std::string("3. Run `elizaos auth:status` to verify configuration"));
    console->log(string_empty);
    console->log(std::string("🧪 For Development/Testing:"));
    console->log(std::string("Use `elizaos auth:test-keys` to see test keys that work without real API costs."));
    return std::shared_ptr<Promise<void>>();
}

string CLIAuthCommands::getStatusIcon(string status)
{
    static switch_type __switch9751_9941 = {
        { any(std::string("healthy")), 1 },
        { any(std::string("degraded")), 2 },
        { any(std::string("failed")), 3 }
    };
    switch (__switch9751_9941[status])
    {
    case 1:
        return std::string("✅");
    case 2:
        return std::string("⚠️");
    case 3:
        return std::string("❌");
    default:
        return std::string("❓");
    }
}

array<std::shared_ptr<CLICommand>> registerAuthCommands(std::shared_ptr<IAgentRuntime> runtime)
{
    auto authCommands = std::make_shared<CLIAuthCommands>(runtime);
    return authCommands->getCommands();
};


