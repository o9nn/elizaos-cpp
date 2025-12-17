#include "parsing.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

AbstractParseFunction getParser(const std::string& parserName) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        switch (parserName) {
            case 'identity':
            return new Identity();
            case 'thought_action':
            return new ThoughtActionParser();
            case 'action_only':
            return new ActionOnlyParser();
            case 'xml_thought_action':
            return new XMLThoughtActionParser();
            case 'edit_format':
            return new EditFormatParser();
            case 'function_calling':
            return new FunctionCallingParser();
            case 'single_bash_code_block':
            return new SingleBashCodeBlockParser();
            case 'multiple_bash_code_blocks':
            return new MultipleBashCodeBlocksParser();
            case 'last_line':
            return new LastLineParser();
            case 'identity':
            return new IdentityParser();
            default:
            throw std::runtime_error(`Unknown parser: ${parserName}`);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

[string, string] parseAction(const std::variant<ModelOutput, std::string>& modelResponse, const std::variant<std::string, AbstractParseFunction = 'thought_action'>& parseFunction, std::optional<std::vector<Command>> commands, boolean = false strict) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto parser: AbstractParseFunction;

    if (typeof parseFunction == 'string') {
        parser = getParser(parseFunction);
        } else {
            parser = parseFunction;
        }

        return parser.call(modelResponse, commands, strict);

}

AbstractParseFunction createParser(const std::string& type) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return getParser(type);

}

} // namespace elizaos
