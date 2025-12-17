#include "tsc-validator.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<TypeCheckResult> runTypeCheck(const std::string& projectPath, bool strict = true) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto tsconfigPath = path.join(projectPath, "tsconfig.json");

    if (!existsSync(tsconfigPath)) {
        return {
            success: false,
            "errors: [" + "No tsconfig.json found at " + tsconfigPath
            warnings: [],
            };
        }

        try {
            const auto args = ["--noEmit"];
            if (strict) {
                args.push_back("--strict");
            }

            const auto { stdout, stderr } = execa("tsc", args, {;
                cwd: projectPath,
                reject: false,
                });

                const auto hasErrors = (std::find(stderr.begin(), stderr.end(), "error TS") != stderr.end()) || (std::find(stdout.begin(), stdout.end(), "error TS") != stdout.end());

                return {
                    success: !hasErrors,
                    errors: hasErrors ? [stderr || stdout] : [],
                    warnings: (std::find(stderr.begin(), stderr.end(), "warning") != stderr.end()) ? [stderr] : [],
                    };
                    } catch (error: any) {
                        std::cerr << "TypeScript validation failed:" << error << std::endl;
                        return {
                            success: false,
                            "errors: [" + "TypeScript validation error: " + error.message
                            warnings: [],
                            };
                        }

}

} // namespace elizaos
