#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "__tests__/e2e/index.hpp"
#include "actions/create-project.hpp"
#include "actions/generate-code.hpp"
#include "providers/current-project-provider.hpp"
#include "providers/projects-provider.hpp"
#include "services/CodeGenerationService.hpp"
#include "services/ProjectPlanningService.hpp"
#include "services/ProjectStatusManager.hpp"
#include "services/SecretsManagerService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Export types
* from './types/index';

// Export services

// Export actions

// Export providers

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

  // Dependencies - required for functionality

  // E2E Test Suites - Real runtime integration tests

// Default default autocoderPlugin;

} // namespace elizaos
