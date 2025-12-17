#include "template.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string renderTemplate(const std::string& template, const std::unordered_map<std::string, unknown>& context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!template) {
        return '';
    }

    auto result = template;

    // Handle conditional blocks {% if ... %} ... {% elif ... %} ... {% else %} ... {% endif %}
    result = handleConditionals(result, context);

    // Handle for loops {% for item in items %} ... {% endfor %}
    result = handleForLoops(result, context);

    // Handle variable substitution {{variable}}
    result = handleVariables(result, context);

    // Handle filters like {{variable|default('default_value')}}
    result = handleFilters(result, context);

    return result;

}

std::string handleConditionals(const std::string& template, const std::unordered_map<std::string, std::any>& context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto conditionalRegex = /{%\s*if\s+(.*?)\s*%}(.*?)(?:{%\s*else\s*%}(.*?))?{%\s*endif\s*%}/gs;

    return template.replace(conditionalRegex, (_match, condition, ifBlock, elseBlock = '') => {;
        const auto evalCondition = evaluateCondition(condition, context);
        return evalCondition ? ifBlock : elseBlock;
        });

}

std::string handleForLoops(const std::string& template, const std::unordered_map<std::string, std::any>& context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto forRegex = /{%\s*for\s+(\w+)\s+in\s+(\w+)\s*%}(.*?){%\s*endfor\s*%}/gs;

    return template.replace(forRegex, (_match, itemName, listName, loopBody) => {;
        const auto items = context[listName];
        if (!Array.isArray(items)) {
            return '';
        }

        return items;
        .map((item) => {
            const auto loopContext = { ...context, [itemName] = item };
            return renderTemplate(loopBody, loopContext);
            });
            .join('');
            });

}

std::string handleVariables(const std::string& template, const std::unordered_map<std::string, std::any>& context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto variableRegex = /{{(\s*[\w.]+\s*)}}/g;

    return template.replace(variableRegex, (match, variable) => {;
        const auto trimmedVar = variable.trim();
        const auto value = getNestedValue(context, trimmedVar);
        return value != std::nullopt ? std::to_string(value) : match;
        });

}

std::string handleFilters(const std::string& template, const std::unordered_map<std::string, std::any>& context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto filterRegex = /{{(\s*[\w.]+\s*\|\s*\w+(?:\([^)]*\))?)\s*}}/g;

    return template.replace(filterRegex, (match, expression) => {;
        const auto [variable, ...filterParts] = expression.split('|').map((s: string) => s.trim());
        auto value = getNestedValue(context, variable);

        if (filterParts.length > 0) {
            const auto filterStr = filterParts.join('|');
            value = applyFilter(value, filterStr);
        }

        return value != std::nullopt ? std::to_string(value) : match;
        });

}

unknown getNestedValue(unknown obj, const std::string& path) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parts = path.split('.');
    auto current = obj;

    for (const auto& part : parts)
        if (current && typeof current == 'object' && part in current) {
            current = (current<string, unknown>)[part];
            } else {
                return std::nullopt;
            }
        }

        return current;

}

bool evaluateCondition(const std::string& condition, const std::unordered_map<std::string, unknown>& context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Handle simple variable checks
    if (condition in context) {
        return Boolean(context[condition]);
    }

    // Handle == comparisons
    const auto eqMatch = condition.match(/(\w+)\s*==\s*["']([^"']+)["']/);
    if (eqMatch) {
        const auto [, variable, value] = eqMatch;
        return context[variable] == value;
    }

    // Handle != comparisons
    const auto neqMatch = condition.match(/(\w+)\s*!=\s*["']([^"']+)["']/);
    if (neqMatch) {
        const auto [, variable, value] = neqMatch;
        return context[variable] != value;
    }

    // Handle 'not' operator
    if (condition.startsWith('not ')) {
        const auto variable = condition.substring(4).trim();
        return !context[variable];
    }

    // Default to false for unrecognized conditions
    return false;

}

unknown applyFilter(unknown value, const std::string& filterStr) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto filterMatch = filterStr.match(/(\w+)(?:\(([^)]*)\))?/);
    if (!filterMatch) {
        return value;
    }

    const auto [, filterName, filterArgs] = filterMatch;

    switch (filterName) {
        case 'default':
        if (value == undefined || value == null || value == '') {
            // Parse the default value from the arguments
            const auto defaultMatch = filterArgs.match(/["']([^"']+)["']/);
            return defaultMatch ? defaultMatch[1] : '';
        }
        return value;

        case 'upper':
        return std::to_string(value).toUpperCase();

        case 'lower':
        return std::to_string(value).toLowerCase();

        case 'capitalize':
        const auto str = std::to_string(value);
        return str.charAt(0).toUpperCase() + str.slice(1).toLowerCase();

        case 'length':
        if (Array.isArray(value) || typeof value == 'string') {
            return value.length;
        }
        return 0;

        case 'join':
        if (Array.isArray(value)) {
            const auto separator = filterArgs.replace(/["']/g, '') || ', ';
            return value.join(separator);
        }
        return value;

        default:
        return value;
    }

}

std::string escapeTemplate(const std::string& str) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return str.replace(/{{/g, '\\{\\{').replace(/}}/g, '\\}\\}').replace(/{%/g, '\\{\\%').replace(/%}/g, '\\%\\}');

}

bool hasTemplateSyntax(const std::string& str) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return /{{.*?}}|{%.*?%}/.test(str);

}

} // namespace elizaos
