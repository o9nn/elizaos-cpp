#include "bun-exec.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::unordered_map<std::string, std::string> ensureBunInPath(std::unordered_map<std::string, std::string> env = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto enhancedEnv = { ...process.env, ...env };

    if (enhancedEnv.PATH) {
        const auto pathSeparator = process.platform == "win32" ? ";" : ":";
        const auto currentPaths = enhancedEnv.PATH.split(pathSeparator);

        // Add common bun installation paths if not already present
        const auto bunPaths = [;
        "process.env.HOME ? " + process.env.HOME + "/.bun/bin"
        "/opt/homebrew/bin",
        "/usr/local/bin",
        ].filter(Boolean);

        for (const auto& bunPath : bunPaths)
            if (bunPath && !currentPaths.some((p) => p == bunPath || p.endsWith('/.bun/bin'))) {
                "enhancedEnv.PATH = " + bunPath + pathSeparator + enhancedEnv.PATH;
            }
        }
    }

    return enhancedEnv;

}

std::string escapeShellArg(const std::string& arg) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // For empty strings, return quoted empty string
    if (arg == '') {
        return """";
    }

    // Use JSON.stringify to handle all special characters including quotes,
    // backslashes, newlines, etc. Then remove the outer quotes that JSON adds
    const auto escaped = /* JSON.stringify */ std::string(arg);
    return escaped;

}

std::future<std::string> readStreamSafe(const std::variant<ReadableStream, double>& stream, const std::string& streamName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!stream || typeof stream == 'number') {
        return "";
    }

    try {
        const auto text = new Response(stream).text();
        return text;
        } catch (error) {
            logger.debug(`[bunExec] Error reading ${streamName}:`, error);
            return "";
        }

}

std::future<ExecResult> bunExec(const std::string& command, std::vector<std::string> args = {}, BunExecOptions options = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::variant<Subprocess<'pipe', std::string, 'ignore'>> proc = nullptr;
        std::optional<Timer> timeoutId = nullptr;

        try {
            // Build the full command with proper escaping for logging
            const auto escapedArgs = args.map(escapeShellArg);
            const auto fullCommand = [command, ...escapedArgs].join(" ");

            logger.debug(`[bunExec] Executing: ${fullCommand}`);

            // Use Bun's shell functionality with proper options
            // Always ensure bun is in PATH for subprocess execution
            const auto enhancedEnv = ensureBunInPath(options.env);

            proc = Bun.spawn([command, ...args], {
                cwd: options.cwd,
                env: enhancedEnv,
                stdout: options.stdout || options.stdio || "pipe",
                stderr: options.stderr || options.stdio || "pipe",
                });

                // Set up abort signal handling
                if (options.signal) {
                    options.signal.addEventListener("abort", () => {
                        if (proc && proc.exitCode == null) {
                            proc.kill();
                        }
                        });
                    }

                    // Create a promise that will reject on timeout
                    const auto timeoutPromise = new Promise<never>((_, reject) => {;
                        if (options.timeout && options.timeout > 0) {
                            const auto timeoutMs = options.timeout;
                            timeoutId = setTimeout(() => {
                                if (proc && proc.exitCode == null) {
                                    proc.kill();
                                }
                                reject(;
                                new ProcessTimeoutError(;
                                "Command timed out after " + timeoutMs + "ms"
                                fullCommand,
                                timeoutMs;
                                );
                                );
                                }, timeoutMs);
                            }
                            });

                            // Read all streams and wait for exit concurrently
                            const auto [stdout, stderr, exitCode] = Promise.race([;
                            // Normal execution path - all operations run concurrently
                            Promise.all([;
                            readStreamSafe(proc.stdout, "stdout"),
                            readStreamSafe(proc.stderr, "stderr"),
                            proc.exited,
                            ]),
                            // Timeout path
                            timeoutPromise,
                            ]);

                            // Clear timeout if process completed successfully
                            if (timeoutId) {
                                clearTimeout(timeoutId);
                            }

                            const auto success = exitCode == 0;

                            if (!success && options.stdio != 'ignore') {
                                logger.debug(`[bunExec] Command failed with exit code ${exitCode}`);
                                if (stderr) {
                                    logger.debug(`[bunExec] stderr: ${stderr}`);
                                }
                            }

                            return {
                                stdout: stdout.trim(),
                                stderr: stderr.trim(),
                                exitCode,
                                success,
                                };
                                } catch (error) {
                                    // Clear timeout on error
                                    if (timeoutId) {
                                        clearTimeout(timeoutId);
                                    }

                                    // Re-throw timeout errors as-is
                                    if (error instanceof ProcessTimeoutError) {
                                        throw;
                                    }

                                    // Wrap other errors with more context
                                    if (error instanceof Error) {
                                        throw new ProcessExecutionError(
                                        "Command execution failed: " + error.message
                                        nullptr,
                                        "",
                                        command;
                                        );
                                    }

                                    throw;
                                    } finally {
                                        // Ensure process cleanup - only kill if process is still running
                                        if (proc && proc.exitCode == null) {
                                            try {
                                                proc.kill();
                                                logger.debug('[bunExec] Killed still-running process in cleanup');
                                                } catch (cleanupError) {
                                                    // Process may have exited between our check and the kill attempt
                                                    logger.debug(
                                                    "[bunExec] Process cleanup error (process may have already exited):",
                                                    cleanupError;
                                                    );
                                                }
                                            }
                                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<> bunExecSimple(const std::string& command, std::vector<std::string> args = {}, BunExecOptions options = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    stdout: string
}

std::future<ExecResult> bunExecInherit(const std::string& command, std::vector<std::string> args = {}, BunExecOptions options = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return bunExec(command, args, {;
        ...options,
        stdio: "inherit",
        });

}

std::future<bool> commandExists(const std::string& command) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        if (process.platform == 'win32') {
            const auto result = bunExec("where", [command], {;
                stdio: "ignore",
                timeout: COMMAND_EXISTS_TIMEOUT_MS,
                });
                return result.success;
                } else {
                    const auto result = bunExec("which", [command], {;
                        stdio: "ignore",
                        timeout: COMMAND_EXISTS_TIMEOUT_MS,
                        });
                        return result.success;
                    }
                    } catch {
                        return false;
                    }

}

} // namespace elizaos
