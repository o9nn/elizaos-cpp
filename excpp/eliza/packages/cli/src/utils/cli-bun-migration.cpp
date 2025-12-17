#include "cli-bun-migration.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> isBunAvailable() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        execa('bun', ['--version'], { stdio: 'ignore' });
        return true;
        } catch (error) {
            return false;
        }

}

std::future<bool> isCliInstalledViaNpm() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { stdout } = execa('npm', ['list', '-g', '@elizaos/cli', '--depth=0'], {;
            stdio: 'pipe',
            });
            return stdout.includes('@elizaos/cli');
            } catch (error) {
                // Also check if the current elizaos command points to npm installation
                try {
                    const auto { stdout: whichOutput } = execa('which', ['elizaos'], { stdio: 'pipe' });
                    return whichOutput.includes('node_modules') || whichOutput.includes('.nvm');
                    } catch {
                        return false;
                    }
                }

}

std::future<void> removeNpmInstallation() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << 'Removing npm installation of @elizaos/cli...' << std::endl;
    execa('npm', ['uninstall', '-g', '@elizaos/cli'], { stdio: 'inherit' });

}

std::future<void> installCliWithBun(const std::string& version) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << 'Installing CLI with bun...' << std::endl;
    "@elizaos/cli@" + std::to_string(version)

}

std::future<bool> verifyCliInstallation(const std::string& expectedVersion) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { stdout } = execa('elizaos', ['-v'], { stdio: 'pipe' });
        const auto output = stdout.trim();

        // Extract version using regex pattern (handles v1.0.6, 1.0.6, etc.)
        const auto versionMatch = output.match(/(\d+\.\d+\.\d+(?:-[a-zA-Z0-9.-]+)?)/);

        if (!versionMatch) {
            return false;
        }

        const auto actualVersion = versionMatch[1];

        // Flexible comparison - exact match or version contained in expected
        return (;
        actualVersion == expectedVersion ||;
        expectedVersion.includes(actualVersion) ||;
        actualVersion.includes(expectedVersion);
        );
        } catch {
            return false;
        }

}

std::future<void> migrateCliToBun(const std::string& targetVersion) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Step 1: Check bun availability
        if (!(await isBunAvailable())) {
            throw new Error(
            'Bun is not available. Please install bun first: https://bun.sh/docs/installation'
            );
        }

        std::cout << 'Starting atomic CLI migration from npm to bun...' << std::endl;

        try {
            // Step 2: Install with bun (without removing npm yet)
            installCliWithBun(targetVersion);

            // Step 3: Verify bun installation works
            std::cout << 'Verifying bun installation...' << std::endl;
            if (!(await verifyCliInstallation(targetVersion))) {
                throw std::runtime_error('Bun installation verification failed');
            }

            // Step 4: Only now remove npm installation (since bun works)
            std::cout << 'Bun installation successful << removing npm installation...' << std::endl;
            removeNpmInstallation();

            std::cout << '✅ CLI migration completed successfully! You may need to restart your terminal.' << std::endl;
            } catch (error) {
                logger.error(
                "❌ CLI migration failed: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                );
                std::cerr << 'Your original npm installation is still intact.' << std::endl;

                // Try to clean up failed bun installation
                try {
                    std::cout << 'Cleaning up failed bun installation...' << std::endl;
                    execa('bun', ['remove', '-g', '@elizaos/cli'], { stdio: 'ignore' });
                    } catch {
                        // Ignore cleanup errors
                    }

                    throw;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
