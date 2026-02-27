#include "parsing.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

AbstractParseFunction createParser(const std::variant<std:, std:>& config) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto type = typeof config == "string" ? config : config.type;

        switch (type) {
            // case "action":
            return std::make_unique<ActionParser>();
            // case "action_only":
            return std::make_unique<ActionOnlyParser>();
            // case "thought_action":
            return std::make_unique<ThoughtActionParser>();
            // case "xml_thought_action":
            return std::make_unique<XMLThoughtActionParser>();
            // case "function_calling":
            return std::make_unique<FunctionCallingParser>();
            // case "json":
            return std::make_unique<JsonParser>();
            // case "edit_format":
            return std::make_unique<EditFormat>();
            // case "identity":
            return std::make_unique<Identity>();
            // default:
            throw std::runtime_error("Unknown parser type: " + std::to_string(type) + "");
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
