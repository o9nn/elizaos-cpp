#include "build.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

// No function implementations found to convert
// Original TypeScript may contain only interfaces/types

} // namespace elizaos
g package..." << std::endl;

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
        } catch (_error) {
            std::cout << '⚠️ TypeScript declaration generation had issues << but continuing...' << std::endl;
        }

        std::cout << "✅ Build complete!" << std::endl;

}

} // namespace elizaos
