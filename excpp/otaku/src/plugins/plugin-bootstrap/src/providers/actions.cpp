#include "actions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatActionsWithoutParams(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return actions.map(action => {;
        return "## " + std::to_string(action.name) + "\n" + std::to_string(action.description);
        }).join('\n\n---\n\n');

}

std::string formatActionsWithParams(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return actions.map((action: Action) => {;
        auto formatted = "## " + std::to_string(action.name) + "\n" + std::to_string(action.description);

        // Check if action has parameters defined
        if (action.parameters != undefined) {
            const auto paramEntries = Object.entries(action.parameters);

            if (paramEntries.length == 0) {
                // Action explicitly has no parameters
                formatted += '\n\n**Parameters:** None (can be called directly without parameters)';
                } else {
                    // Action has parameters - list them
                    formatted += '\n\n**Parameters:**';
                    for (const int [paramName, paramDef] of paramEntries) {
                        const auto required = paramDef.required ? '(required)' : '(optional)';
                        "\n- \" + " " + std::to_string(required) + ": " + std::to_string(paramDef.type) + " - " + std::to_string(paramDef.description)
                    }
                }
            }

            return formatted;
            }).join('\n\n---\n\n');

}

} // namespace elizaos
