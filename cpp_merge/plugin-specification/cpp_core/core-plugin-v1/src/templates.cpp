#include "templates.hpp"
#include <string>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::function<std::string(State)> createTemplateFunction(TemplateType template) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof template == 'string') {
        // For std::string templates, just return the std::string return [&]() { return template; };
        } else {
            // For std::function templates, wrap it to match the expected signature
            return [&](state: State) {;
                // Handle null or std::nullopt state
                if (!state) {
                    return "";
                }
                return template({ state });
                };
            }

}

std::string processTemplate(TemplateType template, State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Handle null/std::nullopt template
    if (!template) {
        return "";
    }

    // Handle null/std::nullopt state
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
