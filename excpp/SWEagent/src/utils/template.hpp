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
 * Simple template rendering utilities
 * Provides basic Jinja2-style template rendering functionality
 */

/**
 * Render a template string with the given context
 * Supports:
 * - Variable substitution: {{variable}}
 * - Conditional blocks: {% if condition %} ... {% endif %}
 * - For loops: {% for item in items %} ... {% endfor %}
 */
/**
 * Render a template string with the given context, replacing placeholders with values
 * @param template - The template string with {{variable}} placeholders
 * @param context - Object containing values to replace in the template
 * @returns The rendered template string
 */
std::string renderTemplate(const std::string& template, Record<string context, auto unknown>);

/**
 * Handle conditional blocks
 */
std::string handleConditionals(const std::string& template, Record<string context, auto any>);

/**
 * Handle for loops
 */
std::string handleForLoops(const std::string& template, Record<string context, auto any>);

/**
 * Handle variable substitution
 */
std::string handleVariables(const std::string& template, Record<string context, auto any>);

/**
 * Handle template filters
 */
std::string handleFilters(const std::string& template, Record<string context, auto any>);

/**
 * Get nested value from object using dot notation
 */
unknown getNestedValue(unknown obj, const std::string& path);

/**
 * Evaluate a condition in the given context
 */
bool evaluateCondition(const std::string& condition, Record<string context, auto unknown>);

/**
 * Apply a filter to a value
 */
unknown applyFilter(unknown value, const std::string& filterStr);

/**
 * Escape special characters in a string for use in templates
 */
std::string escapeTemplate(const std::string& str);

/**
 * Check if a string contains template syntax
 */
bool hasTemplateSyntax(const std::string& str);

} // namespace elizaos
