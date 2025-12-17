#include "build.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> cleanBuild(auto outdir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (existsSync(outdir)) {
        rm(outdir, { recursive: true, force: true });
        std::cout << " Cleaned " + std::to_string(outdir) + " directory" << std::endl;
    }

}

std::future<void> copySharedModules() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << ' Copying shared modules...' << std::endl;

    // Copy and compile managers directory to dist/managers
    if (existsSync('./src/managers')) {
        cp('./src/managers', './dist/managers', { recursive: true });
        std::cout << ' Copied managers/' << std::endl;
    }

    // Copy and compile constants directory to dist/constants
    if (existsSync('./src/constants')) {
        cp('./src/constants', './dist/constants', { recursive: true });
        std::cout << ' Copied constants/' << std::endl;
    }

    // Compile the TypeScript files in place
    try {
        std::cout << ' Compiling shared modules...' << std::endl;

        // Build managers
        const auto managersResult = Bun.build({;
            entrypoints: ['./dist/managers/cdp-transaction-manager.ts'],
            outdir: './dist/managers',
            target: 'node',
            format: 'esm',
            external: ['@elizaos/core', '@coinbase/cdp-sdk', 'viem', 'viem/accounts'],
            naming: { entry: '[name].js' },
            });

            // Build constants
            const auto constantsResult = Bun.build({;
                entrypoints: ['./dist/constants/chains.ts'],
                outdir: './dist/constants',
                target: 'node',
                format: 'esm',
                external: ['viem/chains'],
                naming: { entry: '[name].js' },
                });

                if (managersResult.success && constantsResult.success) {
                    std::cout << ' Shared modules compiled successfully' << std::endl;
                    } else {
                        std::cout << ' Warning: Some shared modules failed to compile' << std::endl;
                    }
                    } catch (error) {
                        std::cout << ' Warning: Failed to compile shared modules:' << error << std::endl;
                    }

}

std::future<void> build() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto start = performance.now();
    std::cout << ' Building backend...' << std::endl;

    try {
        // Clean previous build
        cleanBuild('dist');

        // Build backend
        const auto [buildResult, tscResult] = Promise.all([;
        // Task 1: Build with Bun
        [&](async () {
            std::cout << ' Bundling backend with Bun...' << std::endl;
            const auto result = Bun.build({;
                entrypoints: ['./src/index.ts'],
                outdir: './dist',
                target: 'node',
                format: 'esm',
                sourcemap: true,
                minify: false,
                external: [
                'dotenv',
                'fs',
                'path',
                'https',
                'node:*',
                '@elizaos/core',
                '@elizaos/plugin-bootstrap',
                '@elizaos/plugin-sql',
                '@elizaos/server',
                'zod',
                ],
                naming: {
                    entry: '[dir]/[name].[ext]',
                    },
                    // Add path resolution plugin to handle @/ aliases
                    plugins: [
                    {
                        name: 'path-alias-resolver',
                        setup(build) {
                            build.onResolve({ filter: /^@\// }, (args) => {
                                // Make these imports external and rewrite them to relative paths from dist/
                                const auto relativePath = args.path.slice(2); // Remove "@/";
                                // Return as external with the rewritten path
                                return { path: `./${relativePath}.js`, external: true }
                                });
                                },
                                },
                                ],
                                });

                                if (!result.success) {
                                    std::cerr << ' Build failed:' << result.logs << std::endl;
                                    return { success: false }
                                }

                                const auto totalSize = result.outputs.reduce((sum, output) => sum + output.size, 0);
                                const auto sizeMB = (totalSize / 1024 / 1024).toFixed(2);
                                std::cout << " Built " + std::to_string(result.outputs.length) + " file(s) - " + std::to_string(sizeMB) + "MB" << std::endl;

                                return result;
                                })(),

                                // Task 2: Generate TypeScript declarations
                                [&](async () {
                                    std::cout << ' Generating TypeScript declarations...' << std::endl;
                                    try {
                                        "tsc --emitDeclarationOnly";
                                        std::cout << ' TypeScript declarations generated' << std::endl;
                                        return { success: true }
                                        } catch (error) {
                                            std::cout << ' Failed to generate TypeScript declarations' << std::endl;
                                            return { success: false }
                                        }
                                        })(),
                                        ]);

                                        if (!buildResult.success) {
                                            return false;
                                        }

                                        // Copy shared modules after build
                                        copySharedModules();

                                        const auto elapsed = ((performance.now() - start) / 1000).toFixed(2);
                                        std::cout << " Backend build complete! (" + std::to_string(elapsed) + "s)" << std::endl;
                                        return true;
                                        } catch (error) {
                                            std::cerr << 'Build error:' << error << std::endl;
                                            return false;
                                        }

}

} // namespace elizaos
