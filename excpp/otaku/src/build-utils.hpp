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

#!/usr/bin/env bun
/**
 * Common build utilities for Bun.build across the monorepo
 */

import type { BuildConfig, BunPlugin } from 'bun';
;
;
;

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
    std::optional<bool> isCli;
    std::optional<bool> generateDts;
};


/**
 * Get performance timer
 */
,
    elapsedMs: () => {
      const end = performance.now();
      return Math.round(end - start);
    },
  };
}

/**
 * Creates a path alias resolver plugin for Bun.build
 * Resolves @/ paths to be external imports that will be resolved at runtime
 */
, (args) => {
        // Leave the import as-is but mark it as external
        // The package.json "imports" field will resolve it at runtime
        return { path: args.path, external: true };
      });
    },
  };
}

/**
 * Creates a standardized Bun build configuration for ElizaOS packages
 */
std::future<BuildConfig> createElizaBuildConfig(ElizaBuildOptions options); = options;

  // Resolve paths relative to root
  const resolvedEntrypoints = entrypoints
    .filter((entry) => entry && entry.trim() !== '') // Filter out empty strings
    .map((entry) => (entry.startsWith('./') ? entry : `./${entry}`));

  // Common external packages for Node.js targets
  const nodeExternals =
    target === 'node' || target === 'bun'
      ? [
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
        ]
      : [];

  // ElizaOS workspace packages that should typically be external
  const elizaExternals = [
    '@elizaos/core',
    '@elizaos/server',
    '@elizaos/client',
    '@elizaos/api-client',
    '@elizaos/plugin-*',
  ];

  // Filter out empty strings and clean up the external array
  const cleanExternals = [...external].filter(
    (ext) => ext && !ext.startsWith('//') && ext.trim() !== ''
  );

  // Add path alias plugin to all builds
  const allPlugins = [createPathAliasPlugin(root), ...plugins];

  const config: BuildConfig = {
    entrypoints: resolvedEntrypoints,
    outdir,
    target: target === 'node' ? 'node' : target,
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

/**
 * Copy assets after build with proper error handling (parallel processing)
 */
async >) {
  if (!assets.length) return;

  const timer = getTimer();
  const { cp } = await import('node:fs/promises');

  console.log('Copying assets...');

  // Process all assets in parallel
  const copyPromises = assets.map(async (asset) => {
    const assetTimer = getTimer();
    try {
      if (existsSync(asset.from)) {
        await cp(asset.from, asset.to, { recursive: true });
        return {
          success: true,
          message: `Copied ${asset.from} to ${asset.to} (${assetTimer.elapsed()}ms)`,
          asset,
        };
      } else {
        return {
          success: false,
          message: `Source not found: ${asset.from}`,
          asset,
          error: 'Source not found',
        };
      }
    } catch (error: unknown) {
      const errorMessage = error instanceof Error ? error.message : String(error);
      return {
        success: false,
        message: `Failed to copy ${asset.from} to ${asset.to}: ${errorMessage}`,
        asset,
        error: errorMessage,
      };
    }
  });

  // Wait for all copies to complete
  const results = await Promise.all(copyPromises);

  // Process results
  let successCount = 0;
  let failedAssets: Array<{ asset: { from: string; to: string }; error: string }> = [];

  results.forEach((result) => {
    if (result.success) {
      successCount++;
    } else {
      console.warn(`   ${result.message}`);
      if (result.error) {
        // Check for specific error types
        if (result.error.includes('EACCES') || result.error.includes('EPERM')) {
          console.error(`    Permission denied. Try running with elevated privileges.`);
        } else if (result.error.includes('ENOSPC')) {
          console.error(`    Insufficient disk space.`);
        }
        failedAssets.push({ asset: result.asset, error: result.error });
      }
    }
  });

  const totalTime = timer.elapsed();

  if (failedAssets.length === 0) {
    console.log(` Assets copied (${totalTime}ms)`);
  } else if (successCount > 0) {
    console.warn(` Copied ${successCount}/${assets.length} assets (${totalTime}ms)`);
    console.warn(`  Failed assets: ${failedAssets.map((f) => f.asset.from).join(', ')}`);
  } else {
    throw new Error(
      `Failed to copy all assets. Errors: ${failedAssets.map((f) => `${f.asset.from}: ${f.error}`).join('; ')}`
    );
  }
}

/**
 * Generate TypeScript declarations using tsc
 */
async  = await import('bun');

  if (!existsSync(tsconfigPath)) {
    console.warn(`TypeScript config not found at ${tsconfigPath}, skipping d.ts generation`);
    return;
  }

  console.log('Generating TypeScript declarations...');
  try {
    // Use incremental compilation for faster subsequent builds
    await $`tsc --emitDeclarationOnly --project ${tsconfigPath} --composite false --incremental false --types node,bun`;
    console.log(` TypeScript declarations generated successfully (${timer.elapsed()}ms)`);
  } catch (error: unknown) {
    console.error(` Failed to generate TypeScript declarations (${timer.elapsed()}ms)`);
    console.error('Error details:', error instanceof Error ? error.message : String(error));

    if (throwOnError) {
      // Propagate so calling build fails hard on TS errors
      throw error;
    }
    console.warn('Continuing build without TypeScript declarations...');
  }
}

/**
 * Clean build artifacts with proper error handling and retry logic
 */
async  = await import('node:fs/promises');

  if (!existsSync(outdir)) {
    console.log(` ${outdir} directory already clean (${timer.elapsed()}ms)`);
    return;
  }

  let lastError: unknown;

  for (let attempt = 1; attempt <= maxRetries; attempt++) {
    try {
      await rm(outdir, { recursive: true, force: true });
      console.log(` Cleaned ${outdir} directory (${timer.elapsed()}ms)`);
      return; // Success, exit the function
    } catch (error: unknown) {
      lastError = error;
      const errorMessage = error instanceof Error ? error.message : String(error);

      // Check for specific error types
      if (errorMessage.includes('EACCES') || errorMessage.includes('EPERM')) {
        console.error(` Permission denied while cleaning ${outdir}`);
        console.error(`  Try running with elevated privileges or check file permissions.`);
        throw error; // Don't retry permission errors
      } else if (errorMessage.includes('ENOENT')) {
        // Directory was already deleted (possibly by concurrent process)
        console.log(` ${outdir} directory was already removed (${timer.elapsed()}ms)`);
        return;
      } else if (errorMessage.includes('EBUSY') || errorMessage.includes('EMFILE')) {
        // Resource busy or too many open files - these might be temporary
        if (attempt < maxRetries) {
          const waitTime = attempt * 500; // Exponential backoff: 500ms, 1000ms, 1500ms
          console.warn(
            ` Failed to clean ${outdir} (attempt ${attempt}/${maxRetries}): ${errorMessage}`
          );
          console.warn(`  Retrying in ${waitTime}ms...`);
          await new Promise((resolve) => setTimeout(resolve, waitTime));
        }
      } else {
        // Unknown error
        console.error(` Failed to clean ${outdir}: ${errorMessage}`);
        throw error;
      }
    }
  }

  // If we've exhausted all retries
  const finalError = lastError instanceof Error ? lastError : new Error(String(lastError));
  console.error(` Failed to clean ${outdir} after ${maxRetries} attempts`);
  throw finalError;
}

/**
 * Watch files for changes and trigger rebuilds with proper cleanup
 */
 = {}
): () => void {
  const { extensions = ['.ts', '.js', '.tsx', '.jsx'], debounceMs = 100 } = options;

  let debounceTimer: NodeJS.Timeout | null = null;
  let watcher: FSWatcher | null = null;
  let isCleanedUp = false;

  console.log(` Watching ${directory} for changes...`);
  console.log(' Press Ctrl+C to stop\n');

  // Cleanup 

    if (watcher) {
      try {
        watcher.close();
      } catch (error) {
        // Ignore errors during cleanup
      }
      watcher = null;
    }
  };

  try {
    // Create the watcher with proper error handling
    watcher = watch(directory, { recursive: true }, (eventType, filename) => {
      if (isCleanedUp) return;

      if (filename && extensions.some((ext) => filename.endsWith(ext))) {
        // Debounce to avoid multiple rapid rebuilds
        if (debounceTimer) {
          clearTimeout(debounceTimer);
        }

        debounceTimer = setTimeout(() => {
          if (!isCleanedUp) {
            console.log(`\n File changed: ${filename}`);
            onChange();
          }
        }, debounceMs);
      }
    });

    // Handle watcher errors
    if (watcher && typeof watcher.on === 'function') {
      watcher.on('error', (error: Error) => {
        console.error('Watch error:', error.message);
        if (error.message.includes('EMFILE')) {
          console.error(
            'Too many open files. Consider increasing your system limits or reducing the watch scope.'
          );
        }
      });
    }
  } catch (error: unknown) {
    const errorMessage = error instanceof Error ? error.message : String(error);
    console.error(`Failed to start file watcher: ${errorMessage}`);

    if (errorMessage.includes('EMFILE')) {
      console.error('\n  Too many open files error detected!');
      console.error('Try one of these solutions:');
      console.error('  1. Increase system file limit: ulimit -n 4096');
      console.error('  2. Close other applications using file watchers');
      console.error('  3. Reduce the scope of watched directories');
    }

    throw error;
  }

  // Register cleanup handlers only once per watcher
  const handleExit = () => {
    cleanup();
    console.log('\n\n Stopping watch mode...');
    process.exit(0);
  };

  // Remove any existing handlers to avoid duplicates
  process.removeAllListeners('SIGINT');
  process.removeAllListeners('SIGTERM');

  // Add new handlers
  process.once('SIGINT', handleExit);
  process.once('SIGTERM', handleExit);

  // Also cleanup on normal exit
  process.once('exit', cleanup);

  // Return cleanup ;


/**
 * Run a build with optional watch mode support
 */
async ) {
  const { packageName, buildOptions, isRebuild = false, onBuildComplete } = options;
  const totalTimer = getTimer();

  // Clear console and show timestamp for rebuilds
  if (isRebuild) {
    console.clear();
    const timestamp = new Date().toLocaleTimeString();
    console.log(`[${timestamp}]  Rebuilding ${packageName}...\n`);
  } else {
    console.log(` Building ${packageName}...\n`);
  }

  try {
    // Clean previous build
    await cleanBuild(buildOptions.outdir);

    // Create build configuration
    const configTimer = getTimer();
    const config = await createElizaBuildConfig(buildOptions);
    console.log(` Configuration prepared (${configTimer.elapsed()}ms)`);

    // Build with Bun
    console.log('\nBundling with Bun...');
    const buildTimer = getTimer();
    const result = await Bun.build(config);

    if (!result.success) {
      console.error(' Build failed:', result.logs);
      onBuildComplete?.(false);
      return false;
    }

    const totalSize = result.outputs.reduce((sum, output) => sum + output.size, 0);
    const sizeMB = (totalSize / 1024 / 1024).toFixed(2);
    console.log(
      ` Built ${result.outputs.length} file(s) - ${sizeMB}MB (${buildTimer.elapsed()}ms)`
    );

    // Run post-build tasks
    const postBuildTasks: Promise<void | null>[] = [];

    // Add TypeScript declarations generation if requested
    if (buildOptions.generateDts) {
      postBuildTasks.push(
        generateDts('./tsconfig.build.json').catch((err) => {
          console.error(' TypeScript declarations generation failed:', err);
          // Don't throw here, as it's often non-critical
          return null;
        })
      );
    }

    // Add asset copying if specified
    if (buildOptions.assets?.length) {
      postBuildTasks.push(
        copyAssets(buildOptions.assets).catch((err) => {
          console.error(' Asset copying failed:', err);
          throw err; // Asset copying failure is critical
        })
      );
    }

    // Execute all post-build tasks
    if (postBuildTasks.length > 0) {
      const postBuildTimer = getTimer();
      await Promise.all(postBuildTasks);
      console.log(` Post-build tasks completed (${postBuildTimer.elapsed()}ms)`);
    }

    console.log(`\n ${packageName} build complete!`);
    console.log(`  Total build time: ${totalTimer.elapsed()}ms`);

    onBuildComplete?.(true);
    return true;
  } catch (error) {
    console.error('Build error:', error);
    onBuildComplete?.(false);
    return false;
  }
}

/**
 * Create a standardized build runner with watch mode support
 */
);
  }

  async );
      } catch (error: unknown) {
        console.error('Failed to start watch mode:', error);
        process.exit(1);
      }
    }
  }

  // Ensure cleanup on process exit
  const cleanup = () => {
    if (cleanupWatcher) {
      cleanupWatcher();
      cleanupWatcher = null;
    }
  };

  process.once('beforeExit', cleanup);
  process.once('SIGUSR1', cleanup);
  process.once('SIGUSR2', cleanup);

  // Return the main  else {
      const success = await build();
      if (!success) {
        process.exit(1);
      }
    }
  };
}

} // namespace elizaos
