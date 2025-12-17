#include "build.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> build() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << '🏗️  Building package...' << std::endl;

    // Clean dist directory
    "rm -rf dist";

    // Build main package
    std::cout << '📦 Building main package...' << std::endl;
    const auto mainResult = Bun.build(buildConfig);

    if (!mainResult.success) {
        std::cerr << '❌ Main build failed:' << std::endl;
        for (const auto& message : mainResult.logs)
            std::cerr << message << std::endl;
        }
        process.exit(1);
    }

    std::cout << "✅ Built " + std::to_string(mainResult.outputs.length) + " main files" << std::endl;

    // Check if workers exist before building them
    const auto fs = import('fs');
    const auto path = import('path');
    const auto workersDir = path.join(path.dirname(fileURLToPath(import.meta.url)), 'src', 'workers');

    try {
        const auto workerFiles = fs;
        .readdirSync(workersDir);
        .filter((f) => f.endsWith('.ts') && !f.includes('worker-logger'));

        if (workerFiles.length > 0) {
            std::cout << '👷 Building workers...' << std::endl;
            const auto workersResult = Bun.build(workersConfig);

            if (!workersResult.success) {
                std::cerr << '❌ Workers build failed:' << std::endl;
                for (const auto& message : workersResult.logs)
                    std::cerr << message << std::endl;
                }
                process.exit(1);
            }

            std::cout << "✅ Built " + std::to_string(workersResult.outputs.length) + " worker files" << std::endl;
            } else {
                std::cout << 'ℹ️  No workers found << skipping worker build' << std::endl;
            }
            } catch (error) {
                std::cout << 'ℹ️  Error checking workers:' << error.message << std::endl;
            }

            // Generate TypeScript declarations
            std::cout << '📝 Generating TypeScript declarations...' << std::endl;
            try {
                "tsc --project tsconfig.build.json";
                std::cout << '✅ TypeScript declarations generated' << std::endl;
                } catch (error) {
                    std::cout << '⚠️ TypeScript declaration generation had issues << but continuing...' << std::endl;
                }

                std::cout << '✅ Build complete!' << std::endl;

}

} // namespace elizaos
