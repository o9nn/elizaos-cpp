#include "types.hpp"
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
 * Project overview and structure
 */

;

/**
 * Main entry points for the SWE-agent application
 */
const ENTRY_POINTS: EntryPoint[] = [
  {
    path: 'sweagent/run/run_single.py',
    description: 'Main entry point for running single agent instances',
    type: 'single',
  },
  {
    path: 'sweagent/run/run_batch.py',
    description: 'Entry point for batch processing, used for benchmarking',
    type: 'batch',
  },
];

/**
 * Main agent class information
 */
const MAIN_AGENT_CLASS: ClassInfo = {
  name: 'Agent',
  path: 'sweagent/agent/agents.py',
  description: 'Main class that governs agent behavior',
};

/**
 * SWE environment class information
 */
const SWE_ENV_CLASS: ClassInfo = {
  name: 'SWEEnv',
  path: 'sweagent/environment/swe_env.py',
  description: 'Class that interfaces with the SWE-ReX project to interface with sandboxed docker containers',
};

/**
 * Execution environment configuration
 */
const EXECUTION_ENVIRONMENT: ExecutionEnvironment = {
  type: 'docker',
  description: 'AI agent proposes actions that are executed in sandboxed docker containers',
  interfaceClass: SWE_ENV_CLASS,
  interfaceProject: 'SWE-ReX',
};

/**
 * Tools configuration
 */
const TOOLS_INFO: ToolsInfo = {
  location: 'tools/',
  organization: 'bundles',
  deployment: 'Copied to sandboxed container and made available in $PATH variable',
};

/**
 * Inspector tools for trajectory analysis
 */
const INSPECTORS: Inspector[] = [
  {
    name: 'CLI Inspector',
    path: 'inspector_cli.py',
    type: 'cli',
    description: 'Command line interface for inspecting agent trajectories',
  },
  {
    name: 'Web Inspector',
    path: 'sweagent/inspector/server.py',
    type: 'web',
    description: 'Web interface for inspecting agent trajectories',
  },
];

/**
 * Complete project structure
 */
const PROJECT_STRUCTURE: ProjectStructure = {
  mainEntryPoints: ENTRY_POINTS,
  mainClass: MAIN_AGENT_CLASS,
  executionEnvironment: EXECUTION_ENVIRONMENT,
  tools: TOOLS_INFO,
  inspectors: INSPECTORS,
};

/**
 * Project overview text
 */
const PROJECT_OVERVIEW = `SWE-agent implements an AI software engineering agent that uses language models to fix github issues.`;

/**
 * Get project component by path
 */
 | null {
  // Check entry points
  const entryPoint = ENTRY_POINTS.find((ep) => ep.path === path);
  if (entryPoint) {
    return { component: 'entry-point', description: entryPoint.description };
  }

  // Check main classes
  if (path === MAIN_AGENT_CLASS.path) {
    return { component: 'main-agent', description: MAIN_AGENT_CLASS.description };
  }

  if (path === SWE_ENV_CLASS.path) {
    return { component: 'environment', description: SWE_ENV_CLASS.description };
  }

  // Check inspectors
  const inspector = INSPECTORS.find((i) => i.path === path);
  if (inspector) {
    return { component: 'inspector', description: inspector.description };
  }

  // Check if it's a tool
  if (path.startsWith('tools/')) {
    return { component: 'tool', description: 'Tool bundle for agent actions' };
  }

  return null;
}

/**
 * Get all Python modules in the project structure
 */


/**
 * TypeScript equivalent paths (for conversion tracking)
 */
const TYPESCRIPT_EQUIVALENTS: Record<string, string> = {
  'sweagent/run/run_single.py': 'src/run/run-single.ts',
  'sweagent/run/run_batch.py': 'src/run/run-batch.ts',
  'sweagent/agent/agents.py': 'src/agent/agents.ts',
  'sweagent/environment/swe_env.py': 'src/environment/swe-env.ts',
  'inspector_cli.py': 'src/inspector/cli.ts',
  'sweagent/inspector/server.py': 'src/inspector/server.ts',
};

} // namespace elizaos
