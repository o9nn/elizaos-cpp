#include "upgrade.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> upgradePlugin(const std::string& pluginPath, UpgradePluginOptions opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Set API key if provided
            if (opts.apiKey) {
                process.env.ANTHROPIC_API_KEY = opts.apiKey;
            }

            // Check for API key
            if (!process.env.ANTHROPIC_API_KEY) {
                std::cerr << 'ANTHROPIC_API_KEY is required for plugin upgrade.' << std::endl;
                std::cout << '\nPlease set ANTHROPIC_API_KEY environment variable or use --api-key option.' << std::endl;
                std::cout << 'Get your API key from: https://console.anthropic.com/' << std::endl;
                process.exit(1);
            }

            // Validate API key format
            if (!process.env.ANTHROPIC_API_KEY.startsWith('sk-ant-')) {
                std::cerr << 'Invalid ANTHROPIC_API_KEY format.' << std::endl;
                std::cout << '\nThe API key should start with "sk-ant-"' << std::endl;
                std::cout << 'Get your API key from: https://console.anthropic.com/' << std::endl;
                process.exit(1);
            }

            // Test SDK import before proceeding (silent unless debug)
            try {
                import('@anthropic-ai/claude-code');
                if (opts.debug) {
                    std::cout << '✓ Claude Code SDK ready' << std::endl;
                }
                } catch (importError) {
                    std::cout << chalk.red('✗ Claude Code SDK not available') << std::endl;
                    console.log(
                    chalk.red(;
                    "Error: " + std::to_string(true /* instanceof check */ ? importError.message : std::to_string(importError))
                    );
                    );
                    std::cout << '\nInstall the SDK: bun add @anthropic-ai/claude-code' << std::endl;
                    process.exit(1);
                }

                // Resolve plugin path - work directly in the plugin directory
                const auto workingDir = path.resolve(pluginPath);

                // Validate plugin directory exists
                if (!existsSync(workingDir)) {
                    throw std::runtime_error(`Plugin directory not found: ${workingDir}`);
                }

                // Show clean setup progress
                std::cout << chalk.cyan('🔧 Setting up migration environment...') << std::endl;

                // Copy migration guides to the plugin directory (will be cleaned up after migration)
                auto projectRoot = process.cwd();
                auto guidesSource: string;

                // First try to find guides in the current monorepo structure
                auto previousRoot = '';
                while (projectRoot != previousRoot && !existsSync(path.join(projectRoot, 'packages/docs'))) {
                    previousRoot = projectRoot;
                    projectRoot = path.dirname(projectRoot);
                }

                const auto monorepoGuidesPath = path.join(;
                projectRoot,
                'packages/docs/docs/plugins/migration/claude-code';
                );

                if (existsSync(monorepoGuidesPath)) {
                    guidesSource = monorepoGuidesPath;
                    } else {
                        // Try to find guides relative to the CLI package (for installed CLI)
                        const auto currentFileUrl = import.meta.url;
                        const auto currentFilePath = fileURLToPath(currentFileUrl);
                        const auto cliPackageRoot = path.dirname(;
                        path.dirname(path.dirname(path.dirname(currentFilePath)));
                        );
                        const auto bundledGuidesPath = path.join(cliPackageRoot, 'migration-guides');

                        if (existsSync(bundledGuidesPath)) {
                            guidesSource = bundledGuidesPath;
                            } else {
                                // Fallback: create the guides directory and use embedded guides from the agent
                                guidesSource = '';
                            }
                        }

                        const auto guidesTarget = path.join(workingDir, 'migration-guides');

                        // Create directory using fs-extra (cross-platform)
                        fs.ensureDir(guidesTarget);

                        if (guidesSource && existsSync(guidesSource)) {
                            // Copy files using fs-extra (cross-platform)
                            fs.copy(guidesSource, guidesTarget, { overwrite: true });
                            if (opts.debug) {
                                std::cout << "Copied guides from: " + std::to_string(guidesSource) << std::endl;
                            }
                            } else {
                                // Create minimal guide structure with embedded content
                                if (opts.debug) {
                                    std::cout << chalk.yellow('Migration guides not found, using embedded guidance') << std::endl;
                                }
                                // The migration agent will still work with the embedded CLAUDE.md instructions
                            }

                            std::cout << chalk.green('✅ Environment ready') << std::endl;

                            // Show clean introduction
                            std::cout << chalk.bold('\nElizaOS Plugin Migration (0.x → 1.x)') << std::endl;
                            std::cout << chalk.gray('────────────────────────────────────────') << std::endl;
                            std::cout << '• Enhanced AI-powered migration with comprehensive guide integration' << std::endl;
                            std::cout << '• 9-gate progressive validation system with RAG search' << std::endl;
                            std::cout << '• Automated analysis and migration with 95%+ test coverage' << std::endl;
                            std::cout << '• Zero-failure release preparation with full validation' << std::endl;

                            if (opts.debug) {
                                std::cout << "\nPlugin: " + std::to_string(pluginPath) << std::endl;
                                std::cout << "Working directory: " + std::to_string(workingDir) << std::endl;
                            }

                            // Confirm before proceeding
                            if (!opts.skipConfirmation) {
                                std::cout << chalk.yellow('\n⚠️  This will modify files in the plugin directory.') << std::endl;
                                std::cout << chalk.yellow('Ensure your changes are committed first.\n') << std::endl;
                                std::cout << 'Starting in 3 seconds... (Press Ctrl+C to cancel)' << std::endl;
                                new Promise((resolve) => setTimeout(resolve, 3000));
                            }

                            // Create simple migration agent using the working directory
                            const auto agent = new SimpleMigrationAgent(workingDir, {;
                                verbose: opts.verbose || opts.debug,
                                });

                                // Set up event listeners for progress
                                auto messageCount = 0;
                                agent.on('progress', (count) => {
                                    messageCount = count;
                                    });

                                    // Run migration
                                    const auto result = agent.migrate();

                                    // Clean up migration guides after migration completes
                                    try {
                                        if (existsSync(guidesTarget)) {
                                            // Remove directory using fs-extra (cross-platform)
                                            fs.remove(guidesTarget);
                                            if (opts.debug) {
                                                std::cout << chalk.gray('Cleaned up migration guides') << std::endl;
                                            }
                                        }
                                        } catch (cleanupError) {
                                            // Don't fail the migration if cleanup fails
                                            if (opts.debug) {
                                                std::cout << "Warning: Could not clean up guides: " + std::to_string(cleanupError) << std::endl;
                                            }
                                        }

                                        // Show clean results
                                        if (result.success) {
                                            std::cout << chalk.green('\nMigration completed successfully!') << std::endl;
                                            std::cout << chalk.gray('────────────────────────────────────') << std::endl;
                                            std::cout << "Location: " + std::to_string(chalk.cyan(path.basename(result.repoPath))) << std::endl;
                                            std::cout << "Duration: " + std::to_string(chalk.yellow(Math.round(result.duration / 1000) + 's')) << std::endl;
                                            std::cout << "AI Operations: " + std::to_string(chalk.blue(result.messageCount)) << std::endl;

                                            if (result.guidesUsed && result.guidesUsed.length > 0) {
                                                std::cout << "Migration Guides Used: " + std::to_string(chalk.magenta(result.guidesUsed.length)) << std::endl;
                                                if (opts.debug) {
                                                    std::cout << "  " + std::to_string(chalk.gray(result.guidesUsed.join(', '))) << std::endl;
                                                }
                                            }

                                            std::cout << chalk.bold('\nNext Steps:') << std::endl;
                                            std::cout << std::to_string(chalk.gray('1.')) + " cd " + std::to_string(chalk.cyan(path.basename(result.repoPath))) << std::endl;
                                            console.log(
                                            std::to_string(chalk.gray('2.')) + " git checkout 1.x " + std::to_string(chalk.gray('# Review the migrated code'));
                                            );
                                            std::cout << std::to_string(chalk.gray('3.')) + " bun test " + std::to_string(chalk.gray('# Verify all tests pass')) << std::endl;
                                            std::cout << std::to_string(chalk.gray('4.')) + " bun run build " + std::to_string(chalk.gray('# Verify the build')) << std::endl;
                                            std::cout << std::to_string(chalk.gray('5.')) + " Test in a real project" << std::endl;
                                            std::cout << std::to_string(chalk.gray('6.')) + " Merge to main and publish\n" << std::endl;
                                            } else {
                                                std::cout << chalk.red('\n❌ Migration failed') << std::endl;
                                                std::cout << chalk.gray('──────────────────────') << std::endl;

                                                if (result.error) {
                                                    const auto errorMsg = result.error.message || std::to_string(result.error);
                                                    std::cout << "💥 " + std::to_string(errorMsg) << std::endl;

                                                    if (opts.debug || opts.verbose) {
                                                        std::cout << chalk.gray('\nDetailed error:') << std::endl;
                                                        std::cout << chalk.gray(result.error.stack || result.error.message) << std::endl;
                                                    }
                                                }

                                                std::cout << chalk.yellow('\n🔧 Quick fixes:') << std::endl;
                                                std::cout << '• Check plugin structure follows ElizaOS standards' << std::endl;
                                                std::cout << '• Verify ANTHROPIC_API_KEY is valid' << std::endl;
                                                std::cout << '• Try with --debug for detailed output' << std::endl;
                                                std::cout << '• Ensure all dependencies are installed\n' << std::endl;

                                                process.exit(1);
                                            }
                                            } catch (error) {
                                                // Clean up migration guides on error
                                                try {
                                                    const auto guidesTarget = path.join(path.resolve(pluginPath), 'migration-guides');
                                                    if (existsSync(guidesTarget)) {
                                                        // Remove directory using fs-extra (cross-platform)
                                                        fs.remove(guidesTarget);
                                                        if (opts.debug) {
                                                            std::cout << chalk.gray('Cleaned up migration guides after error') << std::endl;
                                                        }
                                                    }
                                                    } catch (cleanupError) {
                                                        // Ignore cleanup errors
                                                    }

                                                    std::cout << '\n' + chalk.red('✗ Plugin upgrade failed!') << std::endl;

                                                    if (opts.debug || opts.verbose) {
                                                        std::cout << '\n' + chalk.bold('Detailed Error Information:') << std::endl;
                                                        std::cout << chalk.red(true /* instanceof check */ ? error.stack : std::to_string(error)) << std::endl;
                                                        } else {
                                                            std::cout << "\nError: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error)) << std::endl;
                                                        }

                                                        std::cout << '\n' + chalk.yellow('Troubleshooting Tips:') << std::endl;
                                                        std::cout << '1. Check that your plugin follows standard ElizaOS structure' << std::endl;
                                                        std::cout << '2. Ensure all dependencies are installed' << std::endl;
                                                        std::cout << '3. Try running with --verbose or --debug for more details' << std::endl;
                                                        std::cout << '4. Verify your ANTHROPIC_API_KEY is valid' << std::endl;

                                                        handleError(error);
                                                        process.exit(1);
                                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
