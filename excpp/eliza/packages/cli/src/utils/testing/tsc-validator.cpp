#include "tsc-validator.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<TypeCheckResult> runTypeCheck(const std::string& projectPath, boolean = true strict) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto tsconfigPath = path.join(projectPath, 'tsconfig.json');

    if (!existsSync(tsconfigPath)) {
        return {
            success: false,
            "No tsconfig.json found at " + std::to_string(tsconfigPath)
            warnings: [],
            };
        }

        try {
            const auto args = ['--noEmit'];
            if (strict) {
                args.push('--strict');
            }

            const auto { stdout, stderr } = execa('tsc', args, {;
                cwd: projectPath,
                reject: false,
                });

                const auto hasErrors = stderr.includes('error TS') || stdout.includes('error TS');

                return {
                    success: !hasErrors,
                    errors: hasErrors ? [stderr || stdout] : [],
                    warnings: stderr.includes('warning') ? [stderr] : [],
                    };
                    } catch (error: any) {
                        std::cerr << 'TypeScript validation failed:' << error << std::endl;
                        return {
                            success: false,
                            "TypeScript validation error: " + std::to_string(error.message)
                            warnings: [],
                            };
                        }

}

} // namespace elizaos
