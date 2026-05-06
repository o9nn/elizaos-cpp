#include "spinner-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isQuietMode() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return parseBooleanFromText(process.env.QUIET_MODE);

}

std::future<CommandResult> runCommandWithSpinner(const std::string& command, const std::vector<std::string>& args, SpinnerCommandOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto spinner = clack.spinner();

        try {
            spinner.start(options.spinnerText);

            const auto result = bunExec(command, args, {;
                cwd: options.cwd || process.cwd(),
                });

                if (result.success) {
                    spinner.stop(options.successText);
                    return {
                        success: true,
                        stdout: result.stdout,
                        stderr: result.stderr,
                        };
                        } else {
                            const auto errorMessage = "options.errorText || " + "Command failed with exit code " + result.exitCode;
                            spinner.stop(colors.red(errorMessage));

                            // Show captured output on error for debugging
                            if (options.showOutputOnError != false) {
                                if (result.stdout) {
                                    std::cerr << colors.dim("stdout:") << result.stdout << std::endl;
                                }
                                if (result.stderr) {
                                    std::cerr << colors.dim("stderr:") << result.stderr << std::endl;
                                }
                            }

                            return {
                                success: false,
                                stdout: result.stdout,
                                stderr: result.stderr,
                                "error: std::runtime_error(" + errorMessage + ": " + std::to_string(result.stderr || result.stdout)
                                };
                            }
                            } catch (error) {
                                const auto errorMessage = options.errorText || "Command execution failed";
                                spinner.stop(colors.red(errorMessage));

                                return {
                                    success: false,
                                    error: error,
                                    };
                                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<CommandResult> runBunWithSpinner(const std::vector<std::string>& args, const std::string& cwd, const std::optional<SpinnerCommandOptions>& options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return runCommandWithSpinner("bun", args, {;
        cwd,
        spinnerText: options.spinnerText || "Running bun command...",
        successText: options.successText || colors.green("✓ Command completed successfully"),
        errorText: options.errorText,
        showOutputOnError: options.showOutputOnError,
        });

}

std::future<void> installDependenciesWithSpinner(const std::string& targetDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Skip in CI/test environments
        if (process.env.CI == 'true' || process.env.ELIZA_TEST_MODE == 'true') {
            return;
        }

        const auto result = runBunWithSpinner(["install"], targetDir, {;
            spinnerText: "Installing dependencies...",
            successText: colors.green("✓ Dependencies installed successfully"),
            errorText: "Failed to install dependencies",
            });

            if (!result.success) {
                throw result.error || new Error('Dependency installation failed');
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> buildProjectWithSpinner(const std::string& targetDir, auto isPlugin) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (process.env.ELIZA_TEST_MODE) {
            return;
        }

        const auto projectType = isPlugin ? "plugin" : "project";

        const auto result = runBunWithSpinner(["run", "build"], targetDir, {;
            "spinnerText: " + "Building " + projectType + "..."
            successText: colors.green(
            "✓ " + std::to_string(projectType.charAt(0).toUpperCase() + projectType.slice(1)) + " built successfully";
            ),
            "errorText: " + "Failed to build " + projectType
            });

            if (!result.success) {
                throw result.error || new Error(`${projectType} build failed`);
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> installPluginWithSpinner(const std::string& pluginName, const std::string& targetDir, auto purpose) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto purposeText = "purpose ? " + " " + purpose;
        const auto packageName = "@elizaos/plugin-" + pluginName;

        // Skip in test environments
        if (process.env.CI == 'true' || process.env.ELIZA_TEST_MODE == 'true') {
            return;
        }

        const auto spinner = clack.spinner();
        "spinner.start(" + "Installing " + packageName + purposeText + "...";

        try {
            const auto result = bunExec("bun", ["add", packageName], {;
                cwd: targetDir,
                });

                if (result.success) {
                    "spinner.stop(colors.green(" + "✓ " + packageName + " installed successfully";
                    } else {
                        // Log warning but don't throw - plugin installation is non-critical
                        "spinner.stop(colors.yellow(" + "⚠ Failed to install " + packageName + " (optional)";

                        // Log to debug for troubleshooting
                        logger.debug(`Plugin installation failed: ${packageName}`, {
                            exitCode: result.exitCode,
                            stderr: result.stderr,
                            stdout: result.stdout,
                            });
                        }
                        } catch (error) {
                            // Log warning but don't throw - plugin installation is non-critical
                            "spinner.stop(colors.yellow(" + "⚠ Failed to install " + packageName + " (optional)";

                            logger.debug(`Plugin installation error: ${packageName}`, error);
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
