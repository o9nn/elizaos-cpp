#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_AUTOCODER_SRC___TESTS___E2E_CLAUDE_CODE_STRESS_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_AUTOCODER_SRC___TESTS___E2E_CLAUDE_CODE_STRESS_TEST_H
#include "core.hpp"
// External dependency removed
#include "../../services/CodeGenerationService.h"

class ClaudeCodeStressTestSuite;

class ClaudeCodeStressTestSuite : public TestSuite, public std::enable_shared_from_this<ClaudeCodeStressTestSuite> {
public:
    using std::enable_shared_from_this<ClaudeCodeStressTestSuite>::shared_from_this;
    string name = std:("code-generation-stress-test");

    string description = std:("Stress tests for code generation under various conditions");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("should handle multiple concurrent code generation requests")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🚀 Testing concurrent code generation requests..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std:("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std:("⏭️ Skipping concurrent test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            shared codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
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
                            object::pair{std:("projectName"), std:("concurrent-test-") + i + string_empty}, 
                            object::pair{std:("description"), std:("Generate a simple function called "test") + i + std:("" that returns the number ") + i + string_empty}, 
                            object::pair{std:("targetType"), std:("plugin")}, 
                            object::pair{std:("requirements"), array<string>{ std:("Function test") + i + std:(" that returns ") + i + string_empty }}, 
                            object::pair{std:("apis"), array<any>()}
                        }); });
                        auto duration = Date->now() - startTime;
                        return object{
                            object::pair{std:("success"), result->success}, 
                            object::pair{std:("response"), result}, 
                            object::pair{std:("duration"), std:("duration")}, 
                            object::pair{std:("index"), i}
                        };
                    }
                    catch (const any& error)
                    {
                        auto duration = Date->now() - startTime;
                        return object{
                            object::pair{std:("success"), false}, 
                            object::pair{std:("error"), (as<std::shared_ptr<Error>>(error))->message}, 
                            object::pair{std:("duration"), std:("duration")}, 
                            object::pair{std:("index"), i}
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
            console->log(std:("✅ Concurrent requests completed: ") + successful->get_length() + std:("/") + concurrentRequests + std:(" successful"));
            if (failed->get_length() > 0) {
                console->log(std:("❌ Failed requests: ") + failed->get_length() + string_empty);
                failed->forEach([=](auto f) mutable
                {
                    return console->log(std:("  Request ") + f["index"] + std:(": ") + f["error"] + string_empty);
                }
                );
            }
            if (successful->get_length() == 0) {
                throw any(std::make_shared<Error>(std:("All concurrent requests failed")));
            }
            auto& __array2963_3244 = successful;
            for (auto __indx2963_3244 = 0_N; __indx2963_3244 < __array2963_3244->get_length(); __indx2963_3244++)
            {
                auto& result = const_(__array2963_3244)[__indx2963_3244];
                {
                    if (OR((OR((!result["response"]), (!result["response"]["files"]))), (result["response"]["files"]["length"] == 0))) {
                        throw any(std::make_shared<Error>(std:("Request ") + result["index"] + std:(" did not generate any files")));
                    }
                }
            }
            auto avgDuration = successful->reduce([=](auto sum, auto r) mutable
            {
                return sum + r["duration"];
            }
            , 0) / successful->get_length();
            console->log(std:("📊 Average response time: ") + avgDuration->toFixed(0) + std:("ms"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle large context prompts")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("📚 Testing large context prompt handling..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std:("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std:("⏭️ Skipping large context test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            try
            {
                auto requirements = Array->from(object{
                    object::pair{std:("length"), 50}
                }, [=](auto _, auto i) mutable
                {
                    return std:("Feature ") + (i + 1) + std:(": Advanced functionality for e-commerce system");
                }
                );
                auto apis = Array->from(object{
                    object::pair{std:("length"), 20}
                }, [=](auto _, auto i) mutable
                {
                    return std:("API Endpoint ") + (i + 1) + std:(" - Complex e-commerce operations");
                }
                );
                auto startTime = Date->now();
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std:("projectName"), std:("large-context-test")}, 
                    object::pair{std:("description"), std:("Comprehensive ElizaOS plugin for an e-commerce platform with extensive features")}, 
                    object::pair{std:("targetType"), std:("plugin")}, 
                    object::pair{std:("requirements"), std:("requirements")}, 
                    object::pair{std:("apis"), std:("apis")}, 
                    object::pair{std:("testScenarios"), array<string>{ std:("Test basic functionality"), std:("Test API integrations") }}
                }); });
                auto duration = Date->now() - startTime;
                if (!result->success) {
                    throw any(std::make_shared<Error>(std:("Large context generation failed: ") + result->errors->join(std:(", ")) + string_empty));
                }
                if (OR((!result->files), (result->files->length == 0))) {
                    throw any(std::make_shared<Error>(std:("No files generated for large context")));
                }
                auto allContent = result->files->map([=](auto f) mutable
                {
                    return f["content"];
                }
                )->join(std:("\
"))->toLowerCase();
                if (OR((!allContent->includes(std:("plugin"))), (!allContent->includes(std:("action"))))) {
                    throw any(std::make_shared<Error>(std:("Generated code does not contain expected plugin structure")));
                }
                console->log(std:("✅ Large context handled successfully in ") + duration + std:("ms"));
                console->log(std:("📏 Requirements: ") + requirements->get_length() + std:(", APIs: ") + apis->get_length() + std:(", Files generated: ") + result->files->length + string_empty);
            }
            catch (const any& error)
            {
                console->error(std:("❌ Large context test failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle rapid sequential requests")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("⚡ Testing rapid sequential Claude Code requests..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std:("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std:("⏭️ Skipping rapid sequential test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            auto requestCount = 5;
            auto results = array<any>();
            for (auto i = 0; i < requestCount; i++)
            {
                auto startTime = Date->now();
                try
                {
                    auto result = std::async([=]() { codeGenService->generateCode(object{
                        object::pair{std:("projectName"), std:("sequential-test-") + i + string_empty}, 
                        object::pair{std:("description"), std:("Create a simple TypeScript interface called "Interface") + i + std:("" with a property "value") + i + std:(""")}, 
                        object::pair{std:("targetType"), std:("plugin")}, 
                        object::pair{std:("requirements"), array<string>{ std:("Interface Interface") + i + std:(" with property value") + i + string_empty }}, 
                        object::pair{std:("apis"), array<any>()}
                    }); });
                    auto duration = Date->now() - startTime;
                    results->push(object{
                        object::pair{std:("success"), result->success}, 
                        object::pair{std:("response"), result}, 
                        object::pair{std:("duration"), std:("duration")}, 
                        object::pair{std:("index"), i}
                    });
                    console->log(std:("✅ Request ") + (i + 1) + std:("/") + requestCount + std:(" completed in ") + duration + std:("ms"));
                }
                catch (const any& error)
                {
                    auto duration = Date->now() - startTime;
                    results->push(object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), (as<std::shared_ptr<Error>>(error))->message}, 
                        object::pair{std:("duration"), std:("duration")}, 
                        object::pair{std:("index"), i}
                    });
                    console->log(std:("❌ Request ") + (i + 1) + std:("/") + requestCount + std:(" failed in ") + duration + std:("ms"));
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
            console->log(std:("📊 Sequential requests: ") + successful->get_length() + std:("/") + requestCount + std:(" successful"));
            if (successful->get_length() == 0) {
                throw any(std::make_shared<Error>(std:("All sequential requests failed")));
            }
            auto& __array8263_8545 = successful;
            for (auto __indx8263_8545 = 0_N; __indx8263_8545 < __array8263_8545->get_length(); __indx8263_8545++)
            {
                auto& result = const_(__array8263_8545)[__indx8263_8545];
                {
                    if (OR((OR((!result["response"]), (!result["response"]["files"]))), (result["response"]["files"]["length"] == 0))) {
                        throw any(std::make_shared<Error>(std:("Request ") + result["index"] + std:(" did not generate any files")));
                    }
                }
            }
            auto avgDuration = successful->reduce([=](auto sum, auto r) mutable
            {
                return sum + r["duration"];
            }
            , 0) / successful->get_length();
            console->log(std:("⚡ Average response time: ") + avgDuration->toFixed(0) + std:("ms"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle complex multi-file project generation")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🏗️ Testing complex multi-file project generation..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std:("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std:("⏭️ Skipping complex project test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            try
            {
                auto startTime = Date->now();
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std:("projectName"), std:("complex-multi-feature-plugin")}, 
                    object::pair{std:("description"), std:("A comprehensive plugin with multiple advanced features and integrations")}, 
                    object::pair{std:("targetType"), std:("plugin")}, 
                    object::pair{std:("requirements"), array<string>{ std:("Multiple actions for different operations"), std:("Advanced providers for context enrichment"), std:("Service integrations with external APIs"), std:("Comprehensive error handling and logging"), std:("Caching and performance optimization"), std:("Security features and input validation"), std:("Comprehensive test coverage"), std:("Documentation and examples"), std:("Configuration management"), std:("Health monitoring and metrics") }}, 
                    object::pair{std:("apis"), array<string>{ std:("OpenAI API"), std:("Discord API"), std:("GitHub API"), std:("Redis for caching"), std:("PostgreSQL for data storage") }}, 
                    object::pair{std:("testScenarios"), array<string>{ std:("Test all action functionalities"), std:("Test provider integrations"), std:("Test error handling scenarios"), std:("Test performance under load"), std:("Test security validations"), std:("Test configuration management"), std:("Test caching mechanisms"), std:("Test database operations"), std:("Test API integrations"), std:("Test monitoring and metrics") }}
                }); });
                auto duration = Date->now() - startTime;
                if (!result->success) {
                    throw any(std::make_shared<Error>(std:("Complex project generation failed: ") + result->errors->join(std:(", ")) + string_empty));
                }
                if (OR((!result->files), (result->files->length == 0))) {
                    throw any(std::make_shared<Error>(std:("No files generated for complex project")));
                }
                console->log(std:("✅ Complex project generated in ") + duration + std:("ms"));
                console->log(std:("📁 Generated ") + result->files->length + std:(" files"));
                auto fileNames = result->files->map([=](auto f) mutable
                {
                    return f["path"];
                }
                );
                auto requiredFiles = array<string>{ std:("package.json"), std:("src/index.ts"), std:("tsconfig.json"), std:("README.md") };
                for (auto& file : requiredFiles)
                {
                    if (!fileNames->includes(file)) {
                        throw any(std::make_shared<Error>(std:("Missing required file: ") + file + string_empty));
                    }
                }
                auto totalLines = result->files->reduce([=](auto sum, auto file) mutable
                {
                    return sum + file["content"]->split(std:("\
"))->length;
                }
                , 0);
                auto averageFileSize = result->files->reduce([=](auto sum, auto file) mutable
                {
                    return sum + file["content"]->length;
                }
                , 0) / result->files->length;
                console->log(std:("📊 Project complexity:"));
                console->log(std:("  Total lines: ") + totalLines + string_empty);
                console->log(std:("  Average file size: ") + averageFileSize->toFixed(0) + std:(" chars"));
                console->log(std:("  Files generated: ") + result->files->length + string_empty);
                auto allContent = result->files->map([=](auto f) mutable
                {
                    return f["content"];
                }
                )->join(std:("\
"))->toLowerCase();
                auto features = array<string>{ std:("action"), std:("provider"), std:("service"), std:("error"), std:("cache"), std:("test"), std:("async"), std:("interface"), std:("export") };
                auto featuresFound = 0;
                for (auto& feature : features)
                {
                    if (allContent->includes(feature)) {
                        featuresFound++;
                    }
                }
                if (featuresFound < features->get_length() * 0.7) {
                    throw any(std::make_shared<Error>(std:("Only ") + featuresFound + std:("/") + features->get_length() + std:(" expected features found in generated code")));
                }
                console->log(std:("✅ Advanced features found: ") + featuresFound + std:("/") + features->get_length() + string_empty);
                if (result->executionResults) {
                    console->log(std:("📋 QA Results:"));
                    console->log(std:("  Lint: ") + (result->executionResults->lintPass) ? std:("✅") : std:("❌") + string_empty);
                    console->log(std:("  Types: ") + (result->executionResults->typesPass) ? std:("✅") : std:("❌") + string_empty);
                    console->log(std:("  Tests: ") + (result->executionResults->testsPass) ? std:("✅") : std:("❌") + string_empty);
                    console->log(std:("  Build: ") + (result->executionResults->buildPass) ? std:("✅") : std:("❌") + string_empty);
                }
            }
            catch (const any& error)
            {
                console->error(std:("❌ Complex project generation failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("should handle error recovery and retries")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🔄 Testing error recovery and retry mechanisms..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            if (!anthropicKey) {
                console->log(std:("⏭️ Skipping error recovery test - no ANTHROPIC_API_KEY"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            try
            {
                auto problemRequest = object{
                    object::pair{std:("projectName"), std:("error-recovery-test")}, 
                    object::pair{std:("description"), std:("Test plugin designed to trigger potential errors and recovery")}, 
                    object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                    object::pair{std:("requirements"), array<string>{ std:("Handle edge cases and error conditions"), std:("Implement robust error recovery"), std:("Test retry mechanisms"), std:("Validate error handling paths") }}, 
                    object::pair{std:("apis"), array<string>{ std:("Potentially unreliable API") }}, 
                    object::pair{std:("testScenarios"), array<string>{ std:("Test error scenarios"), std:("Test recovery mechanisms"), std:("Test retry logic") }}
                };
                auto startTime = Date->now();
                auto result = std::async([=]() { codeGenService->generateCode(problemRequest); });
                auto duration = Date->now() - startTime;
                if (result->success) {
                    console->log(std:("✅ Error recovery test passed - generation succeeded in ") + duration + std:("ms"));
                    if (AND((result->files), (result->files->length > 0))) {
                        console->log(std:("📁 Generated ") + result->files->length + std:(" files despite potential errors"));
                    }
                } else {
                    console->log(std:("✅ Error recovery test passed - graceful failure in ") + duration + std:("ms"));
                    if (AND((result->errors), (result->errors->length > 0))) {
                        console->log(std:("📋 Errors handled gracefully: ") + result->errors->length + std:(" errors"));
                    }
                }
                auto secondResult = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std:("projectName"), std:("post-error-test")}, 
                    object::pair{std:("description"), std:("Simple test after potential error")}, 
                    object::pair{std:("targetType"), std:("plugin")}, 
                    object::pair{std:("requirements"), array<string>{ std:("Basic functionality") }}, 
                    object::pair{std:("apis"), array<any>()}
                }); });
                if (secondResult->success) {
                    console->log(std:("✅ Service recovered successfully after error scenario"));
                } else {
                    console->log(std:("⚠️ Service state affected by previous error, but handled gracefully"));
                }
            }
            catch (const any& error)
            {
                console->log(std:("🔄 Testing service stability after error..."));
                try
                {
                    auto recoveryResult = std::async([=]() { codeGenService->generateCode(object{
                        object::pair{std:("projectName"), std:("recovery-test")}, 
                        object::pair{std:("description"), std:("Test service recovery")}, 
                        object::pair{std:("targetType"), std:("plugin")}, 
                        object::pair{std:("requirements"), array<string>{ std:("Basic test") }}, 
                        object::pair{std:("apis"), array<any>()}
                    }); });
                    if (recoveryResult->success) {
                        console->log(std:("✅ Service recovered successfully despite error"));
                    } else {
                        console->log(std:("✅ Service remained stable despite error"));
                    }
                }
                catch (const any& recoveryError)
                {
                    console->error(std:("❌ Service failed to recover:"), recoveryError);
                    throw any(recoveryError);
                }
                if (AND((!(as<std::shared_ptr<Error>>(error))->message->includes(std:("timeout"))), (!(as<std::shared_ptr<Error>>(error))->message->includes(std:("rate limit"))))) {
                    throw any(error);
                } else {
                    console->log(std:("✅ Expected error handled correctly"));
                }
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("should maintain performance under sustained load")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("📈 Testing sustained load performance..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            if (!anthropicKey) {
                console->log(std:("⏭️ Skipping sustained load test - no ANTHROPIC_API_KEY"));
                return std::shared_ptr<Promise<void>>();
            }
            process->env->ANTHROPIC_API_KEY = anthropicKey;
            shared loadTestDuration = 30000;
            auto requestInterval = 2000;
            shared startTime = Date->now();
            shared results = array<object>();
            console->log(std:("🔄 Running sustained load test for ") + (loadTestDuration / 1000) + std:(" seconds..."));
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
                    auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
                    if (!codeGenService) {
                        throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
                    }
                    auto result = std::async([=]() { codeGenService->generateCode(object{
                        object::pair{std:("projectName"), std:("load-test-") + requestCount + string_empty}, 
                        object::pair{std:("description"), std:("Create a simple function called "loadTest") + requestCount + std:("" that returns "Load test ") + requestCount + std:("".")}, 
                        object::pair{std:("targetType"), std:("plugin")}, 
                        object::pair{std:("requirements"), array<string>{ std:("Function loadTest") + requestCount + std:(" returning "Load test ") + requestCount + std:(""") }}, 
                        object::pair{std:("apis"), array<any>()}
                    }); });
                    auto duration = Date->now() - requestStartTime;
                    results->push(object{
                        object::pair{std:("success"), result->success}, 
                        object::pair{std:("duration"), std:("duration")}, 
                        object::pair{std:("requestCount"), std:("requestCount")}
                    });
                    console->log(std:("✅ Load test request ") + requestCount + std:(" completed in ") + duration + std:("ms"));
                }
                catch (const any& error)
                {
                    auto duration = Date->now() - requestStartTime;
                    results->push(object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("duration"), std:("duration")}, 
                        object::pair{std:("requestCount"), std:("requestCount")}, 
                        object::pair{std:("error"), (as<std::shared_ptr<Error>>(error))->message}
                    });
                    console->log(std:("❌ Load test request ") + requestCount + std:(" failed in ") + duration + std:("ms"));
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
            console->log(std:("📊 Sustained load test results:"));
            console->log(std:("  Total requests: ") + results->get_length() + string_empty);
            console->log(std:("  Successful: ") + successful->get_length() + string_empty);
            console->log(std:("  Failed: ") + failed->get_length() + string_empty);
            console->log(std:("  Success rate: ") + ((successful->get_length() / results->get_length()) * 100)->toFixed(1) + std:("%"));
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
                console->log(std:("  Average response time: ") + avgDuration->toFixed(0) + std:("ms"));
                console->log(std:("  Max response time: ") + maxDuration + std:("ms"));
                console->log(std:("  Min response time: ") + minDuration + std:("ms"));
            }
            if (successful->get_length() == 0) {
                throw any(std::make_shared<Error>(std:("No successful requests during sustained load test")));
            }
            auto successRate = successful->get_length() / results->get_length();
            if (successRate < 0.5) {
                throw any(std::make_shared<Error>(std:("Success rate too low: ") + (successRate * 100)->toFixed(1) + std:("%")));
            }
            console->log(std:("✅ Sustained load test completed successfully"));
        }
        }
    } };
};

#endif
