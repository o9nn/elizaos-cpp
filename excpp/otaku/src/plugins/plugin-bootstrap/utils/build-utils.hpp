#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Common build utilities for Bun.build across the monorepo
 */

struct ElizaBuildOptions {
    std::optional<std::string> root;
    std::optional<std::vector<std::string>> entrypoints;
    std::optional<std::string> outdir;
    std::optional<'node' | 'bun' | 'browser'> target;
    std::optional<std::vector<std::string>> external;
    std::optional<boolean | 'linked' | 'inline' | 'external'> sourcemap;
    std::optional<bool> minify;
    std::optional<std::vector<BunPlugin>> plugins;
    std::optional<'esm' | 'cjs'> format;
    std::optional<std::vector<{ from: string; to: string }>> assets;

/**
 * Get performance timer
 */
void getTimer();

/**
 * Creates a standardized Bun build configuration for ElizaOS packages
 */
std::future<BuildConfig> createElizaBuildConfig(ElizaBuildOptions options);

/**
 * Copy assets after build with proper error handling (parallel processing)
 */
std::future<void> copyAssets(const std::vector<{ from: string; to: string }>& assets);

/**
 * Generate TypeScript declarations using tsc
 */
std::future<void> generateDts(auto tsconfigPath = './tsconfig.build.json', auto throwOnError = true);

/**
 * Clean build artifacts with proper error handling and retry logic
 */
std::future<void> cleanBuild(auto outdir = 'dist', auto maxRetries = 3);

/**
 * Watch files for changes and trigger rebuilds with proper cleanup
 */

  // Cleanup function to close watcher and clear timers

        // Ignore errors during cleanup

    // Create the watcher with proper error handling

        // Debounce to avoid multiple rapid rebuilds

    // Handle watcher errors

  // Register cleanup handlers only once per watcher

  // Remove any existing handlers to avoid duplicates

  // Add new handlers

  // Also cleanup on normal exit

  // Return cleanup function for manual cleanup

/**
 * Standard build runner configuration
 */
struct BuildRunnerOptions {
    std::string packageName;
    ElizaBuildOptions buildOptions;
    std::optional<(success: boolean) => void> onBuildComplete;
};

/**
 * Run a build with optional watch mode support
 */
std::future<void> runBuild(std::optional<BuildRunnerOptions & { isRebuild: boolean }> options);

/**
 * Create a standardized build runner with watch mode support
 */
void createBuildRunner(BuildRunnerOptions options);

} // namespace elizaos
