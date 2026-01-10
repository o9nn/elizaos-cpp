#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_CLAUDE-CODE-STRESS-TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_CLAUDE-CODE-STRESS-TEST_H
#include "core.h"
#include "@elizaos/core.h"
#include "../../services/CodeGenerationService.h"

class ClaudeCodeStressTestSuite;

class ClaudeCodeStressTestSuite : public TestSuite, public std::enable_shared_from_this<ClaudeCodeStressTestSuite> {
public:
    using std::enable_shared_from_this<ClaudeCodeStressTestSuite>::shared_from_this;
    string name = std::string("code-generation-stress-test");

    string description = std::string("Stress tests for code generation under various conditions");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("should handle multiple concurrent code generation requests")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🚀 Testing concurrent code generation requests..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std::string("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std::string("⏭️ Skipping concurrent test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            shared codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            auto concurrentRequests = 3;
            auto requests = array<any>();
            for (shared i = 0; i < concurrentRequests; i++)
            {
                auto request = ([=]() mutable
                {
                    auto startTime = Date->now();
                    try
                    {
                        auto result = std::async([=]() { codeGenService->generateCode(object{
                            object::pair{std::string("projectName"), std::string("concurrent-test-") + i + string_empty}, 
                            object::pair{std::string("description"), std::string("Generate a simple function called "test") + i + std::string("" that returns the number ") + i + string_empty}, 
                            object::pair{std::string("targetType"), std::string("plugin")}, 
                            object::pair{std::string("requirements"), array<string>{ std::string("Function test") + i + std::string(" that returns ") + i + string_empty }}, 
                            object::pair{std::string("apis"), array<any>()}
                        }); });
                        auto duration = Date->now() - startTime;
                        return object{
                            object::pair{std::string("success"), result->success}, 
                            object::pair{std::string("response"), result}, 
                            object::pair{std::string("duration"), std::string("duration")}, 
                            object::pair{std::string("index"), i}
                        };
                    }
                    catch (const any& error)
                    {
                        auto duration = Date->now() - startTime;
                        return object{
                            object::pair{std::string("success"), false}, 
                            object::pair{std::string("error"), (as<std::shared_ptr<Error>>(error))->message}, 
                            object::pair{std::string("duration"), std::string("duration")}, 
                            object::pair{std::string("index"), i}
                        };
                    }
                }
                )();
                requests->push(request);
            }
            auto results = std::async([=]() { Promise->all(requests); });
            auto successful = results->filter([=](auto r) mutable
            {
                return r["success"];
            }
            );
            auto failed = results->filter([=](auto r) mutable
            {
                return !r["success"];
            }
            );
            console->log(std::string("✅ Concurrent requests completed: ") + successful->get_length() + std::string("/") + concurrentRequests + std::string(" successful"));
            if (failed->get_length() > 0) {
                console->log(std::string("❌ Failed requests: ") + failed->get_length() + string_empty);
                failed->forEach([=](auto f) mutable
                {
                    return console->log(std::string("  Request ") + f["index"] + std::string(": ") + f["error"] + string_empty);
                }
                );
            }
            if (successful->get_length() == 0) {
                throw any(std::make_shared<Error>(std::string("All concurrent requests failed")));
            }
            auto& __array2963_3244 = successful;
            for (auto __indx2963_3244 = 0_N; __indx2963_3244 < __array2963_3244->get_length(); __indx2963_3244++)
            {
                auto& result = const_(__array2963_3244)[__indx2963_3244];
                {
                    if (OR((OR((!result["response"]), (!result["response"]["files"]))), (result["response"]["files"]["length"] == 0))) {
                        throw any(std::make_shared<Error>(std::string("Request ") + result["index"] + std::string(" did not generate any files")));
                    }
                }
            }
            auto avgDuration = successful->reduce([=](auto sum, auto r) mutable
            {
                return sum + r["duration"];
            }
            , 0) / successful->get_length();
            console->log(std::string("📊 Average response time: ") + avgDuration->toFixed(0) + std::string("ms"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle large context prompts")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("📚 Testing large context prompt handling..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std::string("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std::string("⏭️ Skipping large context test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            try
            {
                auto requirements = Array->from(object{
                    object::pair{std::string("length"), 50}
                }, [=](auto _, auto i) mutable
                {
                    return std::string("Feature ") + (i + 1) + std::string(": Advanced functionality for e-commerce system");
                }
                );
                auto apis = Array->from(object{
                    object::pair{std::string("length"), 20}
                }, [=](auto _, auto i) mutable
                {
                    return std::string("API Endpoint ") + (i + 1) + std::string(" - Complex e-commerce operations");
                }
                );
                auto startTime = Date->now();
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std::string("projectName"), std::string("large-context-test")}, 
                    object::pair{std::string("description"), std::string("Comprehensive ElizaOS plugin for an e-commerce platform with extensive features")}, 
                    object::pair{std::string("targetType"), std::string("plugin")}, 
                    object::pair{std::string("requirements"), std::string("requirements")}, 
                    object::pair{std::string("apis"), std::string("apis")}, 
                    object::pair{std::string("testScenarios"), array<string>{ std::string("Test basic functionality"), std::string("Test API integrations") }}
                }); });
                auto duration = Date->now() - startTime;
                if (!result->success) {
                    throw any(std::make_shared<Error>(std::string("Large context generation failed: ") + result->errors->join(std::string(", ")) + string_empty));
                }
                if (OR((!result->files), (result->files->length == 0))) {
                    throw any(std::make_shared<Error>(std::string("No files generated for large context")));
                }
                auto allContent = result->files->map([=](auto f) mutable
                {
                    return f["content"];
                }
                )->join(std::string("\
"))->toLowerCase();
                if (OR((!allContent->includes(std::string("plugin"))), (!allContent->includes(std::string("action"))))) {
                    throw any(std::make_shared<Error>(std::string("Generated code does not contain expected plugin structure")));
                }
                console->log(std::string("✅ Large context handled successfully in ") + duration + std::string("ms"));
                console->log(std::string("📏 Requirements: ") + requirements->get_length() + std::string(", APIs: ") + apis->get_length() + std::string(", Files generated: ") + result->files->length + string_empty);
            }
            catch (const any& error)
            {
                console->error(std::string("❌ Large context test failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle rapid sequential requests")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("⚡ Testing rapid sequential Claude Code requests..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std::string("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std::string("⏭️ Skipping rapid sequential test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            auto requestCount = 5;
            auto results = array<any>();
            for (auto i = 0; i < requestCount; i++)
            {
                auto startTime = Date->now();
                try
                {
                    auto result = std::async([=]() { codeGenService->generateCode(object{
                        object::pair{std::string("projectName"), std::string("sequential-test-") + i + string_empty}, 
                        object::pair{std::string("description"), std::string("Create a simple TypeScript interface called "Interface") + i + std::string("" with a property "value") + i + std::string(""")}, 
                        object::pair{std::string("targetType"), std::string("plugin")}, 
                        object::pair{std::string("requirements"), array<string>{ std::string("Interface Interface") + i + std::string(" with property value") + i + string_empty }}, 
                        object::pair{std::string("apis"), array<any>()}
                    }); });
                    auto duration = Date->now() - startTime;
                    results->push(object{
                        object::pair{std::string("success"), result->success}, 
                        object::pair{std::string("response"), result}, 
                        object::pair{std::string("duration"), std::string("duration")}, 
                        object::pair{std::string("index"), i}
                    });
                    console->log(std::string("✅ Request ") + (i + 1) + std::string("/") + requestCount + std::string(" completed in ") + duration + std::string("ms"));
                }
                catch (const any& error)
                {
                    auto duration = Date->now() - startTime;
                    results->push(object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), (as<std::shared_ptr<Error>>(error))->message}, 
                        object::pair{std::string("duration"), std::string("duration")}, 
                        object::pair{std::string("index"), i}
                    });
                    console->log(std::string("❌ Request ") + (i + 1) + std::string("/") + requestCount + std::string(" failed in ") + duration + std::string("ms"));
                }
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 100);
                }
                ); });
            }
            auto successful = results->filter([=](auto r) mutable
            {
                return r["success"];
            }
            );
            auto failed = results->filter([=](auto r) mutable
            {
                return !r["success"];
            }
            );
            console->log(std::string("📊 Sequential requests: ") + successful->get_length() + std::string("/") + requestCount + std::string(" successful"));
            if (successful->get_length() == 0) {
                throw any(std::make_shared<Error>(std::string("All sequential requests failed")));
            }
            auto& __array8263_8545 = successful;
            for (auto __indx8263_8545 = 0_N; __indx8263_8545 < __array8263_8545->get_length(); __indx8263_8545++)
            {
                auto& result = const_(__array8263_8545)[__indx8263_8545];
                {
                    if (OR((OR((!result["response"]), (!result["response"]["files"]))), (result["response"]["files"]["length"] == 0))) {
                        throw any(std::make_shared<Error>(std::string("Request ") + result["index"] + std::string(" did not generate any files")));
                    }
                }
            }
            auto avgDuration = successful->reduce([=](auto sum, auto r) mutable
            {
                return sum + r["duration"];
            }
            , 0) / successful->get_length();
            console->log(std::string("⚡ Average response time: ") + avgDuration->toFixed(0) + std::string("ms"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle complex multi-file project generation")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🏗️ Testing complex multi-file project generation..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std::string("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std::string("⏭️ Skipping complex project test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            try
            {
                auto startTime = Date->now();
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std::string("projectName"), std::string("complex-multi-feature-plugin")}, 
                    object::pair{std::string("description"), std::string("A comprehensive plugin with multiple advanced features and integrations")}, 
                    object::pair{std::string("targetType"), std::string("plugin")}, 
                    object::pair{std::string("requirements"), array<string>{ std::string("Multiple actions for different operations"), std::string("Advanced providers for context enrichment"), std::string("Service integrations with external APIs"), std::string("Comprehensive error handling and logging"), std::string("Caching and performance optimization"), std::string("Security features and input validation"), std::string("Comprehensive test coverage"), std::string("Documentation and examples"), std::string("Configuration management"), std::string("Health monitoring and metrics") }}, 
                    object::pair{std::string("apis"), array<string>{ std::string("OpenAI API"), std::string("Discord API"), std::string("GitHub API"), std::string("Redis for caching"), std::string("PostgreSQL for data storage") }}, 
                    object::pair{std::string("testScenarios"), array<string>{ std::string("Test all action functionalities"), std::string("Test provider integrations"), std::string("Test error handling scenarios"), std::string("Test performance under load"), std::string("Test security validations"), std::string("Test configuration management"), std::string("Test caching mechanisms"), std::string("Test database operations"), std::string("Test API integrations"), std::string("Test monitoring and metrics") }}
                }); });
                auto duration = Date->now() - startTime;
                if (!result->success) {
                    throw any(std::make_shared<Error>(std::string("Complex project generation failed: ") + result->errors->join(std::string(", ")) + string_empty));
                }
                if (OR((!result->files), (result->files->length == 0))) {
                    throw any(std::make_shared<Error>(std::string("No files generated for complex project")));
                }
                console->log(std::string("✅ Complex project generated in ") + duration + std::string("ms"));
                console->log(std::string("📁 Generated ") + result->files->length + std::string(" files"));
                auto fileNames = result->files->map([=](auto f) mutable
                {
                    return f["path"];
                }
                );
                auto requiredFiles = array<string>{ std::string("package.json"), std::string("src/index.ts"), std::string("tsconfig.json"), std::string("README.md") };
                for (auto& file : requiredFiles)
                {
                    if (!fileNames->includes(file)) {
                        throw any(std::make_shared<Error>(std::string("Missing required file: ") + file + string_empty));
                    }
                }
                auto totalLines = result->files->reduce([=](auto sum, auto file) mutable
                {
                    return sum + file["content"]->split(std::string("\
"))->length;
                }
                , 0);
                auto averageFileSize = result->files->reduce([=](auto sum, auto file) mutable
                {
                    return sum + file["content"]->length;
                }
                , 0) / result->files->length;
                console->log(std::string("📊 Project complexity:"));
                console->log(std::string("  Total lines: ") + totalLines + string_empty);
                console->log(std::string("  Average file size: ") + averageFileSize->toFixed(0) + std::string(" chars"));
                console->log(std::string("  Files generated: ") + result->files->length + string_empty);
                auto allContent = result->files->map([=](auto f) mutable
                {
                    return f["content"];
                }
                )->join(std::string("\
"))->toLowerCase();
                auto features = array<string>{ std::string("action"), std::string("provider"), std::string("service"), std::string("error"), std::string("cache"), std::string("test"), std::string("async"), std::string("interface"), std::string("export") };
                auto featuresFound = 0;
                for (auto& feature : features)
                {
                    if (allContent->includes(feature)) {
                        featuresFound++;
                    }
                }
                if (featuresFound < features->get_length() * 0.7) {
                    throw any(std::make_shared<Error>(std::string("Only ") + featuresFound + std::string("/") + features->get_length() + std::string(" expected features found in generated code")));
                }
                console->log(std::string("✅ Advanced features found: ") + featuresFound + std::string("/") + features->get_length() + string_empty);
                if (result->executionResults) {
                    console->log(std::string("📋 QA Results:"));
                    console->log(std::string("  Lint: ") + (result->executionResults->lintPass) ? std::string("✅") : std::string("❌") + string_empty);
                    console->log(std::string("  Types: ") + (result->executionResults->typesPass) ? std::string("✅") : std::string("❌") + string_empty);
                    console->log(std::string("  Tests: ") + (result->executionResults->testsPass) ? std::string("✅") : std::string("❌") + string_empty);
                    console->log(std::string("  Build: ") + (result->executionResults->buildPass) ? std::string("✅") : std::string("❌") + string_empty);
                }
            }
            catch (const any& error)
            {
                console->error(std::string("❌ Complex project generation failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should handle error recovery and retries")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🔄 Testing error recovery and retry mechanisms..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            if (!anthropicKey) {
                console->log(std::string("⏭️ Skipping error recovery test - no ANTHROPIC_API_KEY"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            try
            {
                auto problemRequest = object{
                    object::pair{std::string("projectName"), std::string("error-recovery-test")}, 
                    object::pair{std::string("description"), std::string("Test plugin designed to trigger potential errors and recovery")}, 
                    object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                    object::pair{std::string("requirements"), array<string>{ std::string("Handle edge cases and error conditions"), std::string("Implement robust error recovery"), std::string("Test retry mechanisms"), std::string("Validate error handling paths") }}, 
                    object::pair{std::string("apis"), array<string>{ std::string("Potentially unreliable API") }}, 
                    object::pair{std::string("testScenarios"), array<string>{ std::string("Test error scenarios"), std::string("Test recovery mechanisms"), std::string("Test retry logic") }}
                };
                auto startTime = Date->now();
                auto result = std::async([=]() { codeGenService->generateCode(problemRequest); });
                auto duration = Date->now() - startTime;
                if (result->success) {
                    console->log(std::string("✅ Error recovery test passed - generation succeeded in ") + duration + std::string("ms"));
                    if (AND((result->files), (result->files->length > 0))) {
                        console->log(std::string("📁 Generated ") + result->files->length + std::string(" files despite potential errors"));
                    }
                } else {
                    console->log(std::string("✅ Error recovery test passed - graceful failure in ") + duration + std::string("ms"));
                    if (AND((result->errors), (result->errors->length > 0))) {
                        console->log(std::string("📋 Errors handled gracefully: ") + result->errors->length + std::string(" errors"));
                    }
                }
                auto secondResult = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std::string("projectName"), std::string("post-error-test")}, 
                    object::pair{std::string("description"), std::string("Simple test after potential error")}, 
                    object::pair{std::string("targetType"), std::string("plugin")}, 
                    object::pair{std::string("requirements"), array<string>{ std::string("Basic functionality") }}, 
                    object::pair{std::string("apis"), array<any>()}
                }); });
                if (secondResult->success) {
                    console->log(std::string("✅ Service recovered successfully after error scenario"));
                } else {
                    console->log(std::string("⚠️ Service state affected by previous error, but handled gracefully"));
                }
            }
            catch (const any& error)
            {
                console->log(std::string("🔄 Testing service stability after error..."));
                try
                {
                    auto recoveryResult = std::async([=]() { codeGenService->generateCode(object{
                        object::pair{std::string("projectName"), std::string("recovery-test")}, 
                        object::pair{std::string("description"), std::string("Test service recovery")}, 
                        object::pair{std::string("targetType"), std::string("plugin")}, 
                        object::pair{std::string("requirements"), array<string>{ std::string("Basic test") }}, 
                        object::pair{std::string("apis"), array<any>()}
                    }); });
                    if (recoveryResult->success) {
                        console->log(std::string("✅ Service recovered successfully despite error"));
                    } else {
                        console->log(std::string("✅ Service remained stable despite error"));
                    }
                }
                catch (const any& recoveryError)
                {
                    console->error(std::string("❌ Service failed to recover:"), recoveryError);
                    throw any(recoveryError);
                }
                if (AND((!(as<std::shared_ptr<Error>>(error))->message->includes(std::string("timeout"))), (!(as<std::shared_ptr<Error>>(error))->message->includes(std::string("rate limit"))))) {
                    throw any(error);
                } else {
                    console->log(std::string("✅ Expected error handled correctly"));
                }
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should maintain performance under sustained load")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("📈 Testing sustained load performance..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            if (!anthropicKey) {
                console->log(std::string("⏭️ Skipping sustained load test - no ANTHROPIC_API_KEY"));
                return std::shared_ptr<Promise<void>>();
            }
            process->env->ANTHROPIC_API_KEY = anthropicKey;
            shared loadTestDuration = 30000;
            auto requestInterval = 2000;
            shared startTime = Date->now();
            shared results = array<object>();
            console->log(std::string("🔄 Running sustained load test for ") + (loadTestDuration / 1000) + std::string(" seconds..."));
            shared requestCount = 0;
            shared intervalId = setInterval([=]() mutable
            {
                if (Date->now() - startTime > loadTestDuration) {
                    clearInterval(intervalId);
                    return std::shared_ptr<Promise<void>>();
                }
                requestCount++;
                auto requestStartTime = Date->now();
                try
                {
                    auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
                    if (!codeGenService) {
                        throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
                    }
                    auto result = std::async([=]() { codeGenService->generateCode(object{
                        object::pair{std::string("projectName"), std::string("load-test-") + requestCount + string_empty}, 
                        object::pair{std::string("description"), std::string("Create a simple function called "loadTest") + requestCount + std::string("" that returns "Load test ") + requestCount + std::string("".")}, 
                        object::pair{std::string("targetType"), std::string("plugin")}, 
                        object::pair{std::string("requirements"), array<string>{ std::string("Function loadTest") + requestCount + std::string(" returning "Load test ") + requestCount + std::string(""") }}, 
                        object::pair{std::string("apis"), array<any>()}
                    }); });
                    auto duration = Date->now() - requestStartTime;
                    results->push(object{
                        object::pair{std::string("success"), result->success}, 
                        object::pair{std::string("duration"), std::string("duration")}, 
                        object::pair{std::string("requestCount"), std::string("requestCount")}
                    });
                    console->log(std::string("✅ Load test request ") + requestCount + std::string(" completed in ") + duration + std::string("ms"));
                }
                catch (const any& error)
                {
                    auto duration = Date->now() - requestStartTime;
                    results->push(object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("duration"), std::string("duration")}, 
                        object::pair{std::string("requestCount"), std::string("requestCount")}, 
                        object::pair{std::string("error"), (as<std::shared_ptr<Error>>(error))->message}
                    });
                    console->log(std::string("❌ Load test request ") + requestCount + std::string(" failed in ") + duration + std::string("ms"));
                }
            }
            , requestInterval);
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                shared checkInterval = setInterval([=]() mutable
                {
                    if (Date->now() - startTime > loadTestDuration + 5000) {
                        clearInterval(checkInterval);
                        resolve(undefined);
                    }
                }
                , 1000);
            }
            ); });
            auto successful = results->filter([=](auto r) mutable
            {
                return r["success"];
            }
            );
            auto failed = results->filter([=](auto r) mutable
            {
                return !r["success"];
            }
            );
            console->log(std::string("📊 Sustained load test results:"));
            console->log(std::string("  Total requests: ") + results->get_length() + string_empty);
            console->log(std::string("  Successful: ") + successful->get_length() + string_empty);
            console->log(std::string("  Failed: ") + failed->get_length() + string_empty);
            console->log(std::string("  Success rate: ") + ((successful->get_length() / results->get_length()) * 100)->toFixed(1) + std::string("%"));
            if (successful->get_length() > 0) {
                auto avgDuration = successful->reduce([=](auto sum, auto r) mutable
                {
                    return sum + r["duration"];
                }
                , 0) / successful->get_length();
                auto maxDuration = Math->max(const_(successful->map([=](auto r) mutable
                {
                    return r["duration"];
                }
                ))[0]);
                auto minDuration = Math->min(const_(successful->map([=](auto r) mutable
                {
                    return r["duration"];
                }
                ))[0]);
                console->log(std::string("  Average response time: ") + avgDuration->toFixed(0) + std::string("ms"));
                console->log(std::string("  Max response time: ") + maxDuration + std::string("ms"));
                console->log(std::string("  Min response time: ") + minDuration + std::string("ms"));
            }
            if (successful->get_length() == 0) {
                throw any(std::make_shared<Error>(std::string("No successful requests during sustained load test")));
            }
            auto successRate = successful->get_length() / results->get_length();
            if (successRate < 0.5) {
                throw any(std::make_shared<Error>(std::string("Success rate too low: ") + (successRate * 100)->toFixed(1) + std::string("%")));
            }
            console->log(std::string("✅ Sustained load test completed successfully"));
        }
        }
    } };
};

#endif
