#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/tools/bundle.h"

Bundle::Bundle(object config) {
    this->path = config["path"];
    this->hiddenTools = OR((config["hiddenTools"]), (array<any>()));
    this->validateTools();
}

void Bundle::validateTools()
{
    if (!fs->existsSync(this->path)) {
        throw any(std::make_shared<Error>(std::string("Bundle path does not exist: ") + this->path + string_empty));
    }
    auto configPath = path->join(this->path, std::string("config.yaml"));
    if (!fs->existsSync(configPath)) {
        throw any(std::make_shared<Error>(std::string("Bundle config not found: ") + configPath + string_empty));
    }
    auto configContent = fs->readFileSync(configPath, std::string("utf-8"));
    this->_config = as<std::shared_ptr<BundleConfig>>(yaml->load(configContent));
    if (OR((!this->_config->tools), (type_of(this->_config->tools) != std::string("object")))) {
        throw any(std::make_shared<Error>(std::string("Bundle config must contain tools object")));
    }
}

any Bundle::get_stateCommand()
{
    return this->get_config()->stateCommand;
}

std::shared_ptr<BundleConfig> Bundle::get_config()
{
    if (!this->_config) {
        this->validateTools();
    }
    return this->_config;
}

array<std::shared_ptr<Command>> Bundle::get_commands()
{
    auto commands = array<std::shared_ptr<Command>>();
    for (auto& [name, toolConfig] : Object->entries(this->get_config()->tools))
    {
        if (this->hiddenTools->includes(name)) {
            continue;
        }
        auto command = std::make_shared<Command>(object{
            object::pair{std::string("name"), std::string("name")}, 
            object::pair{std::string("docstring"), OR((toolConfig["docstring"]), (nullptr))}, 
            object::pair{std::string("signature"), OR((toolConfig["signature"]), (nullptr))}, 
            object::pair{std::string("endName"), OR((toolConfig["end_name"]), (nullptr))}, 
            object::pair{std::string("arguments"), OR((toolConfig["arguments"]), (array<any>()))}
        });
        commands->push(command);
    }
    return commands;
}

