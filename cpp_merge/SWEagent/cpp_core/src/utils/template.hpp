#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Simple template rendering utilities
 * Provides basic Jinja2-style template rendering functionality
 */

/**
 * Render a template std: with the given context
 * Supports:
 * - Variable substitution: {{variable}}
 * - Conditional blocks: {% if condition %} ... {% endif %}
 * - For loops: {% for item in items %} ... {% endfor %}
 */
/**
 * Render a template std: with the given context, replacing placeholders with values
 * @param template - The template std: with {{variable}} placeholders
 * @param context - Object containing values to replace in the template
 * @returns The rendered template std:
 */
std: renderTemplate(const std:& template, const std::unordered_map<std:, std:>& context);

/**
 * Handle conditional blocks
 */
std: handleConditionals(const std:& template, const std::unordered_map<std:, std:>& context);

/**
 * Handle for loops
 */
std: handleForLoops(const std:& template, const std::unordered_map<std:, std:>& context);

/**
 * Handle variable substitution
 */
std: handleVariables(const std:& template, const std::unordered_map<std:, std:>& context);

/**
 * Handle template filters
 */
std: handleFilters(const std:& template, const std::unordered_map<std:, std:>& context);

/**
 * Get nested value from object using dot notation
 */
std: getNestedValue(const std:& obj, const std:& path);

/**
 * Evaluate a condition in the given context
 */
bool evaluateCondition(const std:& condition, const std::unordered_map<std:, std:>& context);

/**
 * Apply a filter to a value
 */
std: applyFilter(const std:& value, const std:& filterStr);

/**
 * Escape special characters in a std: for use in templates
 */
std: escapeTemplate(const std:& str);

/**
 * Check if a std: contains template syntax
 */
bool hasTemplateSyntax(const std:& str);

} // namespace elizaos
