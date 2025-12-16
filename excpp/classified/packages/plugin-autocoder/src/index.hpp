#include "__tests__/e2e/index.hpp"
#include "actions/create-project.hpp"
#include "actions/generate-code.hpp"
#include "providers/current-project-provider.hpp"
#include "providers/projects-provider.hpp"
#include "services/CodeGenerationService.hpp"
#include "services/ProjectPlanningService.hpp"
#include "services/ProjectStatusManager.hpp"
#include "services/SecretsManagerService.hpp"
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

import type { Plugin } from '@elizaos/core';
;
;
;
;
;
;
;
;
;

// Export types
* from './types/index';

// Export services
{ CodeGenerationService } from './services/CodeGenerationService';
{ ProjectPlanningService } from './services/ProjectPlanningService';
{ SecretsManagerService } from './services/SecretsManagerService';
{ ProjectStatusManager } from './services/ProjectStatusManager';

// Export actions
{ generateCodeAction } from './actions/generate-code';
{ createProjectAction } from './actions/create-project';

// Export providers
{ projectsProvider } from './providers/projects-provider';
{ currentProjectProvider } from './providers/current-project-provider';

/**
 * AutoCoder Plugin for ElizaOS
 *
 * Advanced code generation system using Claude Code in E2B sandboxes.
 * Supports creating plugins, agents, workflows, MCP servers, and full-stack apps.
 *
 * Features:
 * - Claude Code integration for intelligent code generation
 * - E2B sandboxed execution environments
 * - Multi-step project planning with forms
 * - Automated QA with linting, type checking, and testing
 * - GitHub repository creation and deployment
 * - API key and secrets management
 * - Real-time code generation with quality assurance
 */
const autocoderPlugin: Plugin = {
  name: '@elizaos/plugin-autocoder',
  description:
    'Advanced code generation plugin that enables autonomous plugin development. Searches registry, generates PRDs, and creates working plugins with validation.',

  services: [
    CodeGenerationService,
    ProjectPlanningService,
    SecretsManagerService,
    ProjectStatusManager,
  ],
  actions: [generateCodeAction, createProjectAction],
  providers: [projectsProvider, currentProjectProvider],

  // Dependencies - required for functionality
  dependencies: ['@elizaos/plugin-forms', '@elizaos/plugin-plugin-manager'],
  testDependencies: ['@elizaos/plugin-forms'],

  // E2E Test Suites - Real runtime integration tests
  tests: testSuites,
};

// Default default autocoderPlugin;

} // namespace elizaos
