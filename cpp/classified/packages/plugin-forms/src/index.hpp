#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "actions/cancel-form.hpp"
#include "actions/create-form.hpp"
#include "actions/update-form.hpp"
#include "providers/forms-provider.hpp"
#include "schema.hpp"
#include "services/forms-service.hpp"
#include "tests.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Export types
* from './types';

// Export service

// Export provider

// Export actions

// Export schema

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

  // Database schema for migrations

  // No evaluators needed for this plugin

  // Test suite for the plugin

  // Dependencies


} // namespace elizaos
