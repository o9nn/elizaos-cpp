#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/tools/tools.h"

ToolHandler::ToolHandler(std::shared_ptr<ToolConfig> config) {
    this->config = utils::assign(object{
    }, defaultToolConfig, config);
    this->logger = getLogger(std:("tools"));
    if (type_of(this->config->parseFunction) == std:("string")) {
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
            object::pair{std:("name"), std:("bash")}, 
            object::pair{std:("docstring"), std:("Execute bash commands")}, 
            object::pair{std:("arguments"), array<object>{ object{
                object::pair{std:("name"), std:("command")}, 
                object::pair{std:("type"), std:("string")}, 
                object::pair{std:("description"), std:("The bash command to execute")}, 
                object::pair{std:("required"), true}, 
                object::pair{std:("argumentFormat"), std:("{{value}}")}
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
    this->logger->info(std:("Installing tools..."));
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
            auto command = (Array->isArray(cmd)) ? cmd->join(std:(" ")) : cmd;
            std::async([=]() { env->communicate(command, this->config->installTimeout); });
        }
    }
    this->logger->info(std:("Tools installed successfully"));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> ToolHandler::uploadBundle(std::shared_ptr<SWEEnv> _env, std::shared_ptr<Bundle> bundle)
{
    this->logger->info(std:("Uploading bundle from ") + bundle->path + string_empty);
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> ToolHandler::reset(std::shared_ptr<SWEEnv> env)
{
    if (this->config->resetCommands) {
        for (auto& cmd : this->config->resetCommands)
        {
            auto command = (Array->isArray(cmd)) ? cmd->join(std:(" ")) : cmd;
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
                auto key = const_(cmd->split(std:(" ")))[0];
                state[key] = result;
            }
            catch (const any& error)
            {
                this->logger->warning(std:("Failed to execute state command: ") + cmd + string_empty);
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
                    state[std:("bundle_state")] = result;
                }
                catch (const any& error)
                {
                    this->logger->warning(std:("Failed to execute bundle state command"));
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
    auto firstWord = const_(action->split((new RegExp(std:("\s")))))[0];
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
    auto submitCommand = OR((this->config->submitCommand), (std:("submit")));
    return OR((observation->includes(std:("<") + submitCommand + std:(">"))), (observation->includes(std:("</") + submitCommand + std:(">"))));
}

string ToolHandler::guardMultilineInput(string action)
{
    for (auto& [cmdName, endName] : this->multilineCommands)
    {
        if (action->startsWith(cmdName)) {
            if (!action->includes(endName)) {
                this->logger->warning(std:("Multiline command ") + cmdName + std:(" missing end marker ") + endName + string_empty);
            }
        }
    }
    return action;
}

std::shared_ptr<ToolFilterConfig> defaultToolFilterConfig = object{
    object::pair{std:("blocklistErrorTemplate"), std:("Operation '{{action}}' is not supported by this environment.")}, 
    object::pair{std:("blocklist"), array<string>{ std:("vim"), std:("vi"), std:("emacs"), std:("nano"), std:("nohup"), std:("gdb"), std:("less"), std:("tail -f"), std:("python -m venv"), std:("make") }}, 
    object::pair{std:("blocklistStandalone"), array<string>{ std:("python"), std:("python3"), std:("ipython"), std:("bash"), std:("sh"), std:("/bin/bash"), std:("/bin/sh"), std:("nohup"), std:("vi"), std:("vim"), std:("emacs"), std:("nano"), std:("su") }}, 
    object::pair{std:("blockUnlessRegex"), object{
        object::pair{std:("git"), std:("^git\s+(status|diff|log|show)")}
    }}
};
std::shared_ptr<ToolConfig> defaultToolConfig = object{
    object::pair{std:("filter"), defaultToolFilterConfig}, 
    object::pair{std:("bundles"), array<any>()}, 
    object::pair{std:("propagateEnvVariables"), array<any>()}, 
    object::pair{std:("envVariables"), object{
        object::pair{std:("PAGER"), std:("cat")}, 
        object::pair{std:("MANPAGER"), std:("cat")}, 
        object::pair{std:("LESS"), std:("-R")}, 
        object::pair{std:("PIP_PROGRESS_BAR"), std:("off")}, 
        object::pair{std:("TQDM_DISABLE"), std:("1")}, 
        object::pair{std:("GIT_PAGER"), std:("cat")}
    }}, 
    object::pair{std:("registryVariables"), object{}}, 
    object::pair{std:("submitCommand"), std:("submit")}, 
    object::pair{std:("parseFunction"), std::make_shared<FunctionCallingParser>()}, 
    object::pair{std:("enableBashTool"), true}, 
    object::pair{std:("formatErrorTemplate"), string_empty}, 
    object::pair{std:("commandDocs"), string_empty}, 
    object::pair{std:("multiLineCommandEndings"), object{}}, 
    object::pair{std:("submitCommandEndName"), nullptr}, 
    object::pair{std:("resetCommands"), array<any>()}, 
    object::pair{std:("executionTimeout"), 30}, 
    object::pair{std:("installTimeout"), 300}, 
    object::pair{std:("totalExecutionTimeout"), 1800}, 
    object::pair{std:("maxConsecutiveExecutionTimeouts"), 3}
};

void Main(void)
{
}

MAIN
