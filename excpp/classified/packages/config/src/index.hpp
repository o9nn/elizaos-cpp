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
 * Standard configuration exports for ElizaOS packages
 * Provides centralized access to all base configurations
 */
{ default as tsConfigBase } from './typescript/tsconfig.base.json';
{ default as tsConfigPlugin } from './typescript/tsconfig.plugin.json';
{ default as tsConfigFrontend } from './typescript/tsconfig.frontend.json';
{ default as tsConfigTest } from './typescript/tsconfig.test.json';
{ default as eslintConfigPlugin } from './eslint/eslint.config.plugin.js';
{ default as eslintConfigFrontend } from './eslint/eslint.config.frontend.js';
{
  baseConfig as eslintBaseConfig,
  testOverrides,
  standardIgnores,
} from './eslint/eslint.config.base.js';
{ default as prettierConfig } from './prettier/prettier.config.js';
declare const configPaths: {
  typescript: {
    base: string;
    plugin: string;
    frontend: string;
    test: string;
  };
  eslint: {
    plugin: string;
    frontend: string;
  };
  prettier: string;
};

} // namespace elizaos
