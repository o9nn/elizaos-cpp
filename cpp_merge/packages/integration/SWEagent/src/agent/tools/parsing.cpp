#include "parsing.hpp"

string FunctionCallingParser::formatErrorMessage(object error)
{
    if (error["errorCode"] == std:("missing")) {
        return std:("The model did not use any tool calls");
    }
    return OR((error["message"]), (std:("Unknown error")));
}

std::shared_ptr<AbstractParseFunction> getParser(string parserName)
{
    static switch_type __switch11968_12730 = {
        { any(std:("identity")), 1 },
        { any(std:("thought_action")), 2 },
        { any(std:("action_only")), 3 },
        { any(std:("xml_thought_action")), 4 },
        { any(std:("edit_format")), 5 },
        { any(std:("function_calling")), 6 },
        { any(std:("single_bash_code_block")), 7 },
        { any(std:("multiple_bash_code_blocks")), 8 },
        { any(std:("last_line")), 9 },
        { any(std:("identity")), 10 }
    };
    switch (__switch11968_12730[parserName])
    {
    case 1:
        return std::make_shared<Identity>();
    case 2:
        return std::make_shared<ThoughtActionParser>();
    case 3:
        return std::make_shared<ActionOnlyParser>();
    case 4:
        return std::make_shared<XMLThoughtActionParser>();
    case 5:
        return std::make_shared<EditFormatParser>();
    case 6:
        return std::make_shared<FunctionCallingParser>();
    case 7:
        return std::make_shared<SingleBashCodeBlockParser>();
    case 8:
        return std::make_shared<MultipleBashCodeBlocksParser>();
    case 9:
        return std::make_shared<LastLineParser>();
    case 10:
        return std::make_shared<IdentityParser>();
    default:
        throw any(std::make_shared<Error>(std:("Unknown parser: ") + parserName + string_empty));
    }
};


std::shared_ptr<AbstractParseFunction> createParser(string type)
{
    return getParser(type);
};


