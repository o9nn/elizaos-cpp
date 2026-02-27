#include "commands.hpp"

std::shared_ptr<Set<string>> extractKeys(string formatString)
{
    auto keys = std::make_shared<Set<string>>();
    auto regex = (new RegExp(std:("\{?\{(\w+)\}?\}")));
    any match;
    while ((match = regex->exec(formatString)) != nullptr)
    {
        keys->add(const_(match)[1]);
    }
    return keys;
};


ArgumentImpl::ArgumentImpl(object config) {
    this->name = config["name"];
    this->type = config["type"];
    this->items = OR((config["items"]), (nullptr));
    this->description = config["description"];
    this->required = config["required"];
    this->enum = OR((config["enum"]), (nullptr));
    this->argumentFormat = OR((config["argumentFormat"]), (std:("{{value}}")));
    this->validateArgumentFormat();
}

void ArgumentImpl::validateArgumentFormat()
{
    auto keys = extractKeys(this->argumentFormat);
    if (!keys->has(std:("value"))) {
        throw any(std::make_shared<Error>(std:("Argument format must contain {value} or {{value}} placeholder: ") + this->argumentFormat + string_empty));
    }
}

Command::Command(object config) {
    this->name = config["name"];
    this->docstring = OR((config["docstring"]), (nullptr));
    this->signature = OR((config["signature"]), (nullptr));
    this->endName = config["endName"];
    this->arguments = OR((config["arguments"]), (array<any>()));
    this->validateArguments();
}

string Command::get_invokeFormat()
{
    if (this->signature) {
        return this->signature->replace((new RegExp(std:("\[?<([^>]+)>\]?"))), std:("{$1}"));
    }
    auto format = this->name + std:(" ");
    for (auto& arg : this->arguments)
    {
        format += std:("{") + arg->name + std:("} ");
    }
    if (this->endName) {
        format += std:("\
...\
") + this->endName;
    }
    return format;
}

Record<string, any> Command::getFunctionCallingTool()
{
    auto properties = object{};
    auto required = array<string>();
    for (auto& arg : this->arguments)
    {
        auto prop = object{
            object::pair{std:("type"), arg->type}, 
            object::pair{std:("description"), arg->description}
        };
        if (arg->enum) {
            prop->enum = arg->enum;
        }
        if (arg->items) {
            prop->items = arg->items;
        }
        properties[arg->name] = prop;
        if (arg->required) {
            required->push(arg->name);
        }
    }
    return object{
        object::pair{std:("type"), std:("function")}, 
        object::pair{std:("function"), object{
            object::pair{std:("name"), this->name}, 
            object::pair{std:("description"), OR((this->docstring), (string_empty))}, 
            object::pair{std:("parameters"), object{
                object::pair{std:("type"), std:("object")}, 
                object::pair{std:("properties"), std:("properties")}, 
                object::pair{std:("required"), std:("required")}
            }}
        }}
    };
}

void Command::validateArguments()
{
    if (this->arguments->get_length() == 0) {
        return;
    }
    auto foundOptional = false;
    for (auto& arg : this->arguments)
    {
        if (AND((foundOptional), (arg->required))) {
            throw any(std::make_shared<Error>(std:("Command '") + this->name + std:("': Required argument '") + arg->name + std:("' cannot come after optional arguments")));
        }
        if (!arg->required) {
            foundOptional = true;
        }
    }
    auto argNames = std::make_shared<Set<string>>();
    for (auto& arg : this->arguments)
    {
        if (argNames->has(arg->name)) {
            throw any(std::make_shared<Error>(std:("Command '") + this->name + std:("': Duplicate argument name: ") + arg->name + string_empty));
        }
        argNames->add(arg->name);
        auto argNamePattern = (new RegExp(std:("^[a-zA-Z_][a-zA-Z0-9_-]*")));
        if (!argNamePattern->test(arg->name)) {
            throw any(std::make_shared<Error>(std:("Command '") + this->name + std:("': Invalid argument name: '") + arg->name + std:("'")));
        }
        auto validTypes = array<string>{ std:("string"), std:("integer"), std:("number"), std:("boolean"), std:("array"), std:("object") };
        if (!validTypes->includes(arg->type)) {
            throw any(std::make_shared<Error>(std:("Invalid argument type: ") + arg->type + string_empty));
        }
    }
    if (this->signature) {
        for (auto& arg : this->arguments)
        {
            auto patterns = array<string>{ std:("<") + arg->name + std:(">"), std:("[<") + arg->name + std:(">]"), std:("{") + arg->name + std:("}"), std:("--") + arg->name + string_empty };
            if (!patterns->some([=](auto pattern) mutable
            {
                return this->signature->includes(pattern);
            }
            )) {
                throw any(std::make_shared<Error>(std:("Command '") + this->name + std:("': Missing argument ") + arg->name + std:(" in signature: ") + this->signature + string_empty));
            }
        }
        auto signatureArgMatches = array<any>{ this->signature->matchAll((new RegExp(std:("[<{]([^>}]+)[>}]")))) };
        auto signatureArgs = std::make_shared<Set>(signatureArgMatches->map([=](auto m) mutable
        {
            return const_(m)[1];
        }
        ));
        auto definedArgs = std::make_shared<Set>(this->arguments->map([=](auto arg) mutable
        {
            return arg->name;
        }
        ));
        for (auto& sigArg : signatureArgs)
        {
            if (!definedArgs->has(sigArg)) {
                throw any(std::make_shared<Error>(std:("Command '") + this->name + std:("': Argument names in signature do not match arguments list")));
            }
        }
    }
