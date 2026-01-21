#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_AGENT_TOOLS_PARSING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_AGENT_TOOLS_PARSING_H
#include "core.h"
#include "../../exceptions.h"
#include "../types.h"

typedef AbstractParseFunction ParseFunction;

class AbstractParseFunction;
class ThoughtActionParser;
class ActionOnlyParser;
class XMLThoughtActionParser;
class EditFormatParser;
class FunctionCallingParser;
class SingleBashCodeBlockParser;
class MultipleBashCodeBlocksParser;
class Identity;
class LastLineParser;
class IdentityParser;
class JsonParser;

class AbstractParseFunction : public object, public std::enable_shared_from_this<AbstractParseFunction> {
public:
    using std::enable_shared_from_this<AbstractParseFunction>::shared_from_this;
    string type;

    string formatErrorTemplate;

    template <typename P0>
    std::tuple<string, string> call(P0 modelResponse, array<std::shared_ptr<Command>> _commands = undefined, boolean strict = undefined) = 0;
    template <typename P0>
    std::tuple<string, string> parse(P0 modelResponse, array<std::shared_ptr<Command>> commands = undefined, boolean strict = true);
};

class ThoughtActionParser : public AbstractParseFunction, public std::enable_shared_from_this<ThoughtActionParser> {
public:
    using std::enable_shared_from_this<ThoughtActionParser>::shared_from_this;
    string type = as<std::shared_ptr<const>>(std::string("thought_action"));

    template <typename P0>
    std::tuple<string, string> call(P0 modelResponse, array<std::shared_ptr<Command>> _commands = undefined, boolean strict = false);
};

class ActionOnlyParser : public AbstractParseFunction, public std::enable_shared_from_this<ActionOnlyParser> {
public:
    using std::enable_shared_from_this<ActionOnlyParser>::shared_from_this;
    string type = as<std::shared_ptr<const>>(std::string("action_only"));

    template <typename P0>
    std::tuple<string, string> call(P0 modelResponse, array<std::shared_ptr<Command>> _commands = undefined, boolean strict = false);
};

class XMLThoughtActionParser : public AbstractParseFunction, public std::enable_shared_from_this<XMLThoughtActionParser> {
public:
    using std::enable_shared_from_this<XMLThoughtActionParser>::shared_from_this;
    string type = as<std::shared_ptr<const>>(std::string("xml_thought_action"));

    template <typename P0>
    std::tuple<string, string> call(P0 modelResponse, array<std::shared_ptr<Command>> _commands = undefined, boolean strict = false);
};

class EditFormatParser : public ThoughtActionParser, public std::enable_shared_from_this<EditFormatParser> {
public:
    using std::enable_shared_from_this<EditFormatParser>::shared_from_this;
    template <typename P0>
    std::tuple<string, string> call(P0 modelResponse, array<std::shared_ptr<Command>> _commands = undefined, boolean strict = false);
};

class FunctionCallingParser : public AbstractParseFunction, public std::enable_shared_from_this<FunctionCallingParser> {
public:
    using std::enable_shared_from_this<FunctionCallingParser>::shared_from_this;
    string type = as<std::shared_ptr<const>>(std::string("function_calling"));

    virtual string formatErrorMessage(object error);
    template <typename P0>
    std::tuple<string, string> call(P0 modelResponse, array<std::shared_ptr<Command>> _commands = undefined, boolean strict = false);
};

class SingleBashCodeBlockParser : public AbstractParseFunction, public std::enable_shared_from_this<SingleBashCodeBlockParser> {
public:
    using std::enable_shared_from_this<SingleBashCodeBlockParser>::shared_from_this;
    string type = as<std::shared_ptr<const>>(std::string("single_bash_code_block"));

    template <typename P0>
    std::tuple<string, string> call(P0 modelResponse, array<std::shared_ptr<Command>> _commands = undefined, boolean strict = false);
};

class MultipleBashCodeBlocksParser : public AbstractParseFunction, public std::enable_shared_from_this<MultipleBashCodeBlocksParser> {
public:
    using std::enable_shared_from_this<MultipleBashCodeBlocksParser>::shared_from_this;
    string type = as<std::shared_ptr<const>>(std::string("multiple_bash_code_blocks"));

    template <typename P0>
    std::tuple<string, string> call(P0 modelResponse, array<std::shared_ptr<Command>> _commands = undefined, boolean strict = false);
};

class Identity : public AbstractParseFunction, public std::enable_shared_from_this<Identity> {
public:
    using std::enable_shared_from_this<Identity>::shared_from_this;
    string type = as<std::shared_ptr<const>>(std::string("identity"));

    template <typename P0>
    std::tuple<string, string> call(P0 modelResponse, array<std::shared_ptr<Command>> _commands = undefined, boolean _strict = false);
};

class LastLineParser : public AbstractParseFunction, public std::enable_shared_from_this<LastLineParser> {
public:
    using std::enable_shared_from_this<LastLineParser>::shared_from_this;
    string type = as<std::shared_ptr<const>>(std::string("last_line"));

    template <typename P0>
    std::tuple<string, string> call(P0 modelResponse, array<std::shared_ptr<Command>> _commands = undefined, boolean strict = false);
};

std::shared_ptr<AbstractParseFunction> getParser(string parserName);

template <typename P0, typename P1>
std::tuple<string, string> parseAction(P0 modelResponse, P1 parseFunction = std::string("thought_action"), array<std::shared_ptr<Command>> commands = undefined, boolean strict = false);

class IdentityParser : public AbstractParseFunction, public std::enable_shared_from_this<IdentityParser> {
public:
    using std::enable_shared_from_this<IdentityParser>::shared_from_this;
    string type = as<std::shared_ptr<const>>(std::string("identity"));

    template <typename P0>
    std::tuple<string, string> call(P0 modelResponse, array<std::shared_ptr<Command>> _commands = undefined, boolean _strict = undefined);
};

class JsonParser : public AbstractParseFunction, public std::enable_shared_from_this<JsonParser> {
public:
    using std::enable_shared_from_this<JsonParser>::shared_from_this;
    string type = as<std::shared_ptr<const>>(std::string("json"));

    template <typename P0>
    std::tuple<string, string> call(P0 modelResponse, array<std::shared_ptr<Command>> _commands = undefined, boolean strict = undefined);
};

std::shared_ptr<AbstractParseFunction> createParser(string type);

template <typename P0>
std::tuple<string, string> AbstractParseFunction::parse(P0 modelResponse, array<std::shared_ptr<Command>> commands, boolean strict)
{
    return this->call(modelResponse, commands, strict);
}

template <typename P0>
std::tuple<string, string> ThoughtActionParser::call(P0 modelResponse, array<std::shared_ptr<Command>> _commands, boolean strict)
{
    auto message = (type_of(modelResponse) == std::string("string")) ? modelResponse : OR((OR((modelResponse->message), (modelResponse->content))), (string_empty));
    auto codeBlockMatch = message->match((new RegExp(std::string("```(?:bash|sh)?\n(.*?)```"))));
    if (codeBlockMatch) {
        auto codeBlockIndex = message->indexOf((*const_(codeBlockMatch))[0]);
        auto thought = message->substring(0, codeBlockIndex);
        auto action = (*const_(codeBlockMatch))[1];
        return std::tuple<string, string>{ thought, action };
    }
    auto thoughtMatch = message->match((new RegExp(std::string("💭\s*THOUGHT[:\s]*([^\n]*(?:\n(?!🎬|ACTION)[^\n]*)*)"))));
    auto actionMatch = message->match((new RegExp(std::string("🎬\s*ACTION[:\s]*(.*?)(?=\n💭|\n🎬|$)/"))));
    auto thought = (thoughtMatch) ? any((*const_(thoughtMatch))[1]->trim()) : any(string_empty);
    auto action = (actionMatch) ? any((*const_(actionMatch))[1]->trim()) : any(string_empty);
    if (AND((!thought), (!action))) {
        auto lines = message->split(std::string("\
"));
        for (auto i = 0; i < lines->get_length(); i++)
        {
            auto line = const_(lines)[i];
            if (line->toLowerCase()->includes(std::string("thought:"))) {
                auto thoughtStart = line->indexOf(std::string(":")) + 1;
                auto thoughtLines = array<string>{ line->substring(thoughtStart)->trim() };
                for (auto j = i + 1; j < lines->get_length(); j++)
                {
                    if (const_(lines)[j]->toLowerCase()->includes(std::string("action:"))) {
                        break;
                    }
                    thoughtLines->push(const_(lines)[j]);
                }
                return std::tuple<string, string>{ thoughtLines->join(std::string("\
"))->trim(), string_empty };
            }
            if (line->toLowerCase()->includes(std::string("action:"))) {
                auto actionStart = line->indexOf(std::string(":")) + 1;
                auto actionLines = array<string>{ line->substring(actionStart)->trim() };
                for (auto j = i + 1; j < lines->get_length(); j++)
                {
                    if (const_(lines)[j]->toLowerCase()->includes(std::string("thought:"))) {
                        break;
                    }
                    actionLines->push(const_(lines)[j]);
                }
                return std::tuple<string, string>{ OR((thought), (string_empty)), actionLines->join(std::string("\
"))->trim() };
            }
        }
    }
    if (AND((strict), (!action))) {
        throw any(std::make_shared<FormatError>(std::string("Could not parse action from model response")));
    }
    return std::tuple<string, string>{ thought, action };
}

template <typename P0>
std::tuple<string, string> ActionOnlyParser::call(P0 modelResponse, array<std::shared_ptr<Command>> _commands, boolean strict)
{
    auto message = (type_of(modelResponse) == std::string("string")) ? modelResponse : OR((OR((modelResponse->message), (modelResponse->content))), (string_empty));
    auto action = message->trim();
    if (AND((strict), (!action))) {
        throw any(std::make_shared<FormatError>(std::string("Could not parse action from model response")));
    }
    if (AND((AND((strict), (_commands))), (_commands->get_length() > 0))) {
        auto actionParts = action->split((new RegExp(std::string("\s"))));
        shared commandName = const_(actionParts)[0];
        auto validCommand = _commands->find([=](auto cmd) mutable
        {
            return cmd->name == commandName;
        }
        );
        if (!validCommand) {
            throw any(std::make_shared<FormatError>(std::string("Invalid command: ") + commandName + string_empty));
        }
    }
    return std::tuple<string, string>{ string_empty, action };
}

template <typename P0>
std::tuple<string, string> XMLThoughtActionParser::call(P0 modelResponse, array<std::shared_ptr<Command>> _commands, boolean strict)
{
    auto message = (type_of(modelResponse) == std::string("string")) ? modelResponse : OR((OR((modelResponse->message), (modelResponse->content))), (string_empty));
    auto thoughtMatch = message->match((new RegExp(std::string("<thought>(.*?)<\/thought>/"))));
    auto actionMatch = message->match((new RegExp(std::string("<action>(.*?)<\/action>/"))));
    auto commandMatch = message->match((new RegExp(std::string("<command>(.*?)<\/command>/"))));
    auto thought = (thoughtMatch) ? any((*const_(thoughtMatch))[1]->trim()) : any(string_empty);
    auto action = (actionMatch) ? (*const_(actionMatch))[1]->trim() : (commandMatch) ? any((*const_(commandMatch))[1]->trim()) : any(string_empty);
    if (AND((commandMatch), (!thought))) {
        auto commandIndex = message->indexOf((*const_(commandMatch))[0]);
        auto thoughtText = message->substring(0, commandIndex)->trim();
        return std::tuple<string, string>{ thoughtText, (*const_(commandMatch))[1]->trim() };
    }
    if (AND((strict), (!action))) {
        throw any(std::make_shared<FormatError>(std::string("Could not parse action from model response")));
    }
    return std::tuple<string, string>{ thought, action };
}

template <typename P0>
std::tuple<string, string> EditFormatParser::call(P0 modelResponse, array<std::shared_ptr<Command>> _commands, boolean strict)
{
    auto [thought, action] = __super::call(modelResponse, _commands, strict);
    if (OR((action->includes(std::string("str_replace_editor"))), (action->includes(std::string("str_replace_based_edit_tool"))))) {
        auto editMatch = action->match((new RegExp(std::string("<<<(.*?)>>>"))));
        if (editMatch) {
            return std::tuple<string, string>{ thought, (*const_(editMatch))[1]->trim() };
        }
    }
    return std::tuple<string, string>{ thought, action };
}

template <typename P0>
std::tuple<string, string> FunctionCallingParser::call(P0 modelResponse, array<std::shared_ptr<Command>> _commands, boolean strict)
{
    auto toolCalls = (type_of(modelResponse) != std::string("string")) ? any(OR((modelResponse->toolCalls), ((as<any>(modelResponse))["tool_calls"]))) : any(nullptr);
    if (AND((toolCalls), (toolCalls["length"] > 0))) {
        if (AND((toolCalls["length"] > 1), (strict))) {
            throw any(std::make_shared<FormatError>(std::string("Multiple tool calls not supported")));
        }
        auto toolCall = const_(toolCalls)[0];
        shared functionName = toolCall["function"]["name"];
        auto args = toolCall["function"]["arguments"];
        if (AND((AND((_commands), (_commands->get_length() > 0))), (strict))) {
            auto validCommand = _commands->find([=](auto cmd) mutable
            {
                return cmd->name == functionName;
            }
            );
            if (!validCommand) {
                throw any(std::make_shared<FormatError>(std::string("Invalid command: ") + functionName + string_empty));
            }
        }
        auto action = functionName;
        if (args) {
            try
            {
                auto argsObj = (type_of(args) == std::string("string")) ? JSON->parse(args) : args;
                if (AND((AND((argsObj), (type_of(argsObj) == std::string("object")))), (Object->keys(argsObj)->get_length() > 0))) {
                    if (argsObj["command"]) {
                        action = String(argsObj["command"]);
                    } else {
                        auto values = Object->values(argsObj)->filter([=](auto v) mutable
                        {
                            return AND((AND((v != undefined), (v != nullptr))), (v != string_empty));
                        }
                        );
                        if (values->get_length() > 0) {
                            action = string_empty + functionName + std::string(" ") + values->map([=](auto v) mutable
                            {
                                return String(v);
                            }
                            )->join(std::string(" ")) + string_empty;
                        }
                    }
                }
            }
            catch (const any& e)
            {
                if (strict) {
                    throw any(std::make_shared<FormatError>(std::string("Invalid JSON in function arguments")));
                }
            }
        }
        auto thought = (type_of(modelResponse) != std::string("string")) ? any(OR((modelResponse->message), (string_empty))) : any(string_empty);
        return std::tuple<string, any>{ thought, action };
    }
    if (strict) {
        throw any(std::make_shared<FormatError>(std::string("No tool calls found in model response")));
    }
    auto message = (type_of(modelResponse) == std::string("string")) ? modelResponse : OR((modelResponse->message), (string_empty));
    return std::tuple<string, string>{ string_empty, message };
}

template <typename P0>
std::tuple<string, string> SingleBashCodeBlockParser::call(P0 modelResponse, array<std::shared_ptr<Command>> _commands, boolean strict)
{
    auto message = (type_of(modelResponse) == std::string("string")) ? modelResponse : OR((OR((modelResponse->message), (modelResponse->content))), (string_empty));
    auto codeBlockMatch = message->match((new RegExp(std::string("```(?:bash|sh)?\n(.*?)```"))));
    if (codeBlockMatch) {
        return std::tuple<string, string>{ string_empty, (*const_(codeBlockMatch))[1]->trim() };
    }
    if (strict) {
        throw any(std::make_shared<FormatError>(std::string("Could not find bash code block in model response")));
    }
    return std::tuple<string, string>{ string_empty, message->trim() };
}

template <typename P0>
std::tuple<string, string> MultipleBashCodeBlocksParser::call(P0 modelResponse, array<std::shared_ptr<Command>> _commands, boolean strict)
{
    auto message = (type_of(modelResponse) == std::string("string")) ? modelResponse : OR((OR((modelResponse->message), (modelResponse->content))), (string_empty));
    auto codeBlocks = array<string>();
    auto regex = (new RegExp(std::string("```(?:bash|sh)?\n(.*?)```/")));
    any match;
    while ((match = regex->exec(message)) != nullptr)
    {
        codeBlocks->push(const_(match)[1]["trim"]());
    }
    if (codeBlocks->get_length() > 0) {
        return std::tuple<string, string>{ string_empty, codeBlocks->join(std::string("\
\
")) };
    }
    if (strict) {
        throw any(std::make_shared<FormatError>(std::string("Could not find bash code blocks in model response")));
    }
    return std::tuple<string, string>{ string_empty, message->trim() };
}

template <typename P0>
std::tuple<string, string> Identity::call(P0 modelResponse, array<std::shared_ptr<Command>> _commands, boolean _strict)
{
    auto message = (type_of(modelResponse) == std::string("string")) ? modelResponse : OR((OR((modelResponse->message), (modelResponse->content))), (string_empty));
    auto codeBlockMatch = message->match((new RegExp(std::string("```(?:bash|sh)?\n(.*?)```"))));
    if (codeBlockMatch) {
        auto action = (*const_(codeBlockMatch))[1]->trim();
        return std::tuple<string, string>{ message, action };
    }
    return std::tuple<string, string>{ message, message };
}

template <typename P0>
std::tuple<string, string> LastLineParser::call(P0 modelResponse, array<std::shared_ptr<Command>> _commands, boolean strict)
{
    auto message = (type_of(modelResponse) == std::string("string")) ? modelResponse : OR((OR((modelResponse->message), (modelResponse->content))), (string_empty));
    auto lines = message->trim()->split(std::string("\
"));
    auto lastLine = const_(lines)[lines->get_length() - 1]->trim();
    if (AND((strict), (!lastLine))) {
        throw any(std::make_shared<FormatError>(std::string("Could not parse action from model response")));
    }
    auto thought = lines->slice(0, -1)->join(std::string("\
"))->trim();
    return std::tuple<string, string>{ thought, lastLine };
}

template <typename P0, typename P1>
std::tuple<string, string> parseAction(P0 modelResponse, P1 parseFunction, array<std::shared_ptr<Command>> commands, boolean strict)
{
    std::shared_ptr<AbstractParseFunction> parser;
    if (type_of(parseFunction) == std::string("string")) {
        parser = getParser(parseFunction);
    } else {
        parser = parseFunction;
    }
    return parser->call(modelResponse, commands, strict);
};


template <typename P0>
std::tuple<string, string> IdentityParser::call(P0 modelResponse, array<std::shared_ptr<Command>> _commands, boolean _strict)
{
    auto content = (type_of(modelResponse) == std::string("object")) ? OR((modelResponse->message), (string_empty)) : modelResponse;
    return std::tuple<string, string>{ string_empty, content->trim() };
}

template <typename P0>
std::tuple<string, string> JsonParser::call(P0 modelResponse, array<std::shared_ptr<Command>> _commands, boolean strict)
{
    auto message = (type_of(modelResponse) == std::string("string")) ? modelResponse : OR((OR((modelResponse->message), (modelResponse->content))), (string_empty));
    try
    {
        auto parsed = JSON->parse(message);
        if (OR((!parsed["thought"]), (!parsed["command"]))) {
            if (strict) {
                throw any(std::make_shared<FormatError>(std::string("JSON must contain both "thought" and "command" keys")));
            }
            return std::tuple<string, string>{ string_empty, message };
        }
        auto thought = parsed["thought"];
        auto action = string_empty;
        if (type_of(parsed["command"]) == std::string("string")) {
            action = parsed["command"];
        } else if (parsed["command"]["name"]) {
            action = parsed["command"]["name"];
            if (parsed["command"]["arguments"]) {
                if (type_of(parsed["command"]["arguments"]) == std::string("string")) {
                    action += std::string(" ") + parsed["command"]["arguments"];
                } else if (parsed["command"]["arguments"]["path"]) {
                    action += std::string(" ") + parsed["command"]["arguments"]["path"];
                }
            }
        }
        return std::tuple<any, string>{ thought, action };
    }
    catch (const any& e)
    {
        if (strict) {
            throw any(std::make_shared<FormatError>(std::string("Invalid JSON format")));
        }
        return std::tuple<string, string>{ string_empty, message };
    }
}

#endif
