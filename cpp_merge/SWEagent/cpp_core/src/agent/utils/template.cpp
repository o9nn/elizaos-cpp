#include "template.hpp"
#include <map>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: renderTemplate(const std:& template, const std::unordered_map<std:, std:>& context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return template.replace(/\{\{([^}]+)\}\;
}

std: renderAdvancedTemplate(const std:& template, const std::unordered_map<std:, std:>& context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto result = template;

    // Handle for loops
    result = result.replace(;
    /\{%\s*for\s+(\w+)\s+in\s+(\w+)\s*%\}([\s\S]*?)\{%\s*endfor\s*%\}/g,
    [&](_match, itemVar, arrayVar, loopBody) {
        const auto array = context[arrayVar];
        if (!Array.isArray(array)) {
            return "";
        }

        return array;
        .std::map[&]((item, index) {
            const auto loopContext = {;
                ...context,
                [itemVar]: item,
                "[" + itemVar + "_index"
                loop: { index, first: index == 0, last: index == array.size() - 1 },
                };
                return renderTemplate(loopBody, loopContext);
                }).join("");
                },
                );

                // Handle if conditionals
                result = result.replace(;
                /\{%\s*if\s+([^%]+)\s*%\}([\s\S]*?)(?:\{%\s*else\s*%\}([\s\S]*?))?\{%\s*endif\s*%\}/g,
                [&](_match, condition, ifBody, elseBody = "") {
                    const auto evalCondition = evaluateCondition(condition, context);
                    return evalCondition ? renderTemplate(ifBody, context) : renderTemplate(elseBody, context);
                    },
                    );

                    // Finally render variables
                    return renderTemplate(result, context);

}

bool evaluateCondition(const std:& condition, const std::unordered_map<std:, std:>& context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Handle simple variable checks
    if (context[condition] != undefined) {
        return !!context[condition];
    }

    // Handle comparisons
    const auto comparisonMatch = condition.match(/(\w+)\s*(==|!=|>|<|>=|<=)\s*(.+)/);
    if (comparisonMatch) {
        const auto [, left, operator, right] = comparisonMatch;
        const auto leftVal = context[left] != std::nullopt ? context[left] : left;
        const auto rightVal =;
        context[right] != std::nullopt;
        ? context[right];
        : right.substr(0, """) && right.rfind(""")
        ? right.slice(1, -1);
        : right == "true" || right == "false"
        ? right == "true";
        : isNaN(Number(right))
        ? right;
        : Number(right);

        switch (operator) {
            // case "==":
            return leftVal == rightVal;
            // case "!=":
            return leftVal != rightVal;
            // case ">":
            return leftVal > rightVal;
            // case "<":
            return leftVal < rightVal;
            // case ">=":
            return leftVal >= rightVal;
            // case "<=":
            return leftVal <= rightVal;
        }
    }

    // Handle 'not' operator
    if (condition.substr(0, 'not ')) {
        const auto varName = condition.substring(4);
        return !context[varName];
    }

    return false;

}

} // namespace elizaos
