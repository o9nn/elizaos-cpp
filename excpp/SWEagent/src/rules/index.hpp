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
 * Main rules module
 * Exports all rule configurations, validators, and utilities
 */

// Type exports
* from './types';

// General coding rules
{
  PYTHON_CODING_RULES,
  TYPESCRIPT_CODING_RULES,
  GENERAL_CODING_GUIDELINES,
  TYPESCRIPT_CODING_GUIDELINES,
  validateAgainstRules,
  getApplicableRules,
} from './general';

// Project structure and overview
{
  ENTRY_POINTS,
  MAIN_AGENT_CLASS,
  SWE_ENV_CLASS,
  EXECUTION_ENVIRONMENT,
  TOOLS_INFO,
  INSPECTORS,
  PROJECT_STRUCTURE,
  PROJECT_OVERVIEW,
  getComponentByPath,
  getPythonModules,
  TYPESCRIPT_EQUIVALENTS,
} from './project-overview';

// Validators
{
  PythonValidator,
  TypeScriptValidator,
  getValidator,
  validateFile,
  validateFiles,
  formatValidationResults,
  type ValidationResult,
  type Violation,
} from './validators';

// Configuration exports
{ RULES_CONFIG, CURSOR_RULES, exportAllRulesToCursor, exportToCursorFormat } from './config';

/**
 * Default with all rules and utilities
 */
default {
  config: require('./config').RULES_CONFIG,
  validators: {
    python: new (require('./validators').PythonValidator)(),
    typescript: new (require('./validators').TypeScriptValidator)(),
  },
  utils: {
    getComponentByPath: require('./project-overview').getComponentByPath,
    getApplicableRules: require('./general').getApplicableRules,
    validateFile: require('./validators').validateFile,
  },
};

} // namespace elizaos
