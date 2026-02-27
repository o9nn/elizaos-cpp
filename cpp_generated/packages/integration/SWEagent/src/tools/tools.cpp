#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/tools/tools.h"

ToolHandler::ToolHandler(std::shared_ptr<ToolConfig> config) {
    this->config = utils::assign(object{
    }, defaultToolConfig, config);
    this->logger = getLogger(std::string("tools"));
    if (type_of(this->config->parseFunction) == std::string("string")) {
        this->parser = createParser(this->config->parseFunction);
    } else if (this->config->parseFunction) {
        this->parser = this->config->parseFunction;
    } else {
        this->parser = std::make_shared<FunctionCallingParser>();
    }
    if (!this->config->commands) {
        this->config->commands = this->getCommandsFromBundles();
    }
    if (!this->config->commandDocs) {
        this->config->commandDocs = generateCommandDocs(this->config->commands, array<any>(), object{});
    }
    if (!this->config->formatErrorTemplate) {
        this->config->formatErrorTemplate = this->parser->get_formatErrorTemplate();
    }
    for (auto& cmd : this->config->commands)
    {
        if (cmd->endName) {
            this->multilineCommands->set(cmd->name, cmd->endName);
        }
    }
}

array<std::shared_ptr<Command>> ToolHandler::getCommandsFromBundles()
{
    auto commands = array<std::shared_ptr<Command>>();
    if (this->config->bundles) {
        for (auto& bundle : this->config->bundles)
        {
            commands->push(const_(bundle->get_commands())[0]);
        }
    }
    if (this->config->enableBashTool) {
        commands->push(std::make_shared<Command>(object{
            object::pair{std::string("name"), std::string("bash")}, 
            object::pair{std::string("docstring"), std::string("Execute bash commands")}, 
            object::pair{std::string("arguments"), array<object>{ object{
                object::pair{std::string("name"), std::string("command")}, 
                object::pair{std::string("type"), std::string("string")}, 
                object::pair{std::string("description"), std::string("The bash command to execute")}, 
                object::pair{std::string("required"), true}, 
                object::pair{std::string("argumentFormat"), std::string("{{value}}")}
            } }}
        }));
    }
    return commands;
}

std::shared_ptr<ToolHandler> ToolHandler::fromConfig(std::shared_ptr<ToolConfig> config)
{
    return std::make_shared<ToolHandler>(config);
}

std::shared_ptr<Promise<void>> ToolHandler::install(std::shared_ptr<SWEEnv> env)
{
    this->logger->info(std::string("Installing tools..."));
    if (this->config->bundles) {
        for (auto& bundle : this->config->bundles)
        {
            std::async([=]() { this->uploadBundle(env, bundle); });
        }
    }
    if (this->config->envVariables) {
        std::async([=]() { env->setEnvVariables(this->config->envVariables); });
    }
    if (this->config->resetCommands) {
        for (auto& cmd : this->config->resetCommands)
        {
            auto command = (Array->isArray(cmd)) ? cmd->join(std::string(" ")) : cmd;
            std::async([=]() { env->communicate(command, this->config->installTimeout); });
        }
    }
    this->logger->info(std::string("Tools installed successfully"));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> ToolHandler::uploadBundle(std::shared_ptr<SWEEnv> _env, std::shared_ptr<Bundle> bundle)
{
    this->logger->info(std::string("Uploading bundle from ") + bundle->path + string_empty);
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> ToolHandler::reset(std::shared_ptr<SWEEnv> env)
{
    if (this->config->resetCommands) {
        for (auto& cmd : this->config->resetCommands)
        {
            auto command = (Array->isArray(cmd)) ? cmd->join(std::string(" ")) : cmd;
            std::async([=]() { env->communicate(command, this->config->executionTimeout); });
        }
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<Record<string, string>>> ToolHandler::getState(std::shared_ptr<SWEEnv> env)
{
    auto state = object{};
    if (this->config->stateCommands) {
        for (auto& cmd : this->config->stateCommands)
        {
            try
            {
                auto result = std::async([=]() { env->communicate(cmd, 5); });
                auto key = const_(cmd->split(std::string(" ")))[0];
                state[key] = result;
            }
            catch (const any& error)
            {
                this->logger->warning(std::string("Failed to execute state command: ") + cmd + string_empty);
            }
        }
    }
    if (this->config->bundles) {
        for (auto& bundle : this->config->bundles)
        {
            if (bundle->get_stateCommand()) {
                try
                {
                    auto result = std::async([=]() { env->communicate(bundle->get_stateCommand(), 5); });
                    state[std::string("bundle_state")] = result;
                }
                catch (const any& error)
                {
                    this->logger->warning(std::string("Failed to execute bundle state command"));
                }
            }
        }
    }
    return state;
}

boolean ToolHandler::shouldBlockAction(string action)
{
    if (!this->config->filter) {
        return false;
    }
    auto actionLower = action->toLowerCase();
    for (auto& blocked : this->config->filter->blocklist)
    {
        if (actionLower->startsWith(blocked->toLowerCase())) {
            return true;
        }
    }
    auto firstWord = const_(action->split((new RegExp(std::string("\s")))))[0];
    if (this->config->filter->blocklistStandalone->includes(firstWord)) {
        return true;
    }
    if (this->config->filter->blockUnlessRegex) {
        for (auto& [cmd, pattern] : Object->entries(this->config->filter->blockUnlessRegex))
        {
            if (actionLower->startsWith(cmd)) {
                auto regex = std::make_shared<RegExp>(pattern);
                if (!regex->test(action)) {
                    return true;
                }
            }
        }
    }
    return false;
}

boolean ToolHandler::checkForSubmissionCmd(string observation)
{
    auto submitCommand = OR((this->config->submitCommand), (std::string("submit")));
    return OR((observation->includes(std::string("<") + submitCommand + std::string(">"))), (observation->includes(std::string("</") + submitCommand + std::string(">"))));
}

string ToolHandler::guardMultilineInput(string action)
{
    for (auto& [cmdName, endName] : this->multilineCommands)
    {
        if (action->startsWith(cmdName)) {
            if (!action->includes(endName)) {
                this->logger->warning(std::string("Multiline command ") + cmdName + std::string(" missing end marker ") + endName + string_empty);
            }
        }
    }
    return action;
}

std::shared_ptr<ToolFilterConfig> defaultToolFilterConfig = object{
    object::pair{std::string("blocklistErrorTemplate"), std::string("Operation '{{action}}' is not supported by this environment.")}, 
    object::pair{std::string("blocklist"), array<string>{ std::string("vim"), std::string("vi"), std::string("emacs"), std::string("nano"), std::string("nohup"), std::string("gdb"), std::string("less"), std::string("tail -f"), std::string("python -m venv"), std::string("make") }}, 
    object::pair{std::string("blocklistStandalone"), array<string>{ std::string("python"), std::string("python3"), std::string("ipython"), std::string("bash"), std::string("sh"), std::string("/bin/bash"), std::string("/bin/sh"), std::string("nohup"), std::string("vi"), std::string("vim"), std::string("emacs"), std::string("nano"), std::string("su") }}, 
    object::pair{std::string("blockUnlessRegex"), object{
        object::pair{std::string("git"), std::string("^git\s+(status|diff|log|show)")}
    }}
};
std::shared_ptr<ToolConfig> defaultToolConfig = object{
    object::pair{std::string("filter"), defaultToolFilterConfig}, 
    object::pair{std::string("bundles"), array<any>()}, 
    object::pair{std::string("propagateEnvVariables"), array<any>()}, 
    object::pair{std::string("envVariables"), object{
        object::pair{std::string("PAGER"), std::string("cat")}, 
        object::pair{std::string("MANPAGER"), std::string("cat")}, 
        object::pair{std::string("LESS"), std::string("-R")}, 
        object::pair{std::string("PIP_PROGRESS_BAR"), std::string("off")}, 
        object::pair{std::string("TQDM_DISABLE"), std::string("1")}, 
        object::pair{std::string("GIT_PAGER"), std::string("cat")}
    }}, 
    object::pair{std::string("registryVariables"), object{}}, 
    object::pair{std::string("submitCommand"), std::string("submit")}, 
    object::pair{std::string("parseFunction"), std::make_shared<FunctionCallingParser>()}, 
    object::pair{std::string("enableBashTool"), true}, 
    object::pair{std::string("formatErrorTemplate"), string_empty}, 
    object::pair{std::string("commandDocs"), string_empty}, 
    object::pair{std::string("multiLineCommandEndings"), object{}}, 
    object::pair{std::string("submitCommandEndName"), nullptr}, 
    object::pair{std::string("resetCommands"), array<any>()}, 
    object::pair{std::string("executionTimeout"), 30}, 
    object::pair{std::string("installTimeout"), 300}, 
    object::pair{std::string("totalExecutionTimeout"), 1800}, 
    object::pair{std::string("maxConsecutiveExecutionTimeouts"), 3}
};

void Main(void)
{
}

MAIN
