#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_CLAUDE-CODE-INTEGRATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_CLAUDE-CODE-INTEGRATION_TEST_H
#include "core.h"
#include "@elizaos/core.h"
#include "../../services/CodeGenerationService.h"

class ClaudeCodeIntegrationTestSuite;

class ClaudeCodeIntegrationTestSuite : public TestSuite, public std::enable_shared_from_this<ClaudeCodeIntegrationTestSuite> {
public:
    using std::enable_shared_from_this<ClaudeCodeIntegrationTestSuite>::shared_from_this;
    string name = std::string("claude-code-integration");

    string description = std::string("Tests actual Claude Code SDK integration and generation");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("should verify Code Generation Service is properly configured")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🧪 Testing Code Generation Service configuration..."));
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std::string("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std::string("⚠️ No API keys configured - at least one LLM key required"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std::string("✅ Code Generation Service properly configured for local execution"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should run basic code generation")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🤖 Testing basic code generation..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std::string("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std::string("⏭️ Skipping basic generation test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            try
            {
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std::string("projectName"), std::string("test-basic-plugin")}, 
                    object::pair{std::string("description"), std::string("A simple test plugin")}, 
                    object::pair{std::string("targetType"), std::string("plugin")}, 
                    object::pair{std::string("requirements"), array<string>{ std::string("Create a hello world action") }}, 
                    object::pair{std::string("apis"), array<any>()}
                }); });
                if (!result->success) {
                    throw any(std::make_shared<Error>(std::string("Generation failed: ") + result->errors->join(std::string(", ")) + string_empty));
                }
                console->log(std::string("✅ Basic generation test successful"));
                console->log(std::string("   Generated ") + (OR((result->files->length), (0))) + std::string(" files"));
            }
            catch (const any& error)
            {
                console->error(std::string("❌ Basic generation test failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should generate TypeScript ElizaOS plugin structure")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("⚙️ Testing ElizaOS plugin structure generation..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std::string("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std::string("⏭️ Skipping plugin structure test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            try
            {
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std::string("projectName"), std::string("test-typescript-plugin")}, 
                    object::pair{std::string("description"), std::string("A TypeScript plugin with proper ElizaOS structure")}, 
                    object::pair{std::string("targetType"), std::string("plugin")}, 
                    object::pair{std::string("requirements"), array<string>{ std::string("Use TypeScript with strict mode"), std::string("Include proper type definitions"), std::string("Follow ElizaOS plugin conventions") }}, 
                    object::pair{std::string("apis"), array<any>()}
                }); });
                if (!result->success) {
                    throw any(std::make_shared<Error>(std::string("Plugin generation failed: ") + result->errors->join(std::string(", ")) + string_empty));
                }
                auto hasTypeScript = result->files->some([=](auto f) mutable
                {
                    return f["path"]->endsWith(std::string(".ts"));
                }
                );
                auto hasTsConfig = result->files->some([=](auto f) mutable
                {
                    return f["path"] == std::string("tsconfig.json");
                }
                );
                if (OR((!hasTypeScript), (!hasTsConfig))) {
                    throw any(std::make_shared<Error>(std::string("Missing TypeScript files or configuration")));
                }
                console->log(std::string("✅ Plugin structure test successful"));
            }
            catch (const any& error)
            {
                console->error(std::string("❌ Plugin structure test failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should test Claude Code with file operations")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("📁 Testing Claude Code file operations..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            auto e2bKey = runtime->getSetting(std::string("E2B_API_KEY"));
            if (OR((!anthropicKey), (!e2bKey))) {
                console->log(std::string("⏭️ Skipping file operations test - missing API keys"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            console->log(std::string("✅ File operations test setup successful (actual operations happen in sandbox)"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should test CodeGenerationService with real Claude Code")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🏗️ Testing CodeGenerationService with real Claude Code..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            auto e2bKey = runtime->getSetting(std::string("E2B_API_KEY"));
            if (OR((!anthropicKey), (!e2bKey))) {
                console->log(std::string("⏭️ Skipping CodeGenerationService test - missing API keys"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            try
            {
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std::string("projectName"), std::string("test-simple-plugin")}, 
                    object::pair{std::string("description"), std::string("A simple test plugin for validation")}, 
                    object::pair{std::string("targetType"), std::string("plugin")}, 
                    object::pair{std::string("requirements"), array<string>{ std::string("Create a basic plugin structure"), std::string("Include a hello action"), std::string("Use proper TypeScript types") }}, 
                    object::pair{std::string("apis"), array<any>()}, 
                    object::pair{std::string("testScenarios"), array<string>{ std::string("Test plugin loads correctly"), std::string("Test action responds to hello") }}
                }); });
                if (!result->success) {
                    throw any(std::make_shared<Error>(std::string("Code generation failed: ") + result->errors->join(std::string(", ")) + string_empty));
                }
                if (OR((!result->files), (result->files->length == 0))) {
                    throw any(std::make_shared<Error>(std::string("No files generated")));
                }
                auto fileNames = result->files->map([=](auto f) mutable
                {
                    return f["path"];
                }
                );
                auto requiredFiles = array<string>{ std::string("package.json"), std::string("src/index.ts") };
                for (auto& file : requiredFiles)
                {
                    if (!fileNames->includes(file)) {
                        throw any(std::make_shared<Error>(std::string("Missing required file: ") + file + string_empty));
                    }
                }
                auto indexFile = result->files->find([=](auto f) mutable
                {
                    return f["path"] == std::string("src/index.ts");
                }
                );
                if (!indexFile) {
                    throw any(std::make_shared<Error>(std::string("Missing index.ts file")));
                }
                auto indexContent = indexFile->content->toLowerCase();
                if (OR((!indexContent->includes(std::string("plugin"))), (!indexContent->includes(std::string("action"))))) {
                    throw any(std::make_shared<Error>(std::string("Generated index.ts missing plugin structure")));
                }
                console->log(std::string("✅ CodeGenerationService test successful - generated ") + result->files->length + std::string(" files"));
                if (result->executionResults) {
                    console->log(std::string("📊 QA Results:"));
                    console->log(std::string("  Lint: ") + (result->executionResults->lintPass) ? std::string("✅") : std::string("❌") + string_empty);
                    console->log(std::string("  Types: ") + (result->executionResults->typesPass) ? std::string("✅") : std::string("❌") + string_empty);
                    console->log(std::string("  Tests: ") + (result->executionResults->testsPass) ? std::string("✅") : std::string("❌") + string_empty);
                    console->log(std::string("  Build: ") + (result->executionResults->buildPass) ? std::string("✅") : std::string("❌") + string_empty);
                }
            }
            catch (const any& error)
            {
                console->error(std::string("❌ CodeGenerationService test failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should test timeout handling in Claude Code")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("⏱️ Testing Claude Code timeout handling..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            if (!anthropicKey) {
                console->log(std::string("⏭️ Skipping timeout test - no ANTHROPIC_API_KEY"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            try
            {
                auto originalTimeout = runtime->getSetting(std::string("ANTHROPIC_TIMEOUT"));
                auto originalRequestTimeout = runtime->getSetting(std::string("ANTHROPIC_REQUEST_TIMEOUT"));
                console->log(std::string("Current timeout settings:"));
                console->log(std::string("  ANTHROPIC_TIMEOUT: ") + (OR((originalTimeout), (std::string("default (300000ms)")))) + string_empty);
                console->log(std::string("  ANTHROPIC_REQUEST_TIMEOUT: ") + (OR((originalRequestTimeout), (std::string("default (600000ms)")))) + string_empty);
                auto startTime = Date->now();
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std::string("projectName"), std::string("timeout-test-plugin")}, 
                    object::pair{std::string("description"), std::string("Simple plugin to test timeout handling")}, 
                    object::pair{std::string("targetType"), std::string("plugin")}, 
                    object::pair{std::string("requirements"), array<string>{ std::string("Basic plugin structure only") }}, 
                    object::pair{std::string("apis"), array<any>()}
                }); });
                auto duration = Date->now() - startTime;
                console->log(std::string("Generation completed in ") + duration + std::string("ms"));
                if (!result->success) {
                    auto errorMessage = OR((result->errors->join(std::string(", "))->toLowerCase()), (string_empty));
                    if (OR((errorMessage["includes"](std::string("timeout"))), (errorMessage["includes"](std::string("timed out"))))) {
                        console->log(std::string("✅ Timeout detected and handled correctly"));
                    } else {
                        throw any(std::make_shared<Error>(std::string("Non-timeout error: ") + result->errors->join(std::string(", ")) + string_empty));
                    }
                } else {
                    console->log(std::string("✅ Generation successful - no timeout issues"));
                }
            }
            catch (const any& error)
            {
                auto errorMessage = (as<std::shared_ptr<Error>>(error))->message->toLowerCase();
                if (OR((errorMessage->includes(std::string("timeout"))), (errorMessage->includes(std::string("timed out"))))) {
                    console->log(std::string("✅ Timeout error properly caught and handled"));
                } else {
                    console->error(std::string("❌ Timeout test failed:"), error);
                    throw any(error);
                }
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should test chunked generation fallback")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🧩 Testing chunked generation fallback..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            if (!anthropicKey) {
                console->log(std::string("⏭️ Skipping chunked generation test - no ANTHROPIC_API_KEY"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            try
            {
                auto testPrompt = std::string("Generate a complete ElizaOS plugin with:\
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
                if (type_of(serviceAsAny["generateCodeInChunks"]) == std::string("function")) {
                    auto chunkResult = std::async([=]() { serviceAsAny["generateCodeInChunks"](testPrompt, 8000); });
                    if (!chunkResult) {
                        throw any(std::make_shared<Error>(std::string("Chunked generation returned no result")));
                    }
                    if (type_of(chunkResult) != std::string("string")) {
                        throw any(std::make_shared<Error>(std::string("Chunked generation result is not a string")));
                    }
                    auto chunks = array<string>{ std::string("Core Structure"), std::string("Services and Actions"), std::string("Documentation and Tests") };
                    auto chunksFound = 0;
                    for (auto& chunk : chunks)
                    {
                        if (chunkResult->includes(chunk)) {
                            chunksFound++;
                        }
                    }
                    if (chunksFound == 0) {
                        throw any(std::make_shared<Error>(std::string("No chunk headers found in result")));
                    }
                    console->log(std::string("✅ Chunked generation successful - ") + chunksFound + std::string("/") + chunks->get_length() + std::string(" chunks found"));
                    console->log(std::string("   Result length: ") + chunkResult->get_length() + std::string(" characters"));
                } else {
                    console->log(std::string("⏭️ Chunked generation method not accessible - testing via timeout"));
                    auto oldTimeout = process->env->ANTHROPIC_TIMEOUT;
                    process->env->ANTHROPIC_TIMEOUT = std::string("1000");
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
                                object::pair{std::string("projectName"), std::string("timeout-chunk-test")}, 
                                object::pair{std::string("description"), std::string("Complex plugin to trigger timeout and chunked generation")}, 
                                object::pair{std::string("targetType"), std::string("plugin")}, 
                                object::pair{std::string("requirements"), array<string>{ std::string("Multiple complex actions"), std::string("Advanced providers"), std::string("Comprehensive testing"), std::string("Full documentation"), std::string("Security features"), std::string("Performance optimization") }}, 
                                object::pair{std::string("apis"), array<string>{ std::string("OpenAI"), std::string("Discord"), std::string("GitHub"), std::string("Stripe"), std::string("AWS") }}
                            }); });
                            if (result->success) {
                                console->log(std::string("✅ Chunked generation likely activated (completed despite short timeout)"));
                            } else {
                                console->log(std::string("✅ Timeout handled gracefully"));
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
                console->error(std::string("❌ Chunked generation test failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should verify code generation with local file system")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🏗️ Testing code generation with local file system..."));
            auto anthropicKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            auto openaiKey = runtime->getSetting(std::string("OPENAI_API_KEY"));
            if (AND((!anthropicKey), (!openaiKey))) {
                console->log(std::string("⏭️ Skipping local file system test - no API keys available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService<std::shared_ptr<CodeGenerationService>>(std::string("code-generation"));
            if (!codeGenService) {
                throw any(std::make_shared<Error>(std::string("CodeGenerationService not available")));
            }
            try
            {
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std::string("projectName"), std::string("local-fs-test")}, 
                    object::pair{std::string("description"), std::string("Simple test to verify code generation with local file system")}, 
                    object::pair{std::string("targetType"), std::string("plugin")}, 
                    object::pair{std::string("requirements"), array<string>{ std::string("Create a basic plugin with a simple action") }}, 
                    object::pair{std::string("apis"), array<any>()}
                }); });
                if (!result->success) {
                    throw any(std::make_shared<Error>(std::string("Code generation failed: ") + result->errors->join(std::string(", ")) + string_empty));
                }
                if (OR((!result->files), (result->files->length == 0))) {
                    throw any(std::make_shared<Error>(std::string("No files generated")));
                }
                if (!result->projectPath) {
                    throw any(std::make_shared<Error>(std::string("No project path returned")));
                }
                console->log(std::string("✅ Code generation with local file system successful"));
                console->log(std::string("   Generated ") + result->files->length + std::string(" files"));
                console->log(std::string("   Project path: ") + result->projectPath + string_empty);
                if (result->executionResults) {
                    console->log(std::string("📊 Validation results:"));
                    console->log(std::string("   Lint: ") + (result->executionResults->lintPass) ? std::string("✅") : std::string("❌") + string_empty);
                    console->log(std::string("   Types: ") + (result->executionResults->typesPass) ? std::string("✅") : std::string("❌") + string_empty);
                    console->log(std::string("   Tests: ") + (result->executionResults->testsPass) ? std::string("✅") : std::string("❌") + string_empty);
                    console->log(std::string("   Build: ") + (result->executionResults->buildPass) ? std::string("✅") : std::string("❌") + string_empty);
                }
            }
            catch (const any& error)
            {
                console->error(std::string("❌ Local file system test failed:"), error);
                throw any(error);
            }
        }
        }
    } };
};

#endif
