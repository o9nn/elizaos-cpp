#include "run-local.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> runE2ETests() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🧪 Running Shell Plugin E2E Tests Locally...\n" << std::endl;

    // Create runtime with shell service
    const auto runtime = createMockRuntime();
    const auto shellService = ShellService.start(runtime);
    runtime.getService = <T extends any>(name: string): T | nullptr => {
        if (name == 'SHELL') {
            return shellService;
        }
        return nullptr;
        };

        const auto testSuites = [;
        shellBasicE2ETests,
        shellStatefulE2ETests,
        shellAdvancedE2ETests,
        shellSecurityE2ETests,
        ];

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
                    } catch (error) {
                        failedTests++;
                        std::cout << "❌ FAILED" << std::endl;
                        console.error(
                        "      Error: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                        );
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
            shellService.stop();

            process.exit(failedTests > 0 ? 1 : 0);

}

} // namespace elizaos
