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
 * Render a template std::string with the given context
 * Supports:
 * - Variable substitution: {{variable}}
 * - Conditional blocks: {% if condition %} ... {% endif %}
 * - For loops: {% for item in items %} ... {% endfor %}
 */
/**
 * Render a template std::string with the given context, replacing placeholders with values
 * @param template - The template std::string with {{variable}} placeholders
 * @param context - Object containing values to replace in the template
 * @returns The rendered template std::string
 */
std::string renderTemplate(const std::string& template, const std::unordered_map<std::string, std::string>& context);

/**
 * Handle conditional blocks
 */
std::string handleConditionals(const std::string& template, const std::unordered_map<std::string, std::string>& context);

/**
 * Handle for loops
 */
std::string handleForLoops(const std::string& template, const std::unordered_map<std::string, std::string>& context);

/**
 * Handle variable substitution
 */
std::string handleVariables(const std::string& template, const std::unordered_map<std::string, std::string>& context);

/**
 * Handle template filters
 */
std::string handleFilters(const std::string& template, const std::unordered_map<std::string, std::string>& context);

/**
 * Get nested value from object using dot notation
 */
std::string getNestedValue(const std::string& obj, const std::string& path);

/**
 * Evaluate a condition in the given context
 */
bool evaluateCondition(const std::string& condition, const std::unordered_map<std::string, std::string>& context);

/**
 * Apply a filter to a value
 */
std::string applyFilter(const std::string& value, const std::string& filterStr);

/**
 * Escape special characters in a std::string for use in templates
 */
std::string escapeTemplate(const std::string& str);

/**
 * Check if a std::string contains template syntax
 */
bool hasTemplateSyntax(const std::string& str);

} // namespace elizaos
