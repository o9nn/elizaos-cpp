#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_SCENARIOS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_SCENARIOS_H
#include "core.h"
#include "@elizaos/core.h"

class AutoCoderScenariosTestSuite;

class AutoCoderScenariosTestSuite : public TestSuite, public std::enable_shared_from_this<AutoCoderScenariosTestSuite> {
public:
    using std::enable_shared_from_this<AutoCoderScenariosTestSuite>::shared_from_this;
    string name = std::string("autocoder-real-world-scenarios");

    string description = std::string("Real-world application generation scenarios using live APIs and runtime");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Generate Tesla News Discord Bot")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🚀 Testing Tesla News Discord Bot generation..."));
            if (!process->env->DISCORD_BOT_TOKEN) {
                console->log(std::string("⏭️ Skipping Discord bot test - no DISCORD_BOT_TOKEN"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime["getService"](std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            auto teslaNewsProject = object{
                object::pair{std::string("projectName"), std::string("tesla-news-discord-bot")}, 
                object::pair{std::string("description"), std::string("Discord bot that monitors RSS feeds for Tesla news and sends alerts")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Monitor RSS feeds for Tesla-related news"), std::string("Send Discord notifications when Tesla news is found"), std::string("Filter news by keywords (Tesla, Elon Musk, stock)"), std::string("Throttle notifications to avoid spam"), std::string("Store seen articles to prevent duplicates"), std::string("Support multiple RSS sources"), std::string("Include article title, summary, and link in Discord messages") }}, 
                object::pair{std::string("apis"), array<string>{ std::string("Discord API"), std::string("RSS feed parsing"), std::string("Web scraping for article content") }}, 
                object::pair{std::string("testScenarios"), array<string>{ std::string("Parse RSS feed and extract Tesla articles"), std::string("Send formatted Discord message"), std::string("Handle duplicate article detection"), std::string("Process multiple RSS sources"), std::string("Throttle notification frequency") }}, 
                object::pair{std::string("githubRepo"), std::string("tesla-news-test-") + Date->now() + string_empty}
            };
            auto result = std::async([=]() { codeGenService["generateCode"](teslaNewsProject); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std::string("Tesla news bot generation failed: ") + result["errors"]["join"](std::string(", ")) + string_empty));
            }
            console->log(std::string("✅ Tesla news Discord bot generated successfully"));
            if (OR((!result["files"]), (result["files"]["length"] == 0))) {
                throw any(std::make_shared<Error>(std::string("No files generated")));
            }
            auto fileNames = result["files"]["map"]([=](auto f) mutable
            {
                return f["path"];
            }
            );
            auto requiredFiles = array<string>{ std::string("src/index.ts"), std::string("package.json"), std::string("README.md") };
            for (auto& file : requiredFiles)
            {
                if (!fileNames["includes"](file)) {
                    throw any(std::make_shared<Error>(std::string("Missing required file: ") + file + string_empty));
                }
            }
            auto indexFile = result["files"]["find"]([=](auto f) mutable
            {
                return f["path"] == std::string("src/index.ts");
            }
            );
            if (!indexFile) {
                throw any(std::make_shared<Error>(std::string("Missing index.ts file")));
            }
            auto content = indexFile["content"]["toLowerCase"]();
            if (OR((!content["includes"](std::string("discord"))), (!content["includes"](std::string("rss"))))) {
                throw any(std::make_shared<Error>(std::string("Generated code missing Discord or RSS functionality")));
            }
            console->log(std::string("📁 Generated ") + result["files"]["length"] + std::string(" files for Tesla news bot"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Generate Global Weather App")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🌤️ Testing Global Weather App generation..."));
            if (!process->env->OPENWEATHER_API_KEY) {
                console->log(std::string("⏭️ Skipping weather app test - no OPENWEATHER_API_KEY"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime["getService"](std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            auto weatherAppProject = object{
                object::pair{std::string("projectName"), std::string("global-weather-app")}, 
                object::pair{std::string("description"), std::string("Complete weather application supporting global location queries")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("agent"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Query weather for any location worldwide"), std::string("Support city names, coordinates, and postal codes"), std::string("Provide current weather and forecasts"), std::string("Include weather alerts and warnings"), std::string("Support multiple weather data providers"), std::string("Cache weather data to reduce API calls"), std::string("Handle location disambiguation"), std::string("Provide weather maps and radar") }}, 
                object::pair{std::string("apis"), array<string>{ std::string("OpenWeatherMap API"), std::string("WeatherAPI.com"), std::string("Location geocoding services"), std::string("Time zone APIs") }}, 
                object::pair{std::string("testScenarios"), array<string>{ std::string("Query weather for major cities"), std::string("Handle ambiguous location names"), std::string("Provide accurate forecasts"), std::string("Display weather alerts"), std::string("Cache and retrieve weather data"), std::string("Support different units (metric/imperial)") }}, 
                object::pair{std::string("githubRepo"), std::string("weather-app-test-") + Date->now() + string_empty}, 
                object::pair{std::string("personality"), std::string("Helpful and informative weather assistant")}, 
                object::pair{std::string("knowledge"), array<string>{ std::string("Weather patterns and terminology"), std::string("Global geography and time zones"), std::string("Weather safety and alerts") }}
            };
            auto result = std::async([=]() { codeGenService["generateCode"](weatherAppProject); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std::string("Weather app generation failed: ") + result["errors"]["join"](std::string(", ")) + string_empty));
            }
            console->log(std::string("✅ Global weather app generated successfully"));
            if (OR((!result["files"]), (result["files"]["length"] == 0))) {
                throw any(std::make_shared<Error>(std::string("No files generated")));
            }
            auto fileNames = result["files"]["map"]([=](auto f) mutable
            {
                return f["path"];
            }
            );
            auto requiredFiles = array<string>{ std::string("src/index.ts"), std::string("package.json"), std::string("character.json") };
            for (auto& file : requiredFiles)
            {
                if (!fileNames["includes"](file)) {
                    throw any(std::make_shared<Error>(std::string("Missing required file: ") + file + string_empty));
                }
            }
            auto indexFile = result["files"]["find"]([=](auto f) mutable
            {
                return f["path"] == std::string("src/index.ts");
            }
            );
            if (!indexFile) {
                throw any(std::make_shared<Error>(std::string("Missing index.ts file")));
            }
            auto content = indexFile["content"]["toLowerCase"]();
            if (!content["includes"](std::string("weather"))) {
                throw any(std::make_shared<Error>(std::string("Generated code missing weather functionality")));
            }
            auto characterFile = result["files"]["find"]([=](auto f) mutable
            {
                return f["path"] == std::string("character.json");
            }
            );
            if (!characterFile) {
                throw any(std::make_shared<Error>(std::string("Missing character.json file for agent")));
            }
            auto character = JSON->parse(characterFile["content"]);
            if (OR((!character["name"]), (!character["bio"]))) {
                throw any(std::make_shared<Error>(std::string("Invalid character configuration")));
            }
            console->log(std::string("📁 Generated ") + result["files"]["length"] + std::string(" files for weather app"));
            if (result["agentId"]) {
                console->log(std::string("🤖 Agent ID: ") + result["agentId"] + string_empty);
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Validate RSS Monitoring Functionality")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🔍 Testing RSS monitoring capabilities..."));
            if (!process->env->DISCORD_BOT_TOKEN) {
                console->log(std::string("⏭️ Skipping RSS monitoring test - no DISCORD_BOT_TOKEN"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime["getService"](std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            auto rssProject = object{
                object::pair{std::string("projectName"), std::string("rss-monitor-test")}, 
                object::pair{std::string("description"), std::string("RSS monitoring with keyword filtering")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Parse RSS feeds from multiple sources"), std::string("Filter articles by keywords"), std::string("Detect duplicate articles"), std::string("Schedule periodic RSS checks"), std::string("Handle RSS feed errors gracefully") }}, 
                object::pair{std::string("apis"), array<string>{ std::string("RSS parsing"), std::string("Web scraping") }}, 
                object::pair{std::string("testScenarios"), array<string>{ std::string("Parse valid RSS feed"), std::string("Handle invalid RSS feed"), std::string("Filter by keywords"), std::string("Detect duplicates") }}
            };
            auto result = std::async([=]() { codeGenService["generateCode"](rssProject); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std::string("RSS monitoring generation failed: ") + result["errors"]["join"](std::string(", ")) + string_empty));
            }
            if (!result["files"]) {
                throw any(std::make_shared<Error>(std::string("No files generated")));
            }
            auto rssFiles = result["files"]["filter"]([=](auto f) mutable
            {
                return OR((OR((f["content"]->toLowerCase()->includes(std::string("rss"))), (f["content"]->toLowerCase()->includes(std::string("feed"))))), (f["content"]->toLowerCase()->includes(std::string("xml"))));
            }
            );
            if (rssFiles["length"] == 0) {
                throw any(std::make_shared<Error>(std::string("No RSS functionality found in generated code")));
            }
            auto hasKeywordFiltering = result["files"]["some"]([=](auto f) mutable
            {
                return AND((f["content"]->toLowerCase()->includes(std::string("filter"))), (f["content"]->toLowerCase()->includes(std::string("keyword"))));
            }
            );
            if (!hasKeywordFiltering) {
                throw any(std::make_shared<Error>(std::string("Missing keyword filtering functionality")));
            }
            console->log(std::string("✅ RSS monitoring functionality verified"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Weather API Integration")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🌍 Testing weather API integration..."));
            if (!process->env->OPENWEATHER_API_KEY) {
                console->log(std::string("⏭️ Skipping weather API test - no OPENWEATHER_API_KEY"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime["getService"](std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            auto weatherApiProject = object{
                object::pair{std::string("projectName"), std::string("weather-api-integration")}, 
                object::pair{std::string("description"), std::string("Weather API integration with caching and error handling")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Connect to OpenWeatherMap API"), std::string("Implement data caching"), std::string("Handle API rate limits"), std::string("Support multiple location formats"), std::string("Provide error recovery") }}, 
                object::pair{std::string("apis"), array<string>{ std::string("OpenWeatherMap API") }}, 
                object::pair{std::string("testScenarios"), array<string>{ std::string("Query weather by city name"), std::string("Query weather by coordinates"), std::string("Handle API errors"), std::string("Test caching behavior") }}
            };
            auto result = std::async([=]() { codeGenService["generateCode"](weatherApiProject); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std::string("Weather API integration failed: ") + result["errors"]["join"](std::string(", ")) + string_empty));
            }
            if (!result["files"]) {
                throw any(std::make_shared<Error>(std::string("No files generated")));
            }
            auto apiFiles = result["files"]["filter"]([=](auto f) mutable
            {
                return OR((OR((f["content"]->toLowerCase()->includes(std::string("api"))), (f["content"]->toLowerCase()->includes(std::string("fetch"))))), (f["content"]->toLowerCase()->includes(std::string("request"))));
            }
            );
            if (apiFiles["length"] == 0) {
                throw any(std::make_shared<Error>(std::string("No API integration found in generated code")));
            }
            auto hasCaching = result["files"]["some"]([=](auto f) mutable
            {
                return OR((OR((f["content"]->toLowerCase()->includes(std::string("cache"))), (f["content"]->toLowerCase()->includes(std::string("redis"))))), (f["content"]->toLowerCase()->includes(std::string("storage"))));
            }
            );
            if (!hasCaching) {
                throw any(std::make_shared<Error>(std::string("Missing caching functionality")));
            }
            auto hasErrorHandling = result["files"]["some"]([=](auto f) mutable
            {
                return OR((OR(((AND((f["content"]->includes(std::string("try"))), (f["content"]->includes(std::string("catch")))))), (f["content"]->includes(std::string("error"))))), (f["content"]->includes(std::string("Error"))));
            }
            );
            if (!hasErrorHandling) {
                throw any(std::make_shared<Error>(std::string("Missing error handling")));
            }
            console->log(std::string("✅ Weather API integration verified"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("End-to-End Application Validation")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🔍 Testing complete application functionality..."));
            if (OR((!process->env->ANTHROPIC_API_KEY), (!process->env->E2B_API_KEY))) {
                console->log(std::string("⏭️ Skipping E2E validation - missing API keys"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime["getService"](std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            auto e2eProject = object{
                object::pair{std::string("projectName"), std::string("e2e-validation-test")}, 
                object::pair{std::string("description"), std::string("Complete application with comprehensive validation")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("agent"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Multi-step conversation flow"), std::string("External API integration"), std::string("Data persistence"), std::string("Error handling"), std::string("Comprehensive testing") }}, 
                object::pair{std::string("apis"), array<string>{ std::string("Mock external API") }}, 
                object::pair{std::string("testScenarios"), array<string>{ std::string("Complete conversation flow"), std::string("API integration test"), std::string("Data persistence test"), std::string("Error recovery test"), std::string("Performance test") }}, 
                object::pair{std::string("personality"), std::string("Professional and helpful assistant")}, 
                object::pair{std::string("knowledge"), array<string>{ std::string("Domain-specific knowledge base") }}
            };
            auto result = std::async([=]() { codeGenService["generateCode"](e2eProject); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std::string("E2E validation failed: ") + result["errors"]["join"](std::string(", ")) + string_empty));
            }
            if (OR((!result["files"]), (result["files"]["length"] <= 5))) {
                throw any(std::make_shared<Error>(std::string("Insufficient files generated for complete application")));
            }
            auto fileNames = result["files"]["map"]([=](auto f) mutable
            {
                return f["path"];
            }
            );
            auto requiredFiles = array<string>{ std::string("src/index.ts"), std::string("package.json"), std::string("character.json"), std::string("README.md") };
            for (auto& file : requiredFiles)
            {
                if (!fileNames["includes"](file)) {
                    throw any(std::make_shared<Error>(std::string("Missing required file: ") + file + string_empty));
                }
            }
            auto hasTests = result["files"]["some"]([=](auto f) mutable
            {
                return OR((f["path"]->includes(std::string("test"))), (f["path"]->includes(std::string("spec"))));
            }
            );
            if (!hasTests) {
                throw any(std::make_shared<Error>(std::string("No test files found in generated application")));
            }
            console->log(std::string("✅ Complete application functionality validated"));
            console->log(std::string("📁 Generated ") + result["files"]["length"] + std::string(" files total"));
        }
        }
    } };
};

#endif
