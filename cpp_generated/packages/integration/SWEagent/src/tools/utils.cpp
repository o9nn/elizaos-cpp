#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/tools/utils.h"

string guardMultilineInput(string action, std::function<any(string)> matchFct)
{
    auto match = matchFct(action);
    if (!match) {
        return action;
    }
    auto lines = action->split(std::string("\
"));
    auto processedLines = lines->map([=](auto line) mutable
    {
        return line->trimEnd();
    }
    );
    return processedLines->join(std::string("\
"));
};


boolean shouldQuote(any value, std::shared_ptr<Command> command)
{
    if (type_of(value) != std::string("string")) {
        return false;
    }
    if ((new RegExp(std::string("[\s"'`$")))->test(value)) {
        return true;
    }
    for (auto& arg : command->arguments)
    {
        if (AND((arg->type == std::string("string")), (arg->required))) {
            return true;
        }
    }
    return false;
};


string getSignature(std::shared_ptr<Command> cmd)
{
    if (cmd->signature) {
        return cmd->signature;
    }
    auto sig = cmd->name;
    for (auto& arg : cmd->arguments)
    {
        if (arg->required) {
            sig += std::string(" <") + arg->name + std::string(">");
        } else {
            sig += std::string(" [") + arg->name + std::string("]");
        }
    }
    if (cmd->endName) {
        sig += std::string("\
...\
") + cmd->endName;
    }
    return sig;
};


string generateCommandDocs(array<std::shared_ptr<Command>> commands, array<string> subroutineTypes, Record<string, any> _kwargs)
{
    auto docs = array<string>();
    auto bashCommands = array<std::shared_ptr<Command>>();
    auto subroutineCommands = array<std::shared_ptr<Command>>();
    auto utilityCommands = array<std::shared_ptr<Command>>();
    for (auto& cmd : commands)
    {
        if (OR((cmd->name == std::string("bash")), (cmd->name == std::string("shell")))) {
            bashCommands->push(cmd);
        } else if (subroutineTypes->includes(cmd->name)) {
            subroutineCommands->push(cmd);
        } else {
            utilityCommands->push(cmd);
        }
    }
    if (bashCommands->get_length() > 0) {
        docs->push(std::string("# Bash Commands"));
        docs->push(std::string("Use bash commands to interact with the system."));
        for (auto& cmd : bashCommands)
        {
            docs->push(formatCommand(cmd));
        }
        docs->push(string_empty);
    }
    if (subroutineCommands->get_length() > 0) {
        docs->push(std::string("# Subroutine Commands"));
        for (auto& cmd : subroutineCommands)
        {
            docs->push(formatCommand(cmd));
        }
        docs->push(string_empty);
    }
    if (utilityCommands->get_length() > 0) {
        docs->push(std::string("# Utility Commands"));
        for (auto& cmd : utilityCommands)
        {
            docs->push(formatCommand(cmd));
        }
        docs->push(string_empty);
    }
    return docs->join(std::string("\
"));
};


string formatCommand(std::shared_ptr<Command> cmd)
{
    auto lines = array<string>();
    lines->push(std::string("## ") + getSignature(cmd) + string_empty);
    if (cmd->docstring) {
        lines->push(cmd->docstring);
    }
    if (cmd->arguments->get_length() > 0) {
        lines->push(std::string("Arguments:"));
        for (auto& arg : cmd->arguments)
        {
            auto required = (arg->required) ? std::string(" (required)") : std::string(" (optional)");
            lines->push(std::string("  - ") + arg->name + std::string(": ") + arg->description + string_empty + required + string_empty);
            if (arg->enum) {
                lines->push(std::string("    Allowed values: ") + arg->enum->join(std::string(", ")) + string_empty);
            }
        }
    }
    lines->push(string_empty);
    return lines->join(std::string("\
"));
};


