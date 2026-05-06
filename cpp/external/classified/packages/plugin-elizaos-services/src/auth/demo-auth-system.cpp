#include "demo-auth-system.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> demoCLIAuthentication() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n🖥️  CLI AUTHENTICATION DEMO" << std::endl;
    std::cout << "═".repeat(50) << std::endl;

    const auto runtime = createDemoRuntime();
    const auto cliCommands = new CLIAuthCommands(runtime);

    std::cout << "\n1. Testing CLI Commands Registration..." << std::endl;
    const auto commands = cliCommands.getCommands();
    std::cout << "✅ Registered " + commands.size() + " CLI commands:" << std::endl;
    commands.forEach((cmd) => {
        std::cout << "   - " + cmd.name + ": " + cmd.description << std::endl;
        });

        std::cout << "\n2. Testing CLI Auth Status..." << std::endl;
        const auto statusCommand = commands.find((c) => c.name == "auth:status");
        if (statusCommand) {
            statusCommand.handler({});
        }

        std::cout << "\n3. Testing CLI Key Validation..." << std::endl;
        const auto validateCommand = commands.find((c) => c.name == "auth:validate");
        if (validateCommand) {
            validateCommand.handler({
                provider: "openai",
                key: TEST_KEYS.OPENAI_TEST_KEY,
                });
            }

            std::cout << "\n4. Testing CLI Test Keys Display..." << std::endl;
            const auto testKeysCommand = commands.find((c) => c.name == "auth:test-keys");
            if (testKeysCommand) {
                testKeysCommand.handler({});
            }

            std::cout << "\n✅ CLI Authentication Demo Complete\n" << std::endl;

}

std::future<void> demoAgentPluginAuthentication() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n🤖 AGENT PLUGIN AUTHENTICATION DEMO" << std::endl;
    std::cout << "═".repeat(50) << std::endl;

    const auto runtime = createDemoRuntime();

    std::cout << "\n1. Starting Agent Authentication Service..." << std::endl;
    const auto authService = AgentAuthService.start(runtime);
    runtime.registerService(authService);
    std::cout << "✅ Authentication service started" << std::endl;

    std::cout << "\n2. Testing Provider Readiness Check..." << std::endl;
    const auto isOpenAIReady = AuthHelper.isProviderReady(runtime, "openai", "text_generation");
    std::cout << "   OpenAI ready for text generation: " + std::to_string(isOpenAIReady ? "✅" : "❌") << std::endl;

    const auto isGroqReady = AuthHelper.isProviderReady(runtime, "groq", "text_generation");
    std::cout << "   Groq ready for text generation: " + std::to_string(isGroqReady ? "✅" : "❌") << std::endl;

    std::cout << "\n3. Finding Best Provider..." << std::endl;
    const auto bestProvider = AuthHelper.getBestProvider(runtime, "text_generation");
    std::cout << "   Best provider for text generation: " + std::to_string(bestProvider || "None available") << std::endl;

    std::cout << "\n4. Validation Before Use..." << std::endl;
    if (bestProvider) {
        const auto validation = AuthHelper.validateBeforeUse(runtime, bestProvider, "text_generation");
        console.log(
        "   Validation result: " + std::to_string(validation.isValid ? "✅ Valid" : `❌ ${validation.error}`)
        );
    }

    std::cout << "\n5. Getting Debug Information..." << std::endl;
    const auto debugInfo = AuthHelper.getDebugInfo(runtime);
    std::cout << "   Debug info:" << /* JSON.stringify */ std::string(debugInfo, nullptr, 2) << std::endl;

    std::cout << "\n6. Testing API Functionality..." << std::endl;
    const auto testResult = authService.testApiFunctionality("openai");
    std::cout << "   API test result: " + std::to_string(testResult.success ? "✅" : "❌") << std::endl;
    if (testResult.success) {
        std::cout << "   Response: \"" + testResult.response + "\"" << std::endl;
        std::cout << "   Latency: " + testResult.latency + "ms" << std::endl;
    }

    std::cout << "\n7. Stopping Service..." << std::endl;
    authService.stop();
    std::cout << "✅ Authentication service stopped" << std::endl;

    std::cout << "\n✅ Agent Plugin Authentication Demo Complete\n" << std::endl;

}

std::future<void> demoPlatformIntegration() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n🌐 PLATFORM INTEGRATION DEMO" << std::endl;
    std::cout << "═".repeat(50) << std::endl;

    const auto runtime = createDemoRuntime();

    std::cout << "\n1. Creating Platform Integration Services..." << std::endl;
    const auto cliPlatform = PlatformIntegrationFactory.createForCLI(runtime);
    const auto guiPlatform = PlatformIntegrationFactory.createForGUI(runtime);
    const auto agentPlatform = PlatformIntegrationFactory.createForAgent(runtime);
    std::cout << '✅ Created platform integrations for CLI << GUI << and Agent' << std::endl;

    std::cout << "\n2. Registering Client Sessions..." << std::endl;
    const auto cliSessionId = PlatformAuthUtils.generateSessionId();
    const auto guiSessionId = PlatformAuthUtils.generateSessionId();
    const auto agentSessionId = PlatformAuthUtils.generateSessionId();

    const auto cliSession = cliPlatform.registerSession(cliSessionId, "cli", "elizaos-cli");
    const auto guiSession = guiPlatform.registerSession(guiSessionId, "gui", "elizaos-gui");
    const auto agentSession = agentPlatform.registerSession(;
    agentSessionId,
    "agent",
    "elizaos-agent";
    );

    std::cout << "   CLI Session: " + cliSession.sessionId << std::endl;
    std::cout << "   GUI Session: " + guiSession.sessionId << std::endl;
    std::cout << "   Agent Session: " + agentSession.sessionId << std::endl;

    std::cout << "\n3. Distributing Test Keys..." << std::endl;

    // CLI gets test key
    const auto cliKeyResponse = cliPlatform.distributeKey({;
        sessionId: cliSessionId,
        provider: "openai",
        keyType: "test",
        clientCapabilities: PlatformAuthUtils.getClientCapabilities("cli"),
        });
        console.log(
        "   CLI OpenAI test key: " + std::to_string(cliKeyResponse.success ? "✅ Distributed" : `❌ ${cliKeyResponse.error}`)
        );

        // GUI gets test key
        const auto guiKeyResponse = guiPlatform.distributeKey({;
            sessionId: guiSessionId,
            provider: "openai",
            keyType: "test",
            clientCapabilities: PlatformAuthUtils.getClientCapabilities("gui"),
            });
            console.log(
            "   GUI OpenAI test key: " + std::to_string(guiKeyResponse.success ? "✅ Distributed" : `❌ ${guiKeyResponse.error}`)
            );

            std::cout << "\n4. Validating Distributed Keys..." << std::endl;
            if (cliKeyResponse.success && cliKeyResponse.apiKey) {
                const auto validation = cliPlatform.validateDistributedKey(;
                cliSessionId,
                "openai",
                cliKeyResponse.apiKey;
                );
                console.log(
                "   CLI key validation: " + std::to_string(validation.isValid ? "✅ Valid" : `❌ ${validation.error}`)
                );
            }

            std::cout << "\n5. Getting Session Status..." << std::endl;
            const auto cliStatus = cliPlatform.getSessionStatus(cliSessionId);
            std::cout << "   CLI session status: " + std::to_string(cliStatus.session ? "Active" : "Inactive") << std::endl;
            std::cout << "   CLI capabilities: " + std::to_string(cliStatus.capabilities.join(", ")) << std::endl;

            std::cout << "\n6. Platform Analytics..." << std::endl;
            const auto analytics = cliPlatform.getAnalytics();
            std::cout << "   Active sessions: " + analytics.activeSessions << std::endl;
            std::cout << "   Sessions by type:" << analytics.sessionsByType << std::endl;
            std::cout << "   Key distributions: " + analytics.keyDistributions << std::endl;

            std::cout << "\n7. Testing Success and Failure Cases..." << std::endl;

            // Test invalid session
            const auto invalidKeyResponse = cliPlatform.distributeKey({;
                sessionId: "invalid-session",
                provider: "openai",
                keyType: "test",
                clientCapabilities: [],
                });
                console.log(
                "   Invalid session test: " + std::to_string(!invalidKeyResponse.success ? "✅ Properly rejected" : "❌ Should have failed")
                );

                // Test unsupported provider
                const auto unsupportedResponse = cliPlatform.distributeKey({;
                    sessionId: cliSessionId,
                    provider: "unsupported",
                    keyType: "test",
                    clientCapabilities: [],
                    });
                    console.log(
                    "   Unsupported provider test: " + std::to_string(!unsupportedResponse.success ? "✅ Properly rejected" : "❌ Should have failed")
                    );

                    std::cout << "\n8. Session Cleanup..." << std::endl;
                    cliPlatform.invalidateSession(cliSessionId);
                    guiPlatform.invalidateSession(guiSessionId);
                    agentPlatform.invalidateSession(agentSessionId);
                    std::cout << "✅ All sessions invalidated" << std::endl;

                    std::cout << "\n✅ Platform Integration Demo Complete\n" << std::endl;

}

std::future<void> demoSuccessAndFailureScenarios() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n🧪 SUCCESS AND FAILURE SCENARIOS DEMO" << std::endl;
    std::cout << "═".repeat(50) << std::endl;

    std::cout << "\n1. Testing with Valid Configuration..." << std::endl;
    const auto validRuntime = createDemoRuntime(true);
    const auto validAuthService = new AuthenticationService(validRuntime);

    const auto validStatus = validAuthService.getAuthStatus();
    console.log(
    "   Valid config status: " + validStatus.overall + " (" + std::to_string(Object.keys(validStatus.providers).size()) + " providers)"
    );

    std::cout << "\n2. Testing with Invalid Configuration..." << std::endl;
    const auto invalidRuntime = createDemoRuntime(false);
    const auto invalidAuthService = new AuthenticationService(invalidRuntime);

    const auto invalidStatus = invalidAuthService.getAuthStatus();
    console.log(
    "   Invalid config status: " + invalidStatus.overall + " (" + std::to_string(Object.keys(invalidStatus.providers).size()) + " providers)"
    );

    std::cout << "\n3. Testing Key Validation Success..." << std::endl;
    const auto validResult = validAuthService.validateApiKey("openai", TEST_KEYS.OPENAI_TEST_KEY);
    std::cout << "   Test key validation: " + std::to_string(validResult.isValid ? "✅ Valid" : "❌ Invalid") << std::endl;
    std::cout << "   Key type: " + validResult.keyType << std::endl;
    std::cout << "   Capabilities: " + std::to_string(validResult.capabilities.join(", ")) << std::endl;

    std::cout << "\n4. Testing Key Validation Failure..." << std::endl;
    const auto invalidResult = validAuthService.validateApiKey("openai", "invalid-key");
    console.log(
    "   Invalid key validation: " + std::to_string(!invalidResult.isValid ? "✅ Properly rejected" : "❌ Should have failed")
    );
    std::cout << "   Error: " + invalidResult.errorMessage << std::endl;

    std::cout << "\n5. Testing API Functionality Success..." << std::endl;
    const auto apiTestSuccess = validAuthService.testApiFunctionality("openai");
    std::cout << "   API test with test key: " + std::to_string(apiTestSuccess.success ? "✅ Success" : "❌ Failed") << std::endl;
    if (apiTestSuccess.success) {
        std::cout << "   Response: \"" + std::to_string(apiTestSuccess.response.substring(0, 50)) + "...\"" << std::endl;
    }

    std::cout << "\n6. Testing API Functionality Failure..." << std::endl;
    const auto apiTestFailure = invalidAuthService.testApiFunctionality("openai");
    console.log(
    "   API test without key: " + std::to_string(!apiTestFailure.success ? "✅ Properly failed" : "❌ Should have failed")
    );
    std::cout << "   Error: " + apiTestFailure.error << std::endl;

    std::cout << "\n7. Testing Cache Behavior..." << std::endl;
    const auto startTime = Date.now();
    validAuthService.validateApiKey("openai", TEST_KEYS.OPENAI_TEST_KEY);
    const auto firstCallTime = Date.now() - startTime;

    const auto cacheStartTime = Date.now();
    validAuthService.validateApiKey("openai", TEST_KEYS.OPENAI_TEST_KEY);
    const auto cachedCallTime = Date.now() - cacheStartTime;

    std::cout << "   First call time: " + firstCallTime + "ms" << std::endl;
    std::cout << "   Cached call time: " + cachedCallTime + "ms" << std::endl;
    std::cout << "   Cache working: " + std::to_string(cachedCallTime < firstCallTime ? "✅ Yes" : "❌ No") << std::endl;

    std::cout << "\n✅ Success and Failure Scenarios Demo Complete\n" << std::endl;

}

std::future<void> runComprehensiveDemo() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🚀 COMPREHENSIVE AUTHENTICATION SYSTEM DEMO" << std::endl;
    std::cout << "═".repeat(70) << std::endl;
    std::cout << "This demo showcases the complete authentication system across all modalities" << std::endl;
    std::cout << "" << std::endl;

    try {
        // Demo each component
        demoCLIAuthentication();
        demoAgentPluginAuthentication();
        demoPlatformIntegration();
        demoSuccessAndFailureScenarios();

        std::cout << "🎉 COMPREHENSIVE DEMO COMPLETED SUCCESSFULLY!" << std::endl;
        std::cout << "═".repeat(70) << std::endl;
        std::cout << "" << std::endl;
        std::cout << "📋 Demo Summary:" << std::endl;
        std::cout << "✅ CLI authentication commands working" << std::endl;
        std::cout << "✅ Agent plugin integration working" << std::endl;
        std::cout << "✅ Platform integration across modalities working" << std::endl;
        std::cout << "✅ Success and failure scenarios handled correctly" << std::endl;
        std::cout << "✅ Test key distribution and validation working" << std::endl;
        std::cout << "✅ Session management and analytics working" << std::endl;
        std::cout << "✅ Cross-modality consistency maintained" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "🚀 The authentication system is ready for production use!" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "📚 Usage Examples:" << std::endl;
        std::cout << "   CLI: elizaos auth:status" << std::endl;
        std::cout << "   CLI: elizaos auth:setup" << std::endl;
        std::cout << "   Agent: runtime.getService("elizaos-services-auth")" << std::endl;
        std::cout << "   GUI: <AuthenticationPanel runtime={runtime} />" << std::endl;
        } catch (error) {
            std::cerr << "💥 Demo failed with error:" << error << std::endl;
            process.exit(1);
        }

}

} // namespace elizaos
