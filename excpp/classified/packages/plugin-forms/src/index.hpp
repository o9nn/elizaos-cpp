#include "actions/cancel-form.hpp"
#include "actions/create-form.hpp"
#include "actions/update-form.hpp"
#include "providers/forms-provider.hpp"
#include "schema.hpp"
#include "services/forms-service.hpp"
#include "tests.hpp"
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

// Export types
* from './types';

// Export service
{ FormsService };

// Export provider
{ formsProvider };

// Export actions
{ createFormAction, updateFormAction, cancelFormAction };

// Export schema
{ formsSchema };

/**
 * Forms Plugin for ElizaOS
 *
 * This plugin provides structured form collection capabilities for agents,
 * allowing them to gather information from users in a conversational manner.
 *
 * Features:
 * - Multi-step forms with field validation
 * - LLM-based value extraction from natural language
 * - Secret field handling for sensitive data
 * - Form templates for common use cases
 * - Step and form completion callbacks
 * - Provider for showing active form state
 * - Database persistence for form state
 *
 * Usage:
 * 1. Add the plugin to your agent's plugins array
 * 2. The agent will automatically recognize form-related requests
 * 3. Forms are filled through natural conversation
 * 4. Other plugins can use the FormsService to create custom forms
 */
const formsPlugin: Plugin = {
  name: '@elizaos/plugin-forms',
  description: 'Structured form collection capabilities for conversational data gathering',

  services: [FormsService],
  providers: [formsProvider],
  actions: [createFormAction, updateFormAction, cancelFormAction],

  // Database schema for migrations
  schema: formsSchema,

  // No evaluators needed for this plugin
  evaluators: [],

  // Test suite for the plugin
  tests: [FormsPluginTestSuite],

  // Dependencies
  dependencies: ['@elizaos/plugin-sql'],
  testDependencies: ['@elizaos/plugin-sql'],
};

default formsPlugin;

} // namespace elizaos
