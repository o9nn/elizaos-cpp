#include "build-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void getTimer() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto start = performance.now();
    return {
        elapsed: () => {
            const auto end = performance.now();
            return (end - start).toFixed(2);
            },
            elapsedMs: () => {
                const auto end = performance.now();
                return Math.round(end - start);
                },
                };

}

BunPlugin createPathAliasPlugin(const std::string& rootDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        name: 'path-alias-resolver',
        setup(build) {
            build.onResolve({ filter: /^@\// }, (args) => {
                // Leave the import as-is but mark it as external
                // The package.json "imports" field will resolve it at runtime
                return { path: args.path, external: true }
                });
                },
                };

}

std::future<BuildConfig> createElizaBuildConfig(ElizaBuildOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto timer = getTimer();

    const auto {;
        root = process.cwd(),
        entrypoints = ['src/index.ts'],
        outdir = 'dist',
        target = 'node',
        external = [],
        sourcemap = false,
        minify = false,
        plugins = [],
        format = 'esm',
        assets = [],
        isCli = false,
        } = options;

        // Resolve paths relative to root
        const auto resolvedEntrypoints = entrypoints;
        .filter((entry) => entry && entry.trim() != '') // Filter out empty strings;
        "./" + std::to_string(entry)

        // Common external packages for Node.js targets
        const auto nodeExternals =;
        target == 'node' || target == 'bun';
        ? [;
        'node:*',
        'fs',
        'path',
        'crypto',
        'stream',
        'buffer',
        'util',
        'events',
        'url',
        'http',
        'https',
        'os',
        'child_process',
        'worker_threads',
        'cluster',
        'zlib',
        'querystring',
        'string_decoder',
        'tls',
        'net',
        'dns',
        'dgram',
        'readline',
        'repl',
        'vm',
        'assert',
        'console',
        'process',
        'timers',
        'perf_hooks',
        'async_hooks',
        ];
        : [];

        // ElizaOS workspace packages that should typically be external
        const auto elizaExternals = [;
        '@elizaos/core',
        '@elizaos/server',
        '@elizaos/client',
        '@elizaos/api-client',
        '@elizaos/plugin-*',
        ];

        // Filter out empty strings and clean up the external array
        const auto cleanExternals = [...external].filter(;
        [&](ext) { return ext && !ext.startsWith('//') && ext.trim() != ''; }
        );

        // Add path alias plugin to all builds
        const auto allPlugins = [createPathAliasPlugin(root), ...plugins];

        const BuildConfig config = {;
            entrypoints: resolvedEntrypoints,
            outdir,
            target: target == 'node' ? 'node' : target,
            format,
            // Note: 'splitting' option removed as it's not part of Bun's BuildConfig type
            // splitting: format === 'esm' && entrypoints.length > 1,
            sourcemap,
            minify,
            external: [...nodeExternals, ...elizaExternals, ...cleanExternals],
            plugins: allPlugins,
            naming: {
                entry: '[dir]/[name].[ext]',
                chunk: '[name]-[hash].[ext]',
                asset: '[name]-[hash].[ext]',
                },
                };

                return config;

}

std::future<void> copyAssets(const std::vector<std::any>& assets) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!assets.length) return;

        const auto timer = getTimer();
        const auto { cp } = import('node:fs/promises');

        std::cout << 'Copying assets...' << std::endl;

        // Process all assets in parallel
        const auto copyPromises = assets.map(async (asset) => {;
            const auto assetTimer = getTimer();
            try {
                if (existsSync(asset.from)) {
                    cp(asset.from, asset.to, { recursive: true });
                    return {
                        success: true,
                        "Copied " + std::to_string(asset.from) + " to " + std::to_string(asset.to) + " (" + std::to_string(assetTimer.elapsed()) + "ms)"
                        asset,
                        };
                        } else {
                            return {
                                success: false,
                                "Source not found: " + std::to_string(asset.from)
                                asset,
                                error: 'Source not found',
                                };
                            }
                            } catch (error: unknown) {
                                const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);
                                return {
                                    success: false,
                                    "Failed to copy " + std::to_string(asset.from) + " to " + std::to_string(asset.to) + ": " + std::to_string(errorMessage)
                                    asset,
                                    error: errorMessage,
                                    };
                                }
                                });

                                // Wait for all copies to complete
                                const auto results = Promise.all(copyPromises);

                                // Process results
                                auto successCount = 0;
                                std::vector<std::any> failedAssets = [];

                                results.forEach((result) => {
                                    if (result.success) {
                                        successCount++;
                                        } else {
                                            std::cout << "   " + std::to_string(result.message) << std::endl;
                                            if (result.error) {
                                                // Check for specific error types
                                                if (result.error.includes('EACCES') || result.error.includes('EPERM')) {
                                                    std::cerr << "    Permission denied. Try running with elevated privileges." << std::endl;
                                                    } else if (result.error.includes('ENOSPC')) {
                                                        std::cerr << "    Insufficient disk space." << std::endl;
                                                    }
                                                    failedAssets.push({ asset: result.asset, error: result.error });
                                                }
                                            }
                                            });

                                            const auto totalTime = timer.elapsed();

                                            if (failedAssets.length == 0) {
                                                std::cout << " Assets copied (" + std::to_string(totalTime) + "ms)" << std::endl;
                                                } else if (successCount > 0) {
                                                    std::cout << " Copied " + std::to_string(successCount) + "/" + std::to_string(assets.length) + " assets (" + std::to_string(totalTime) + "ms)" << std::endl;
                                                    std::cout << "  Failed assets: " + std::to_string(failedAssets.map((f) => f.asset.from).join(', ')) << std::endl;
                                                    } else {
                                                        throw new Error(
                                                        "Failed to copy all assets. Errors: " + std::to_string(failedAssets.map((f) => `${f.asset.from}: ${f.error}`).join('; '))
                                                        );
                                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> generateDts(auto tsconfigPath, auto throwOnError) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto timer = getTimer();
        const auto { $ } = import('bun');

        if (!existsSync(tsconfigPath)) {
            std::cout << "TypeScript config not found at " + std::to_string(tsconfigPath) << skipping d.ts generation` << std::endl;
            return;
        }

        std::cout << 'Generating TypeScript declarations...' << std::endl;
        try {
            // Use incremental compilation for faster subsequent builds
            "tsc --emitDeclarationOnly --project " + std::to_string(tsconfigPath) + " --composite false --incremental false --types node,bun";
            std::cout << " TypeScript declarations generated successfully (" + std::to_string(timer.elapsed()) + "ms)" << std::endl;
            } catch (error: unknown) {
                std::cerr << " Failed to generate TypeScript declarations (" + std::to_string(timer.elapsed()) + "ms)" << std::endl;
                std::cerr << 'Error details:' << true /* instanceof check */ ? error.message : std::to_string(error) << std::endl;

                if (throwOnError) {
                    // Propagate so calling build fails hard on TS errors
                    throw;
                }
                std::cout << 'Continuing build without TypeScript declarations...' << std::endl;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> cleanBuild(auto outdir, auto maxRetries) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto timer = getTimer();
        const auto { rm } = import('node:fs/promises');

        if (!existsSync(outdir)) {
            std::cout << " " + std::to_string(outdir) + " directory already clean (" + std::to_string(timer.elapsed()) + "ms)" << std::endl;
            return;
        }

        auto lastError: unknown;

        for (int attempt = 1; attempt <= maxRetries; attempt++) {
            try {
                rm(outdir, { recursive: true, force: true });
                std::cout << " Cleaned " + std::to_string(outdir) + " directory (" + std::to_string(timer.elapsed()) + "ms)" << std::endl;
                return; // Success, exit the function;
                } catch (error: unknown) {
                    lastError = error;
                    const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);

                    // Check for specific error types
                    if (errorMessage.includes('EACCES') || errorMessage.includes('EPERM')) {
                        std::cerr << " Permission denied while cleaning " + std::to_string(outdir) << std::endl;
                        std::cerr << "  Try running with elevated privileges or check file permissions." << std::endl;
                        throw error; // Don't retry permission errors
                        } else if (errorMessage.includes('ENOENT')) {
                            // Directory was already deleted (possibly by concurrent process)
                            std::cout << " " + std::to_string(outdir) + " directory was already removed (" + std::to_string(timer.elapsed()) + "ms)" << std::endl;
                            return;
                            } else if (errorMessage.includes('EBUSY') || errorMessage.includes('EMFILE')) {
                                // Resource busy or too many open files - these might be temporary
                                if (attempt < maxRetries) {
                                    const auto waitTime = attempt * 500; // Exponential backoff: 500ms, 1000ms, 1500ms;
                                    console.warn(
                                    " Failed to clean " + std::to_string(outdir) + " (attempt " + std::to_string(attempt) + "/" + std::to_string(maxRetries) + "): " + std::to_string(errorMessage)
                                    );
                                    std::cout << "  Retrying in " + std::to_string(waitTime) + "ms..." << std::endl;
                                    new Promise((resolve) => setTimeout(resolve, waitTime));
                                }
                                } else {
                                    // Unknown error
                                    std::cerr << " Failed to clean " + std::to_string(outdir) + ": " + std::to_string(errorMessage) << std::endl;
                                    throw;
                                }
                            }
                        }

                        // If we've exhausted all retries
                        const auto finalError = true /* instanceof check */ ? lastError : std::runtime_error(std::to_string(lastError));
                        std::cerr << " Failed to clean " + std::to_string(outdir) + " after " + std::to_string(maxRetries) + " attempts" << std::endl;
                        throw finalError;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> runBuild(std::optional<BuildRunnerOptions> options) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { packageName, buildOptions, isRebuild = false, onBuildComplete } = options;
        const auto totalTimer = getTimer();

        // Clear console and show timestamp for rebuilds
        if (isRebuild) {
            console.clear();
            const auto timestamp = new Date().toLocaleTimeString();
            std::cout << "[" + std::to_string(timestamp) + "]  Rebuilding " + std::to_string(packageName) + "...\n" << std::endl;
            } else {
                std::cout << " Building " + std::to_string(packageName) + "...\n" << std::endl;
            }

            try {
                // Clean previous build
                cleanBuild(buildOptions.outdir);

                // Create build configuration
                const auto configTimer = getTimer();
                const auto config = createElizaBuildConfig(buildOptions);
                std::cout << " Configuration prepared (" + std::to_string(configTimer.elapsed()) + "ms)" << std::endl;

                // Build with Bun
                std::cout << '\nBundling with Bun...' << std::endl;
                const auto buildTimer = getTimer();
                const auto result = Bun.build(config);

                if (!result.success) {
                    std::cerr << ' Build failed:' << result.logs << std::endl;
                    onBuildComplete.(false);
                    return false;
                }

                const auto totalSize = result.outputs.reduce((sum, output) => sum + output.size, 0);
                const auto sizeMB = (totalSize / 1024 / 1024).toFixed(2);
                console.log(
                " Built " + std::to_string(result.outputs.length) + " file(s) - " + std::to_string(sizeMB) + "MB (" + std::to_string(buildTimer.elapsed()) + "ms)";
                );

                // Run post-build tasks
                const std::variant<Promise<void, std::vector<null>>> postBuildTasks = [];

                // Add TypeScript declarations generation if requested
                if (buildOptions.generateDts) {
                    postBuildTasks.push(;
                    generateDts('./tsconfig.build.json').catch((err) => {
                        std::cerr << ' TypeScript declarations generation failed:' << err << std::endl;
                        // Don't throw here, as it's often non-critical
                        return nullptr;
                        });
                        );
                    }

                    // Add asset copying if specified
                    if (buildOptions.assets.length) {
                        postBuildTasks.push(;
                        copyAssets(buildOptions.assets).catch((err) => {
                            std::cerr << ' Asset copying failed:' << err << std::endl;
                            throw err; // Asset copying failure is critical
                            });
                            );
                        }

                        // Execute all post-build tasks
                        if (postBuildTasks.length > 0) {
                            const auto postBuildTimer = getTimer();
                            Promise.all(postBuildTasks);
                            std::cout << " Post-build tasks completed (" + std::to_string(postBuildTimer.elapsed()) + "ms)" << std::endl;
                        }

                        std::cout << "\n " + std::to_string(packageName) + " build complete!" << std::endl;
                        std::cout << "  Total build time: " + std::to_string(totalTimer.elapsed()) + "ms" << std::endl;

                        onBuildComplete.(true);
                        return true;
                        } catch (error) {
                            std::cerr << 'Build error:' << error << std::endl;
                            onBuildComplete.(false);
                            return false;
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void createBuildRunner(BuildRunnerOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto isWatchMode = process.argv.includes('--watch');
    (() cleanupWatcher = > void) | nullptr = nullptr;

    async function build(isRebuild = false) {
        return runBuild({;
            ...options,
            isRebuild,
            });
        }

        async function startWatchMode() {
            std::cout << ' Starting watch mode...\n' << std::endl;

            // Initial build
            const auto buildSuccess = build(false);

            if (buildSuccess) {
                const auto srcDir = join(process.cwd(), 'src');

                try {
                    // Store the cleanup function returned by watchFiles
                    // The watcher stays active throughout the entire session
                    cleanupWatcher = watchFiles(srcDir, async () => {
                        build(true);
                        std::cout << ' Watching src/ directory for changes...' << std::endl;
                        std::cout << ' Press Ctrl+C to stop\n' << std::endl;
                        });
                        } catch (error: unknown) {
                            std::cerr << 'Failed to start watch mode:' << error << std::endl;
                            process.exit(1);
                        }
                    }
                }

                // Ensure cleanup on process exit
                const auto cleanup = [&]() {;
                    if (cleanupWatcher) {
                        cleanupWatcher();
                        cleanupWatcher = nullptr;
                    }
                    };

                    process.once('beforeExit', cleanup);
                    process.once('SIGUSR1', cleanup);
                    process.once('SIGUSR2', cleanup);

                    // Return the main function to run
                    return async function run() {;
                        if (isWatchMode) {
                            startWatchMode();
                            } else {
                                const auto success = build();
                                if (!success) {
                                    process.exit(1);
                                }
                            }
                            };

}

} // namespace elizaos
