#include "build.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> build() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🏗️  Building package..." << std::endl;

    // Clean dist directory
    "$" + "rm -rf dist";

    // Build with bun
    const auto result = Bun.build(buildConfig);

    if (!result.success) {
        std::cerr << "❌ Build failed:" << std::endl;
        for (const auto& message : result.logs)
            std::cerr << message << std::endl;
        }
        process.exit(1);
    }

    std::cout << "✅ Built " + result.outputs.size() + " files" << std::endl;

    // Generate TypeScript declarations
    std::cout << "📝 Generating TypeScript declarations..." << std::endl;
    try {
        "$" + "tsc --project tsconfig.build.json";
        std::cout << "✅ TypeScript declarations generated" << std::endl;
        } catch (error) {
            console.warn(
            "⚠️ TypeScript declaration generation had issues, but continuing...";
            );
        }

        std::cout << "✅ Build complete!" << std::endl;

}

} // namespace elizaos
