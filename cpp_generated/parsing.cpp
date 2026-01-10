#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/agent/tools/parsing.h"

string FunctionCallingParser::formatErrorMessage(object error)
{
    if (error["errorCode"] == std::string("missing")) {
        return std::string("The model did not use any tool calls");
    }
    return OR((error["message"]), (std::string("Unknown error")));
}

std::shared_ptr<AbstractParseFunction> getParser(string parserName)
{
    static switch_type __switch11968_12730 = {
        { any(std::string("identity")), 1 },
        { any(std::string("thought_action")), 2 },
        { any(std::string("action_only")), 3 },
        { any(std::string("xml_thought_action")), 4 },
        { any(std::string("edit_format")), 5 },
        { any(std::string("function_calling")), 6 },
        { any(std::string("single_bash_code_block")), 7 },
        { any(std::string("multiple_bash_code_blocks")), 8 },
        { any(std::string("last_line")), 9 },
        { any(std::string("identity")), 10 }
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
        throw any(std::make_shared<Error>(std::string("Unknown parser: ") + parserName + string_empty));
    }
};


std::shared_ptr<AbstractParseFunction> createParser(string type)
{
    return getParser(type);
};


