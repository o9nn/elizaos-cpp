#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_SCENARIOS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_SCENARIOS_H
#include "core.h"
#include "@elizaos/core.h"

class AutoCoderScenariosTestSuite;

class AutoCoderScenariosTestSuite : public TestSuite, public std::enable_shared_from_this<AutoCoderScenariosTestSuite> {
public:
    using std::enable_shared_from_this<AutoCoderScenariosTestSuite>::shared_from_this;
    string name = std:("autocoder-real-world-scenarios");

    string description = std:("Real-world application generation scenarios using live APIs and runtime");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Generate Tesla News Discord Bot")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🚀 Testing Tesla News Discord Bot generation..."));
            if (!process->env->DISCORD_BOT_TOKEN) {
                console->log(std:("⏭️ Skipping Discord bot test - no DISCORD_BOT_TOKEN"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime["getService"](std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            auto teslaNewsProject = object{
                object::pair{std:("projectName"), std:("tesla-news-discord-bot")}, 
                object::pair{std:("description"), std:("Discord bot that monitors RSS feeds for Tesla news and sends alerts")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Monitor RSS feeds for Tesla-related news"), std:("Send Discord notifications when Tesla news is found"), std:("Filter news by keywords (Tesla, Elon Musk, stock)"), std:("Throttle notifications to avoid spam"), std:("Store seen articles to prevent duplicates"), std:("Support multiple RSS sources"), std:("Include article title, summary, and link in Discord messages") }}, 
                object::pair{std:("apis"), array<string>{ std:("Discord API"), std:("RSS feed parsing"), std:("Web scraping for article content") }}, 
                object::pair{std:("testScenarios"), array<string>{ std:("Parse RSS feed and extract Tesla articles"), std:("Send formatted Discord message"), std:("Handle duplicate article detection"), std:("Process multiple RSS sources"), std:("Throttle notification frequency") }}, 
                object::pair{std:("githubRepo"), std:("tesla-news-test-") + Date->now() + string_empty}
            };
            auto result = std::async([=]() { codeGenService["generateCode"](teslaNewsProject); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std:("Tesla news bot generation failed: ") + result["errors"]["join"](std:(", ")) + string_empty));
            }
            console->log(std:("✅ Tesla news Discord bot generated successfully"));
            if (OR((!result["files"]), (result["files"]["length"] == 0))) {
                throw any(std::make_shared<Error>(std:("No files generated")));
            }
            auto fileNames = result["files"]["map"]([=](auto f) mutable
            {
                return f["path"];
            }
            );
            auto requiredFiles = array<string>{ std:("src/index.ts"), std:("package.json"), std:("README.md") };
            for (auto& file : requiredFiles)
            {
                if (!fileNames["includes"](file)) {
                    throw any(std::make_shared<Error>(std:("Missing required file: ") + file + string_empty));
                }
            }
            auto indexFile = result["files"]["find"]([=](auto f) mutable
            {
                return f["path"] == std:("src/index.ts");
            }
            );
            if (!indexFile) {
                throw any(std::make_shared<Error>(std:("Missing index.ts file")));
            }
            auto content = indexFile["content"]["toLowerCase"]();
            if (OR((!content["includes"](std:("discord"))), (!content["includes"](std:("rss"))))) {
                throw any(std::make_shared<Error>(std:("Generated code missing Discord or RSS functionality")));
            }
            console->log(std:("📁 Generated ") + result["files"]["length"] + std:(" files for Tesla news bot"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Generate Global Weather App")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🌤️ Testing Global Weather App generation..."));
            if (!process->env->OPENWEATHER_API_KEY) {
                console->log(std:("⏭️ Skipping weather app test - no OPENWEATHER_API_KEY"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime["getService"](std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            auto weatherAppProject = object{
                object::pair{std:("projectName"), std:("global-weather-app")}, 
                object::pair{std:("description"), std:("Complete weather application supporting global location queries")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("agent"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Query weather for any location worldwide"), std:("Support city names, coordinates, and postal codes"), std:("Provide current weather and forecasts"), std:("Include weather alerts and warnings"), std:("Support multiple weather data providers"), std:("Cache weather data to reduce API calls"), std:("Handle location disambiguation"), std:("Provide weather maps and radar") }}, 
                object::pair{std:("apis"), array<string>{ std:("OpenWeatherMap API"), std:("WeatherAPI.com"), std:("Location geocoding services"), std:("Time zone APIs") }}, 
                object::pair{std:("testScenarios"), array<string>{ std:("Query weather for major cities"), std:("Handle ambiguous location names"), std:("Provide accurate forecasts"), std:("Display weather alerts"), std:("Cache and retrieve weather data"), std:("Support different units (metric/imperial)") }}, 
                object::pair{std:("githubRepo"), std:("weather-app-test-") + Date->now() + string_empty}, 
                object::pair{std:("personality"), std:("Helpful and informative weather assistant")}, 
                object::pair{std:("knowledge"), array<string>{ std:("Weather patterns and terminology"), std:("Global geography and time zones"), std:("Weather safety and alerts") }}
            };
            auto result = std::async([=]() { codeGenService["generateCode"](weatherAppProject); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std:("Weather app generation failed: ") + result["errors"]["join"](std:(", ")) + string_empty));
            }
            console->log(std:("✅ Global weather app generated successfully"));
            if (OR((!result["files"]), (result["files"]["length"] == 0))) {
                throw any(std::make_shared<Error>(std:("No files generated")));
            }
            auto fileNames = result["files"]["map"]([=](auto f) mutable
            {
                return f["path"];
            }
            );
            auto requiredFiles = array<string>{ std:("src/index.ts"), std:("package.json"), std:("character.json") };
            for (auto& file : requiredFiles)
            {
                if (!fileNames["includes"](file)) {
                    throw any(std::make_shared<Error>(std:("Missing required file: ") + file + string_empty));
                }
            }
            auto indexFile = result["files"]["find"]([=](auto f) mutable
            {
                return f["path"] == std:("src/index.ts");
            }
            );
            if (!indexFile) {
                throw any(std::make_shared<Error>(std:("Missing index.ts file")));
            }
            auto content = indexFile["content"]["toLowerCase"]();
            if (!content["includes"](std:("weather"))) {
                throw any(std::make_shared<Error>(std:("Generated code missing weather functionality")));
            }
            auto characterFile = result["files"]["find"]([=](auto f) mutable
            {
                return f["path"] == std:("character.json");
            }
            );
            if (!characterFile) {
                throw any(std::make_shared<Error>(std:("Missing character.json file for agent")));
            }
            auto character = JSON->parse(characterFile["content"]);
            if (OR((!character["name"]), (!character["bio"]))) {
                throw any(std::make_shared<Error>(std:("Invalid character configuration")));
            }
            console->log(std:("📁 Generated ") + result["files"]["length"] + std:(" files for weather app"));
            if (result["agentId"]) {
                console->log(std:("🤖 Agent ID: ") + result["agentId"] + string_empty);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Validate RSS Monitoring Functionality")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🔍 Testing RSS monitoring capabilities..."));
            if (!process->env->DISCORD_BOT_TOKEN) {
                console->log(std:("⏭️ Skipping RSS monitoring test - no DISCORD_BOT_TOKEN"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime["getService"](std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            auto rssProject = object{
                object::pair{std:("projectName"), std:("rss-monitor-test")}, 
                object::pair{std:("description"), std:("RSS monitoring with keyword filtering")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Parse RSS feeds from multiple sources"), std:("Filter articles by keywords"), std:("Detect duplicate articles"), std:("Schedule periodic RSS checks"), std:("Handle RSS feed errors gracefully") }}, 
                object::pair{std:("apis"), array<string>{ std:("RSS parsing"), std:("Web scraping") }}, 
                object::pair{std:("testScenarios"), array<string>{ std:("Parse valid RSS feed"), std:("Handle invalid RSS feed"), std:("Filter by keywords"), std:("Detect duplicates") }}
            };
            auto result = std::async([=]() { codeGenService["generateCode"](rssProject); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std:("RSS monitoring generation failed: ") + result["errors"]["join"](std:(", ")) + string_empty));
            }
            if (!result["files"]) {
                throw any(std::make_shared<Error>(std:("No files generated")));
            }
            auto rssFiles = result["files"]["filter"]([=](auto f) mutable
            {
                return OR((OR((f["content"]->toLowerCase()->includes(std:("rss"))), (f["content"]->toLowerCase()->includes(std:("feed"))))), (f["content"]->toLowerCase()->includes(std:("xml"))));
            }
            );
            if (rssFiles["length"] == 0) {
                throw any(std::make_shared<Error>(std:("No RSS functionality found in generated code")));
            }
            auto hasKeywordFiltering = result["files"]["some"]([=](auto f) mutable
            {
                return AND((f["content"]->toLowerCase()->includes(std:("filter"))), (f["content"]->toLowerCase()->includes(std:("keyword"))));
            }
            );
            if (!hasKeywordFiltering) {
                throw any(std::make_shared<Error>(std:("Missing keyword filtering functionality")));
            }
            console->log(std:("✅ RSS monitoring functionality verified"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Weather API Integration")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🌍 Testing weather API integration..."));
            if (!process->env->OPENWEATHER_API_KEY) {
                console->log(std:("⏭️ Skipping weather API test - no OPENWEATHER_API_KEY"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime["getService"](std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            auto weatherApiProject = object{
                object::pair{std:("projectName"), std:("weather-api-integration")}, 
                object::pair{std:("description"), std:("Weather API integration with caching and error handling")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Connect to OpenWeatherMap API"), std:("Implement data caching"), std:("Handle API rate limits"), std:("Support multiple location formats"), std:("Provide error recovery") }}, 
                object::pair{std:("apis"), array<string>{ std:("OpenWeatherMap API") }}, 
                object::pair{std:("testScenarios"), array<string>{ std:("Query weather by city name"), std:("Query weather by coordinates"), std:("Handle API errors"), std:("Test caching behavior") }}
            };
            auto result = std::async([=]() { codeGenService["generateCode"](weatherApiProject); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std:("Weather API integration failed: ") + result["errors"]["join"](std:(", ")) + string_empty));
            }
            if (!result["files"]) {
                throw any(std::make_shared<Error>(std:("No files generated")));
            }
            auto apiFiles = result["files"]["filter"]([=](auto f) mutable
            {
                return OR((OR((f["content"]->toLowerCase()->includes(std:("api"))), (f["content"]->toLowerCase()->includes(std:("fetch"))))), (f["content"]->toLowerCase()->includes(std:("request"))));
            }
            );
            if (apiFiles["length"] == 0) {
                throw any(std::make_shared<Error>(std:("No API integration found in generated code")));
            }
            auto hasCaching = result["files"]["some"]([=](auto f) mutable
            {
                return OR((OR((f["content"]->toLowerCase()->includes(std:("cache"))), (f["content"]->toLowerCase()->includes(std:("redis"))))), (f["content"]->toLowerCase()->includes(std:("storage"))));
            }
            );
            if (!hasCaching) {
                throw any(std::make_shared<Error>(std:("Missing caching functionality")));
            }
            auto hasErrorHandling = result["files"]["some"]([=](auto f) mutable
            {
                return OR((OR(((AND((f["content"]->includes(std:("try"))), (f["content"]->includes(std:("catch")))))), (f["content"]->includes(std:("error"))))), (f["content"]->includes(std:("Error"))));
            }
            );
            if (!hasErrorHandling) {
                throw any(std::make_shared<Error>(std:("Missing error handling")));
            }
            console->log(std:("✅ Weather API integration verified"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("End-to-End Application Validation")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🔍 Testing complete application functionality..."));
            if (OR((!process->env->ANTHROPIC_API_KEY), (!process->env->E2B_API_KEY))) {
                console->log(std:("⏭️ Skipping E2E validation - missing API keys"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime["getService"](std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            auto e2eProject = object{
                object::pair{std:("projectName"), std:("e2e-validation-test")}, 
                object::pair{std:("description"), std:("Complete application with comprehensive validation")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("agent"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Multi-step conversation flow"), std:("External API integration"), std:("Data persistence"), std:("Error handling"), std:("Comprehensive testing") }}, 
                object::pair{std:("apis"), array<string>{ std:("Mock external API") }}, 
                object::pair{std:("testScenarios"), array<string>{ std:("Complete conversation flow"), std:("API integration test"), std:("Data persistence test"), std:("Error recovery test"), std:("Performance test") }}, 
                object::pair{std:("personality"), std:("Professional and helpful assistant")}, 
                object::pair{std:("knowledge"), array<string>{ std:("Domain-specific knowledge base") }}
            };
            auto result = std::async([=]() { codeGenService["generateCode"](e2eProject); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std:("E2E validation failed: ") + result["errors"]["join"](std:(", ")) + string_empty));
            }
            if (OR((!result["files"]), (result["files"]["length"] <= 5))) {
                throw any(std::make_shared<Error>(std:("Insufficient files generated for complete application")));
            }
            auto fileNames = result["files"]["map"]([=](auto f) mutable
            {
                return f["path"];
            }
            );
            auto requiredFiles = array<string>{ std:("src/index.ts"), std:("package.json"), std:("character.json"), std:("README.md") };
            for (auto& file : requiredFiles)
            {
                if (!fileNames["includes"](file)) {
                    throw any(std::make_shared<Error>(std:("Missing required file: ") + file + string_empty));
                }
            }
            auto hasTests = result["files"]["some"]([=](auto f) mutable
            {
                return OR((f["path"]->includes(std:("test"))), (f["path"]->includes(std:("spec"))));
            }
            );
            if (!hasTests) {
                throw any(std::make_shared<Error>(std:("No test files found in generated application")));
            }
            console->log(std:("✅ Complete application functionality validated"));
            console->log(std:("📁 Generated ") + result["files"]["length"] + std:(" files total"));
        }
        }
    } };
};

#endif
