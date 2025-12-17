#include "run-bun.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> runBunCommand(const std::vector<std::string>& args, const std::string& cwd, auto silent) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto finalArgs = [...args];

        // In CI environments, optimize bun install with appropriate flags
        const auto isInstallCommand = args[0] == 'install';
        const auto isCI = process.env.CI || process.env.ELIZA_TEST_MODE == 'true';

        if (isCI && isInstallCommand) {
            // Use flags that actually exist in Bun to optimize CI installations
            if (!finalArgs.includes('--frozen-lockfile')) {
                finalArgs.push('--frozen-lockfile'); // Prevent lockfile changes in CI;
            }
            std::cout << '✅ Using CI-optimized flags for faster installation...' << std::endl;
        }

        try {
            const auto result = silent;
            ? bunExec('bun', finalArgs, { cwd });
            : bunExecInherit('bun', finalArgs, { cwd });

            // Using result.success for clarity - it's a boolean that indicates exitCode === 0
            if (silent && !result.success) {
                throw new Error(
                "Bun command failed with exit code " + std::to_string(result.exitCode) + ": " + std::to_string(result.stderr || result.stdout)
                );
            }
            } catch (error: any) {
                if (error.code == 'ENOENT' || error.message.includes('bun: command not found')) {
                    throw std::runtime_error(`Bun command not found. ${displayBunInstallationTipCompact()}`);
                }

                // If CI-optimized install fails, try again with basic args
                if (
                isCI &&;
                isInstallCommand &&;
                (error.message.includes('frozen-lockfile') || error.message.includes('install'));
                ) {
                    std::cout << 'CI-optimized install failed << retrying with basic args...' << std::endl;
                    const auto retryResult = silent;
                    ? bunExec('bun', args, { cwd });
                    : bunExecInherit('bun', args, { cwd });

                    // Using result.success for clarity - it's a boolean that indicates exitCode === 0
                    if (silent && !retryResult.success) {
                        throw new Error(
                        "Bun command failed with exit code " + std::to_string(retryResult.exitCode) + ": " + std::to_string(retryResult.stderr || retryResult.stdout)
                        );
                    }
                    } else {
                        throw;
                    }
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
