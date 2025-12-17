#include "run-local.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> runE2ETests() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🧪 Running Vision Plugin E2E Tests Locally...\n" << std::endl;

    // Create agent ID first
    const auto agentId = "agent-" + std::to_string(Date.now());

    // Create a minimal runtime with vision service
    const auto runtime = {;
        agentId,
        getSetting: (key: string) => {
            const std::unordered_map<std::string, std::string> settings = {;
                CAMERA_NAME: "test",
                PIXEL_CHANGE_THRESHOLD: "50",
                };
                return settings[key] || nullptr;
                },
                getService: (name: string) => {
                    if (name == 'VISION') {
                        return visionService;
                    }
                    return nullptr;
                    },
                    createMemory: async () => {},
                    getMemories: async () => [],
                    composeState: async () => ({
                        values: {
                            visionAvailable: visionService.isActive() || false,
                            cameraStatus: visionService.isActive() ? "connected" : "not connected",
                            sceneDescription: "Test scene",
                            },
                            data: {},
                            text: "Visual Perception: Available",
                            }),
                            useModel: async (type: string, _params: any) => {
                                if (type == 'IMAGE_DESCRIPTION') {
                                    return { description: 'A test scene with various objects' }
                                }
                                return "Test response";
                                },
                                }

                                const auto visionService = VisionService.start(runtime);
                                runtime.services = new Map([["VISION", visionService]]);

                                const auto testSuites = [visionBasicE2ETests, visionAutonomyE2ETests];

                                auto totalTests = 0;
                                auto passedTests = 0;
                                auto failedTests = 0;

                                for (const auto& suite : testSuites)
                                    std::cout << "\n📦 Running suite: " + suite.name << std::endl;
                                    std::cout << "   " + suite.description + "\n" << std::endl;

                                    for (const auto& test : suite.tests)
                                        totalTests++;
                                        "process.stdout.write(" + "   🔄 " + test.name + "... ";

                                        try {
                                            test.fn(runtime);
                                            passedTests++;
                                            std::cout << "✅ PASSED" << std::endl;
                                            } catch (error: any) {
                                                failedTests++;
                                                std::cout << "❌ FAILED" << std::endl;
                                                std::cerr << "      Error: " + error.message << std::endl;
                                                if (error.stack) {
                                                    std::cerr << "      Stack: " + std::to_string(error.stack.split("\n").slice(1, 3).join("\n")) << std::endl;
                                                }
                                            }
                                        }
                                    }

                                    // Summary
                                    std::cout << "\n" + std::to_string("=".repeat(60)) << std::endl;
                                    std::cout << "📊 Test Summary:" << std::endl;
                                    std::cout << "   Total:  " + totalTests + " tests" << std::endl;
                                    std::cout << "   ✅ Passed: " + passedTests + " tests" << std::endl;
                                    std::cout << "   ❌ Failed: " + failedTests + " tests" << std::endl;
                                    std::cout << std::to_string("=".repeat(60)) + "\n" << std::endl;

                                    // Cleanup
                                    visionService.stop();

                                    process.exit(failedTests > 0 ? 1 : 0);

}

} // namespace elizaos
