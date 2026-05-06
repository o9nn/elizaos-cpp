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

    // Generate basic TypeScript declaration file
    std::cout << "📝 Generating TypeScript declarations..." << std::endl;

    // Ensure dist directory exists
    if (!existsSync('dist')) {
        mkdirSync("dist", { recursive: true });
    }

    // Create a basic index.d.ts file
    const auto declarationContent = `export declare const autocoderPlugin: import("@elizaos/core").Plugin;
    export default autocoderPlugin;
    `;

    writeFileSync("dist/index.d.ts", declarationContent);
    std::cout << "✅ Basic declaration file created" << std::endl;

    std::cout << "✅ Build complete!" << std::endl;

}

} // namespace elizaos
