#include "elizaos/core.hpp"
#include "elizaos/logger.hpp"
#include "elizaos/plugin-sql.hpp"
#include "elizaos/types.hpp"
#include "run-bun.hpp"
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



// Configuration

struct CreationResult {
    bool success;
    std::string pluginName;
    std::string pluginPath;
    std::optional<Error> error;
};

struct PluginSpecification {
    std::string name;
    std::string description;
    std::vector<std::string> features;
    std::optional<std::vector<std::string>> actions;
    std::optional<std::vector<std::string>> providers;
    std::optional<std::vector<std::string>> evaluators;
    std::optional<std::vector<std::string>> services;
};

struct ValidationResult {
    bool production_ready;
    std::optional<std::string> revision_instructions;
};

struct CreatorOptions {
    std::optional<bool> skipTests;
    std::optional<bool> skipValidation;
    std::optional<bool> skipPrompts;
    std::optional<PluginSpecification> spec;
};

class PluginCreator {
  private git: SimpleGit;
  private pluginPath: string | null = null;
  private anthropic: Anthropic | null = null;
  private activeClaudeProcess: any = null;
  private options: CreatorOptions;

  constructor(options: CreatorOptions = {}) {
    this.git = simpleGit();
    this.options = options;
    this.registerCleanupHandlers();
  }

      await cleanup();

      // Check disk space

      // Check for Claude Code
        await execa('claude', ['--version'], { stdio: 'pipe' });

      // Step 1: Collect plugin specification

      // Step 2: Create plugin from template

      // Step 3: Generate detailed specification

      // Step 4: Create PLUGIN_SPEC.md

      // Step 5: Generate plugin code with validation loops

      // Step 6: Copy to current directory

    // Collect specific component details

    // Use elizaos create command to create from template
      // Fallback to manual creation if elizaos create fails

    // Initialize git

    // Create basic structure

    // Create package.json

    // Create tsconfig.json

    // Create tsup.config.ts

    // Bun test doesn't need a separate config file
    // Configuration is handled in package.json

    // No separate config file needed for bun test

    // Create .gitignore
*.log

    // Create README.md

// Add to your ElizaOS configuration

    // Create basic index.ts

// Memory operations - works with both Pglite and PostgreSQL

// Search memories - database-agnostic

// Goal operations - database-agnostic

// ✅ CORRECT - Memory operations

// ✅ CORRECT - Search operations

// ❌ WRONG - Direct database imports

// ✅ CORRECT - All from @elizaos/core

// ❌ WRONG - These packages don't exist

      // Test plugin functionality
    
      // Test plugin functionality

    // Initial code generation

    // Build loop

    // Test loop (if not skipped)

    // Production validation (if not skipped)

        // Re-run build and test loops

      // Install dependencies first using bun
      await runBunCommand(['install'], this.pluginPath!);

      // Run build using bun
      await runBunCommand(['run', 'build'], this.pluginPath!);

      await runBunCommand(['test'], this.pluginPath!);


} // namespace elizaos
