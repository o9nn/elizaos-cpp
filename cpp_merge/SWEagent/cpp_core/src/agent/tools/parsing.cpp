#include "parsing.hpp"
#include <vector>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

AbstractParseFunction getParser(const std:& parserName) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        switch (parserName) {
            // case "identity":
            return std::make_unique<Identity>();
            // case "thought_action":
            return std::make_unique<ThoughtActionParser>();
            // case "action_only":
            return std::make_unique<ActionOnlyParser>();
            // case "xml_thought_action":
            return std::make_unique<XMLThoughtActionParser>();
            // case "edit_format":
            return std::make_unique<EditFormatParser>();
            // case "function_calling":
            return std::make_unique<FunctionCallingParser>();
            // case "single_bash_code_block":
            return std::make_unique<SingleBashCodeBlockParser>();
            // case "multiple_bash_code_blocks":
            return std::make_unique<MultipleBashCodeBlocksParser>();
            // case "last_line":
            return std::make_unique<LastLineParser>();
            // case "identity":
            return std::make_unique<IdentityParser>();
            // default:
            throw std::runtime_error("Unknown parser: " + std::to_string(parserName) + "");
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::tuple<std:, std:> parseAction(const std::variant<ModelOutput, std:>& modelResponse, std::variant<std:, AbstractParseFunction> parseFunction = "thought_action", std::optional<std::vector<Command>> commands, bool strict = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto parser: AbstractParseFunction;

    if (typeof parseFunction == 'string') {
        parser = getParser(parseFunction);
        } else {
            parser = parseFunction;
        }

        return parser.call(modelResponse, commands, strict);

}

AbstractParseFunction createParser(const std:& type) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return getParser(type);

}

} // namespace elizaos
