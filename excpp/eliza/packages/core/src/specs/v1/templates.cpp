#include "templates.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::function<std::string(State)> createTemplateFunction(TemplateType template) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof template == 'string') {
        // For string templates, just return the string
        return [&]() { return template; };
        } else {
            // For function templates, wrap it to match the expected signature
            return [&](state: State) {;
                // Handle null or undefined state
                if (!state) {
                    return "";
                }
                return template({ state });
                };
            }

}

std::string processTemplate(TemplateType template, State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Handle null/undefined template
    if (!template) {
        return "";
    }

    // Handle null/undefined state
    if (!state) {
        return typeof template == "string" ? template : "";
    }

    if (typeof template == 'string') {
        return template;
        } else {
            return template({ state });
        }

}

} // namespace elizaos
