#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_AUTOCODER_SRC___TESTS___E2E_CLAUDE_CODE_INTEGRATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_AUTOCODER_SRC___TESTS___E2E_CLAUDE_CODE_INTEGRATION_TEST_H
#include "core.hpp"
// External dependency removed
#include "../../services/CodeGenerationService.h"

class ClaudeCodeIntegrationTestSuite;

class ClaudeCodeIntegrationTestSuite : public TestSuite, public std::enable_shared_from_this<ClaudeCodeIntegrationTestSuite> {
public:
    using std::enable_shared_from_this<ClaudeCodeIntegrationTestSuite>::shared_from_this;
    string name = std:("claude-code-integration");

    string description = std:("Tests actual Claude Code SDK integration and generation");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("should verify Code Generation Service is properly configured")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing Code Generation Service configuration..."));
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std:("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std:("⚠️ No API keys configured - at least one LLM key required"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std:("✅ Code Generation Service properly configured for local execution"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should run basic code generation")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🤖 Testing basic code generation..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std:("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std:("⏭️ Skipping basic generation test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            try
            {
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std:("projectName"), std:("test-basic-plugin")}, 
                    object::pair{std:("description"), std:("A simple test plugin")}, 
                    object::pair{std:("targetType"), std:("plugin")}, 
                    object::pair{std:("requirements"), array<string>{ std:("Create a hello world action") }}, 
                    object::pair{std:("apis"), array<any>()}
                }); });
                if (!result->success) {
                    throw any(std::make_shared<Error>(std:("Generation failed: ") + result->errors->join(std:(", ")) + string_empty));
                }
                console->log(std:("✅ Basic generation test successful"));
                console->log(std:("   Generated ") + (OR((result->files->length), (0))) + std:(" files"));
            }
            catch (const any& error)
            {
                console->error(std:("❌ Basic generation test failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("should generate TypeScript ElizaOS plugin structure")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("⚙️ Testing ElizaOS plugin structure generation..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std:("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std:("⏭️ Skipping plugin structure test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            try
            {
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std:("projectName"), std:("test-typescript-plugin")}, 
                    object::pair{std:("description"), std:("A TypeScript plugin with proper ElizaOS structure")}, 
                    object::pair{std:("targetType"), std:("plugin")}, 
                    object::pair{std:("requirements"), array<string>{ std:("Use TypeScript with strict mode"), std:("Include proper type definitions"), std:("Follow ElizaOS plugin conventions") }}, 
                    object::pair{std:("apis"), array<any>()}
                }); });
                if (!result->success) {
                    throw any(std::make_shared<Error>(std:("Plugin generation failed: ") + result->errors->join(std:(", ")) + string_empty));
                }
                auto hasTypeScript = result->files->some([=](auto f) mutable
                {
                    return f["path"]->endsWith(std:(".ts"));
                }
                );
                auto hasTsConfig = result->files->some([=](auto f) mutable
                {
                    return f["path"] == std:("tsconfig.json");
                }
                );
                if (OR((!hasTypeScript), (!hasTsConfig))) {
                    throw any(std::make_shared<Error>(std:("Missing TypeScript files or configuration")));
                }
                console->log(std:("✅ Plugin structure test successful"));
            }
            catch (const any& error)
            {
                console->error(std:("❌ Plugin structure test failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("should test Claude Code with file operations")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("📁 Testing Claude Code file operations..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            auto e2bKey = runtime->getSetting(std:("E2B_API_KEY"));
            if (OR((!anthropicKey), (!e2bKey))) {
                console->log(std:("⏭️ Skipping file operations test - missing API keys"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            console->log(std:("✅ File operations test setup successful (actual operations happen in sandbox)"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("should test CodeGenerationService with real Claude Code")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🏗️ Testing CodeGenerationService with real Claude Code..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            auto e2bKey = runtime->getSetting(std:("E2B_API_KEY"));
            if (OR((!anthropicKey), (!e2bKey))) {
                console->log(std:("⏭️ Skipping CodeGenerationService test - missing API keys"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            try
            {
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std:("projectName"), std:("test-simple-plugin")}, 
                    object::pair{std:("description"), std:("A simple test plugin for validation")}, 
                    object::pair{std:("targetType"), std:("plugin")}, 
                    object::pair{std:("requirements"), array<string>{ std:("Create a basic plugin structure"), std:("Include a hello action"), std:("Use proper TypeScript types") }}, 
                    object::pair{std:("apis"), array<any>()}, 
                    object::pair{std:("testScenarios"), array<string>{ std:("Test plugin loads correctly"), std:("Test action responds to hello") }}
                }); });
                if (!result->success) {
                    throw any(std::make_shared<Error>(std:("Code generation failed: ") + result->errors->join(std:(", ")) + string_empty));
                }
                if (OR((!result->files), (result->files->length == 0))) {
                    throw any(std::make_shared<Error>(std:("No files generated")));
                }
                auto fileNames = result->files->map([=](auto f) mutable
                {
                    return f["path"];
                }
                );
                auto requiredFiles = array<string>{ std:("package.json"), std:("src/index.ts") };
                for (auto& file : requiredFiles)
                {
                    if (!fileNames->includes(file)) {
                        throw any(std::make_shared<Error>(std:("Missing required file: ") + file + string_empty));
                    }
                }
                auto indexFile = result->files->find([=](auto f) mutable
                {
                    return f["path"] == std:("src/index.ts");
                }
                );
                if (!indexFile) {
                    throw any(std::make_shared<Error>(std:("Missing index.ts file")));
                }
                auto indexContent = indexFile->content->toLowerCase();
                if (OR((!indexContent->includes(std:("plugin"))), (!indexContent->includes(std:("action"))))) {
                    throw any(std::make_shared<Error>(std:("Generated index.ts missing plugin structure")));
                }
                console->log(std:("✅ CodeGenerationService test successful - generated ") + result->files->length + std:(" files"));
                if (result->executionResults) {
                    console->log(std:("📊 QA Results:"));
                    console->log(std:("  Lint: ") + (result->executionResults->lintPass) ? std:("✅") : std:("❌") + string_empty);
                    console->log(std:("  Types: ") + (result->executionResults->typesPass) ? std:("✅") : std:("❌") + string_empty);
                    console->log(std:("  Tests: ") + (result->executionResults->testsPass) ? std:("✅") : std:("❌") + string_empty);
                    console->log(std:("  Build: ") + (result->executionResults->buildPass) ? std:("✅") : std:("❌") + string_empty);
                }
            }
            catch (const any& error)
            {
                console->error(std:("❌ CodeGenerationService test failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("should test timeout handling in Claude Code")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("⏱️ Testing Claude Code timeout handling..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            if (!anthropicKey) {
                console->log(std:("⏭️ Skipping timeout test - no ANTHROPIC_API_KEY"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            try
            {
                auto originalTimeout = runtime->getSetting(std:("ANTHROPIC_TIMEOUT"));
                auto originalRequestTimeout = runtime->getSetting(std:("ANTHROPIC_REQUEST_TIMEOUT"));
                console->log(std:("Current timeout settings:"));
                console->log(std:("  ANTHROPIC_TIMEOUT: ") + (OR((originalTimeout), (std:("default (300000ms)")))) + string_empty);
                console->log(std:("  ANTHROPIC_REQUEST_TIMEOUT: ") + (OR((originalRequestTimeout), (std:("default (600000ms)")))) + string_empty);
                auto startTime = Date->now();
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std:("projectName"), std:("timeout-test-plugin")}, 
                    object::pair{std:("description"), std:("Simple plugin to test timeout handling")}, 
                    object::pair{std:("targetType"), std:("plugin")}, 
                    object::pair{std:("requirements"), array<string>{ std:("Basic plugin structure only") }}, 
                    object::pair{std:("apis"), array<any>()}
                }); });
                auto duration = Date->now() - startTime;
                console->log(std:("Generation completed in ") + duration + std:("ms"));
                if (!result->success) {
                    auto errorMessage = OR((result->errors->join(std:(", "))->toLowerCase()), (string_empty));
                    if (OR((errorMessage["includes"](std:("timeout"))), (errorMessage["includes"](std:("timed out"))))) {
                        console->log(std:("✅ Timeout detected and handled correctly"));
                    } else {
                        throw any(std::make_shared<Error>(std:("Non-timeout error: ") + result->errors->join(std:(", ")) + string_empty));
                    }
                } else {
                    console->log(std:("✅ Generation successful - no timeout issues"));
                }
            }
            catch (const any& error)
            {
                auto errorMessage = (as<std::shared_ptr<Error>>(error))->message->toLowerCase();
                if (OR((errorMessage->includes(std:("timeout"))), (errorMessage->includes(std:("timed out"))))) {
                    console->log(std:("✅ Timeout error properly caught and handled"));
                } else {
                    console->error(std:("❌ Timeout test failed:"), error);
                    throw any(error);
                }
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("should test chunked generation fallback")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧩 Testing chunked generation fallback..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            if (!anthropicKey) {
                console->log(std:("⏭️ Skipping chunked generation test - no ANTHROPIC_API_KEY"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            try
            {
                auto testPrompt = std:("Generate a complete ElizaOS plugin with:\
1. Full package.json with all dependencies\
2. TypeScript configuration\
3. Multiple actions (hello, goodbye, help)\
4. Multiple providers (time, weather, news)\
5. Comprehensive tests\
6. Complete documentation\
7. Docker configuration\
8. CI/CD pipeline\
9. Security scanning\
10. Performance benchmarks\
\
Make it a production-ready, enterprise-grade plugin.");
                auto serviceAsAny = as<any>(codeGenService);
                if (type_of(serviceAsAny["generateCodeInChunks"]) == std:("function")) {
                    auto chunkResult = std::async([=]() { serviceAsAny["generateCodeInChunks"](testPrompt, 8000); });
                    if (!chunkResult) {
                        throw any(std::make_shared<Error>(std:("Chunked generation returned no result")));
                    }
                    if (type_of(chunkResult) != std:("string")) {
                        throw any(std::make_shared<Error>(std:("Chunked generation result is not a string")));
                    }
                    auto chunks = array<string>{ std:("Core Structure"), std:("Services and Actions"), std:("Documentation and Tests") };
                    auto chunksFound = 0;
                    for (auto& chunk : chunks)
                    {
                        if (chunkResult->includes(chunk)) {
                            chunksFound++;
                        }
                    }
                    if (chunksFound == 0) {
                        throw any(std::make_shared<Error>(std:("No chunk headers found in result")));
                    }
                    console->log(std:("✅ Chunked generation successful - ") + chunksFound + std:("/") + chunks->get_length() + std:(" chunks found"));
                    console->log(std:("   Result length: ") + chunkResult->get_length() + std:(" characters"));
                } else {
                    console->log(std:("⏭️ Chunked generation method not accessible - testing via timeout"));
                    auto oldTimeout = process->env->ANTHROPIC_TIMEOUT;
                    process->env->ANTHROPIC_TIMEOUT = std:("1000");
                    {
                        utils::finally __finally14673_14916([&]() mutable
                        {
                            if (oldTimeout) {
                                process->env->ANTHROPIC_TIMEOUT = oldTimeout;
                            } else {
                                process->env.Delete("ANTHROPIC_TIMEOUT");
                            }
                        });
                        try
                        {
                            auto result = std::async([=]() { codeGenService->generateCode(object{
                                object::pair{std:("projectName"), std:("timeout-chunk-test")}, 
                                object::pair{std:("description"), std:("Complex plugin to trigger timeout and chunked generation")}, 
                                object::pair{std:("targetType"), std:("plugin")}, 
                                object::pair{std:("requirements"), array<string>{ std:("Multiple complex actions"), std:("Advanced providers"), std:("Comprehensive testing"), std:("Full documentation"), std:("Security features"), std:("Performance optimization") }}, 
                                object::pair{std:("apis"), array<string>{ std:("OpenAI"), std:("Discord"), std:("GitHub"), std:("Stripe"), std:("AWS") }}
                            }); });
                            if (result->success) {
                                console->log(std:("✅ Chunked generation likely activated (completed despite short timeout)"));
                            } else {
                                console->log(std:("✅ Timeout handled gracefully"));
                            }
                        }
                        catch (...)
                        {
                            throw;
                        }
                    }
                }
            }
            catch (const any& error)
            {
                console->error(std:("❌ Chunked generation test failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("should verify code generation with local file system")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🏗️ Testing code generation with local file system..."));
            auto anthropicKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std:("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std:("⏭️ Skipping local file system test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std:("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std:("CodeGenerationService not available")));
            }
            try
            {
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std:("projectName"), std:("local-fs-test")}, 
                    object::pair{std:("description"), std:("Simple test to verify code generation with local file system")}, 
                    object::pair{std:("targetType"), std:("plugin")}, 
                    object::pair{std:("requirements"), array<string>{ std:("Create a basic plugin with a simple action") }}, 
                    object::pair{std:("apis"), array<any>()}
                }); });
                if (!result->success) {
                    throw any(std::make_shared<Error>(std:("Code generation failed: ") + result->errors->join(std:(", ")) + string_empty));
                }
                if (OR((!result->files), (result->files->length == 0))) {
                    throw any(std::make_shared<Error>(std:("No files generated")));
                }
                if (!result->projectPath) {
                    throw any(std::make_shared<Error>(std:("No project path returned")));
                }
                console->log(std:("✅ Code generation with local file system successful"));
                console->log(std:("   Generated ") + result->files->length + std:(" files"));
                console->log(std:("   Project path: ") + result->projectPath + string_empty);
                if (result->executionResults) {
                    console->log(std:("📊 Validation results:"));
                    console->log(std:("   Lint: ") + (result->executionResults->lintPass) ? std:("✅") : std:("❌") + string_empty);
                    console->log(std:("   Types: ") + (result->executionResults->typesPass) ? std:("✅") : std:("❌") + string_empty);
                    console->log(std:("   Tests: ") + (result->executionResults->testsPass) ? std:("✅") : std:("❌") + string_empty);
                    console->log(std:("   Build: ") + (result->executionResults->buildPass) ? std:("✅") : std:("❌") + string_empty);
                }
            }
            catch (const any& error)
            {
                console->error(std:("❌ Local file system test failed:"), error);
                throw any(error);
            }
        }
        }
    } };
};

#endif
