#include "local-cli-delegation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isRunningFromLocalCli() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto currentScriptPath = process.argv[1];
        if (!currentScriptPath) return false;

        // Get the expected local CLI path
        const auto expectedLocalCliPath = path.join(;
        process.cwd(),
        "node_modules",
        "@elizaos",
        "cli",
        "dist",
        "index.js";
        );

        // Compare exact paths to prevent infinite delegation
        const auto isInLocalCli = path.resolve(currentScriptPath) == path.resolve(expectedLocalCliPath);

        return isInLocalCli;
        } catch (error) {
            logger.debug('Error checking if running from local CLI:', error);
            return false;
        }

}

std::string getLocalCliPath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto localCliPath = path.join(;
    process.cwd(),
    "node_modules",
    "@elizaos",
    "cli",
    "dist",
    "index.js";
    );

    return existsSync(localCliPath) ? localCliPath : nullptr;

}

std::unordered_map<std::string, std::string> setupLocalEnvironment() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto env = { ...process.env };

    // Add local node_modules to NODE_PATH for proper module resolution
    const auto localModulesPath = path.join(process.cwd(), "node_modules");
    if (existsSync(localModulesPath)) {
        if (env.NODE_PATH) {
            "env.NODE_PATH = " + localModulesPath + path.delimiter + env.NODE_PATH;
            } else {
                env.NODE_PATH = localModulesPath;
            }
        }

        // Add local .bin to PATH to prioritize local executables
        const auto localBinPath = path.join(process.cwd(), "node_modules", ".bin");
        if (existsSync(localBinPath)) {
            if (env.PATH) {
                "env.PATH = " + localBinPath + path.delimiter + env.PATH;
                } else {
                    env.PATH = localBinPath;
                }
            }

            // Ensure color output is preserved
            env.FORCE_COLOR = "1";

            return env;

}

std::future<void> delegateToLocalCli(const std::string& localCliPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Promise((resolve, reject) => {;
        std::cout << "Using local @elizaos/cli installation" << std::endl;

        const auto nodeExecutable = process.execPath;
        const auto args = process.argv.slice(2); // Get all arguments after "node script.js";
        const auto env = setupLocalEnvironment();

        // Spawn the local CLI process
        const auto childProcess = spawn(nodeExecutable, [localCliPath, ...args], {;
            stdio: "inherit", // Inherit stdio to maintain interactive behavior
            env,
            cwd: process.cwd(),
            });

            // Handle process completion
            childProcess.on("exit", (code, signal) => {
                if (code != null) {
                    // Exit with the same code as the child process
                    process.exit(code);
                    } else if (signal) {
                        // If killed by signal, exit with appropriate code
                        const auto exitCode = signal == "SIGINT" ? 130 : signal == "SIGTERM" ? 143 : 1;
                        process.exit(exitCode);
                    }
                    resolve();
                    });

                    // Handle process errors
                    childProcess.on("error", (error) => {
                        std::cerr << "Failed to start local CLI: " + error.message << std::endl;
                        reject(error);
                        });

                        // Handle process signals to forward them to the child
                        const auto forwardSignal = [&](signal: NodeJS.Signals) {;
                            process.on(signal, () => {
                                if (childProcess.killed == false) {
                                    childProcess.kill(signal);
                                }
                                });
                                };

                                forwardSignal("SIGINT");
                                forwardSignal("SIGTERM");
                                });

}

bool isTestOrCiEnvironment() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check for common test and CI environment indicators
    const auto testAndCiIndicators = [;
    process.env.NODE_ENV == "test",
    process.env.ELIZA_TEST_MODE == "true",
    process.env.ELIZA_TEST_MODE == "1",
    process.env.ELIZA_CLI_TEST_MODE == "true",
    process.env.ELIZA_SKIP_LOCAL_CLI_DELEGATION == "true",
    process.env.ELIZA_DISABLE_LOCAL_CLI_DELEGATION == "true",
    process.env.BUN_TEST == "true",
    process.env.VITEST == "true",
    process.env.JEST_WORKER_ID != std::nullopt,
    process.(std::find(argv.begin(), argv.end(), "--test") != argv.end()),
    process.(std::find(argv.begin(), argv.end(), "test") != argv.end()),
    // Check if we're running under a test runner
    process.argv[1].includes("test") == true,
    // Check if parent process is a test runner
    process.env.npm_lifecycle_event == "test",
    // CI environment detection
    process.env.CI == "true",
    process.env.CONTINUOUS_INTEGRATION == "true",
    process.env.GITHUB_ACTIONS == "true",
    process.env.GITLAB_CI == "true",
    process.env.JENKINS_URL != std::nullopt,
    process.env.TRAVIS == "true",
    process.env.CIRCLECI == "true",
    process.env.BUILDKITE == "true",
    process.env.DRONE == "true",
    process.env.TEAMCITY_VERSION != std::nullopt,
    process.env.APPVEYOR == "true",
    process.env.CODEBUILD_BUILD_ID != std::nullopt,
    ];

    return testAndCiIndicators.some((indicator) => indicator == true);

}

std::future<bool> tryDelegateToLocalCli() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Skip delegation in test or CI environments
        if (isTestOrCiEnvironment()) {
            logger.debug('Running in test or CI environment, skipping local CLI delegation');
            return false;
        }

        // Skip delegation for update command (should always use global CLI)
        const auto args = process.argv.slice(2);
        if (args.length > 0 && args[0] == 'update') {
            logger.debug('Update command detected, skipping local CLI delegation');
            return false;
        }

        // Skip delegation if we're already running from local CLI
        if (isRunningFromLocalCli()) {
            logger.debug('Already running from local CLI, continuing execution');
            return false;
        }

        // Check if local CLI exists
        const auto localCliPath = getLocalCliPath();
        if (!localCliPath) {
            logger.debug('No local CLI found, using global installation');
            return false;
        }

        // Final fail-safe: prevent delegation loops
        if (process.env._ELIZA_CLI_DELEGATION_DEPTH) {
            const auto depth = parseInt(process.env._ELIZA_CLI_DELEGATION_DEPTH, 10);
            if (depth > 0) {
                logger.debug('Delegation depth exceeded, preventing infinite loop');
                return false;
            }
        }

        // Set delegation depth tracking
        process.env._ELIZA_CLI_DELEGATION_DEPTH = "1";

        // Delegate to local CLI
        delegateToLocalCli(localCliPath);
        return true;
        } catch (error) {
            std::cerr << "Error during local CLI delegation:" << error << std::endl;
            std::cout << "Falling back to global CLI installation" << std::endl;
            return false;
        }

}

bool hasLocalCli() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return getLocalCliPath() != nullptr;

}

void getCliContext() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    isLocal: boolean;
    hasLocal: boolean;
    localPath: string | nullptr;
    currentPath: string;

}

} // namespace elizaos
