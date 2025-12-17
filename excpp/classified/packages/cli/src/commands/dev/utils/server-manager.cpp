#include "server-manager.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<std::string>> getLocalCliPath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto localCliPath = path.join(;
    process.cwd(),
    'node_modules',
    '@elizaos',
    'cli',
    'dist',
    'index.js';
    );

    try {
        const auto fs = import('fs');
        return fs.existsSync(localCliPath) ? localCliPath : nullptr;
        } catch {
            return nullptr;
        }

}

std::unordered_map<std::string, std::string> setupEnvironment() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto env = { ...process.env };

    // Add local node_modules to NODE_PATH for proper module resolution
    const auto localModulesPath = path.join(process.cwd(), 'node_modules');
    if (env.NODE_PATH) {
        std::to_string(localModulesPath) + std::to_string(path.delimiter) + std::to_string(env.NODE_PATH);
        } else {
            env.NODE_PATH = localModulesPath;
        }

        // Add local .bin to PATH to prioritize local executables
        const auto localBinPath = path.join(process.cwd(), 'node_modules', '.bin');
        if (env.PATH) {
            std::to_string(localBinPath) + std::to_string(path.delimiter) + std::to_string(env.PATH);
            } else {
                env.PATH = localBinPath;
            }

            // Ensure color output
            env.FORCE_COLOR = '1';

            // Preserve ELIZA_TEST_MODE for test environments
            if (process.env.ELIZA_TEST_MODE) {
                env.ELIZA_TEST_MODE = process.env.ELIZA_TEST_MODE;
            }

            return env;

}

std::future<void> startServerProcess(const std::vector<string[] =>& args) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    stopServerProcess();

    std::cout << 'Starting server...' << std::endl;

    const auto nodeExecutable = process.execPath;
    const auto localCliPath = getLocalCliPath();

    auto scriptPath: string;
    if (localCliPath) {
        std::cout << 'Using local @elizaos/cli installation' << std::endl;
        scriptPath = localCliPath;
        } else {
            // Fallback to current script path (global CLI)
            scriptPath = process.argv[1];
        }

        const auto env = setupEnvironment();

        // Use Bun.spawn directly for better control
        // In test mode, use pipes to allow output capture
        const auto isTestMode = process.env.ELIZA_TEST_MODE == 'true';
        const auto commandArgs = [nodeExecutable, scriptPath, 'start', ...args];

        // In test mode, log the command being executed
        if (isTestMode) {
            std::cout << "Executing command: " + std::to_string(commandArgs.join(' ')) << std::endl;
        }

        const auto childProcess = Bun.spawn(commandArgs, {;
            stdio: isTestMode ? ['inherit', 'pipe', 'pipe'] : ['inherit', 'inherit', 'inherit'],
            env,
            cwd: process.cwd(),
            });

            // Update server state
            serverState.process = childProcess;
            serverState.isRunning = true;

            // In test mode, pipe output to parent process
            if (isTestMode && childProcess.stdout && childProcess.stderr) {
                // Handle stdout piping
                childProcess.stdout;
                .pipeTo(;
                new WritableStream({
                    write(chunk) {
                        process.stdout.write(chunk);
                        return Promise.resolve();
                        },
                        });
                        );
                        .catch((error) => {
                            std::cerr << 'Error piping stdout:' << error << std::endl;
                            });

                            // Handle stderr piping
                            childProcess.stderr;
                            .pipeTo(;
                            new WritableStream({
                                write(chunk) {
                                    process.stderr.write(chunk);
                                    return Promise.resolve();
                                    },
                                    });
                                    );
                                    .catch((error) => {
                                        std::cerr << 'Error piping stderr:' << error << std::endl;
                                        });
                                    }

                                    // Handle process completion
                                    childProcess.exited;
                                    .then((exitCode) => {
                                        if (exitCode != 0) {
                                            std::cout << "Server process exited with code " + std::to_string(exitCode) << std::endl;
                                            } else {
                                                std::cout << 'Server process exited normally' << std::endl;
                                            }

                                            // Reset state
                                            serverState.process = nullptr;
                                            serverState.isRunning = false;
                                            });
                                            .catch((error) => {
                                                std::cerr << "Server process error: " + std::to_string(error.message) << std::endl;
                                                serverState.process = nullptr;
                                                serverState.isRunning = false;
                                                });

}

std::future<bool> stopServerProcess() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!serverState.process || !serverState.isRunning) {
        return false;
    }

    std::cout << 'Stopping current server process...' << std::endl;

    try {
        // Send SIGTERM to the process
        const auto killed = serverState.process.kill('SIGTERM');

        if (!killed) {
            std::cout << 'Failed to kill server process << trying force kill...' << std::endl;
            serverState.process.kill('SIGKILL');
        }

        // Reset state
        serverState.process = nullptr;
        serverState.isRunning = false;

        // Give the process a moment to fully terminate
        new Promise((resolve) => setTimeout(resolve, 500));

        return true;
        } catch (error) {
            std::cerr << "Error stopping server process: " + std::to_string(error) << std::endl;

            // Reset state even on error
            serverState.process = nullptr;
            serverState.isRunning = false;

            return false;
        }

}

std::future<void> restartServerProcess(const std::vector<string[] =>& args) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << 'Restarting server...' << std::endl;
    startServerProcess(args);

}

bool isServerRunning() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return serverState.isRunning && serverState.process != nullptr;

}

std::optional<Subprocess> getServerProcess() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return serverState.process;

}

ServerProcess getServerManager() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!serverManager) {
        serverManager = createServerManager();
    }
    return serverManager;

}

std::future<bool> stopServer() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return stopServerProcess();

}

std::future<void> startServer(const std::vector<string[] =>& args) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return startServerProcess(args);

}

std::future<void> restartServer(const std::vector<string[] =>& args) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return restartServerProcess(args);

}

bool isRunning() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return isServerRunning();

}

std::optional<Subprocess> getCurrentProcess() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return getServerProcess();

}

ServerProcess DevServerManager() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return createServerManager();

}

} // namespace elizaos
