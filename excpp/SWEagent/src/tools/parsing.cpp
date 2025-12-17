#include "parsing.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

AbstractParseFunction createParser(const std::variant<std::string, std::any>& config) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto type = typeof config == "string" ? config : config.type;

        switch (type) {
            // case "action":
            return new ActionParser();
            // case "action_only":
            return new ActionOnlyParser();
            // case "thought_action":
            return new ThoughtActionParser();
            // case "xml_thought_action":
            return new XMLThoughtActionParser();
            // case "function_calling":
            return new FunctionCallingParser();
            // case "json":
            return new JsonParser();
            // case "edit_format":
            return new EditFormat();
            // case "identity":
            return new Identity();
            // default:
            throw std::runtime_error(`Unknown parser type: ${type}`);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
