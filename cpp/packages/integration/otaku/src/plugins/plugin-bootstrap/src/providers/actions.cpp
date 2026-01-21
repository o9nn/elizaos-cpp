#include "actions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatActionsWithoutParams(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return actions.map(action => {;
        return "## " + action.name + "\n" + action.description;
        }).join("\n\n---\n\n");

}

std::string formatActionsWithParams(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return actions.map((action: Action) => {;
        auto formatted = "## " + action.name + "\n" + action.description;

        // Check if action has parameters defined
        if (action.parameters != undefined) {
            const auto paramEntries = Object.entries(action.parameters);

            if (paramEntries.length == 0) {
                // Action explicitly has no parameters
                formatted += "\n\n**Parameters:** None (can be called directly without parameters)";
                } else {
                    // Action has parameters - list them
                    formatted += "\n\n**Parameters:**";
                    for (const int [paramName, paramDef] of paramEntries) {
                        const auto required = paramDef.required ? "(required)" : "(optional)";
                        "formatted += " + "\n- \" + "${paramName}\" + " " + required + ": " + paramDef.type + " - " + paramDef.description
                    }
                }
            }

            return formatted;
            }).join("\n\n---\n\n");

}

} // namespace elizaos
