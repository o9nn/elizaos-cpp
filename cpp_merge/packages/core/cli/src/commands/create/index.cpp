#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/create/index.h"

any create = ((std::make_shared<Command>(std:("create"))))->description(std:("Create a new ElizaOS project, plugin, agent, or TEE project"))->argument(std:("[name]"), std:("name of the project/plugin/agent to create"))->option(std:("--dir <dir>"), std:("directory to create the project in"), std:("."))->option(std:("--yes, -y"), std:("skip prompts and use defaults"))->option(std:("--type <type>"), std:("type of project to create (project, plugin, agent, tee)"), std:("project"))->action([=](auto name = undefined, auto opts = undefined) mutable
{
    try
    {
        if (OR((OR((OR((process->env->ELIZA_NONINTERACTIVE == std:("1")), (process->env->ELIZA_NONINTERACTIVE == std:("true")))), (process->argv->includes(std:("-y"))))), (process->argv->includes(std:("--yes"))))) {
            if (opts) {
                opts["yes"] = true;
            } else {
                opts = object{
                    object::pair{std:("yes"), true}
                };
            }
        }
        auto options = validateCreateOptions(OR((opts), (object{})));
        auto isNonInteractive = options->yes;
        if (!isNonInteractive) {
            std::async([=]() { displayBanner(); });
            clack->intro(colors->inverse(std:(" Creating ElizaOS Project ")));
        }
        shared projectType = options->type;
        auto projectName = name;
        if (!projectName) {
            if (!isNonInteractive) {
                auto selectedType = std::async([=]() { clack->select(object{
                    object::pair{std:("message"), std:("What would you like to create?")}, 
                    object::pair{std:("options"), array<object>{ object{
                        object::pair{std:("label"), std:("Project - Full ElizaOS application")}, 
                        object::pair{std:("value"), std:("project")}, 
                        object::pair{std:("hint"), std:("Complete project with runtime, agents, and all features")}
                    }, object{
                        object::pair{std:("label"), std:("Plugin - Custom ElizaOS plugin")}, 
                        object::pair{std:("value"), std:("plugin")}, 
                        object::pair{std:("hint"), std:("Extend ElizaOS functionality with custom plugins")}
                    }, object{
                        object::pair{std:("label"), std:("Agent - Character definition file")}, 
                        object::pair{std:("value"), std:("agent")}, 
                        object::pair{std:("hint"), std:("Create a new agent character file")}
                    }, object{
                        object::pair{std:("label"), std:("TEE Project - Trusted Execution Environment project")}, 
                        object::pair{std:("value"), std:("tee")}, 
                        object::pair{std:("hint"), std:("Secure computing environment for privacy-focused applications")}
                    } }}, 
                    object::pair{std:("initialValue"), std:("project")}
                }); });
                if (clack->isCancel(selectedType)) {
                    clack->cancel(std:("Operation cancelled."));
                    process->exit(0);
                }
                projectType = as<any>(selectedType);
            }
            if (!isNonInteractive) {
                auto nameInput = std::async([=]() { clack->text(object{
                    object::pair{std:("message"), std:("What is the name of your ") + projectType + std:("?")}, 
                    object::pair{std:("placeholder"), std:("my-") + projectType + string_empty}, 
                    object::pair{std:("validate"), [=](auto value) mutable
                    {
                        if (!value) return std:("Name is required");
                        if (projectType == std:("agent")) {
                            return (value["length"] > 0) ? any(undefined) (std:("Agent name cannot be empty"));
                        }
                        auto validation = validateProjectName(value);
                        return (validation["isValid"]) ? any(undefined) (validation["error"]);
                    }
                    }
                }); });
                if (clack->isCancel(nameInput)) {
                    clack->cancel(std:("Operation cancelled."));
                    process->exit(0);
                }
                projectName = as<string>(nameInput);
            } else {
                throw any(std::make_shared<Error>(std:("Project name is required. Usage: elizaos create [name]")));
            }
        }
        if (projectType != std:("agent")) {
            auto nameValidation = validateProjectName(projectName);
            if (!nameValidation["isValid"]) {
                throw any(std::make_shared<Error>(nameValidation["error"]));
            }
        }
        auto targetDir = options->dir;
        static switch_type __switch4413_6308 = {
            { any(std:("plugin")), 1 },
            { any(std:("agent")), 2 },
            { any(std:("tee")), 3 },
            { any(std:("project")), 4 }
        };
        switch (__switch4413_6308[projectType])
        {
        case 1:
            std::async([=]() { createPlugin(projectName, targetDir, isNonInteractive); });
            break;
        case 2:
            std::async([=]() { createAgent(projectName, targetDir, isNonInteractive); });
            break;
        case 3:
            {
                auto database = std:("pglite");
                auto aiModel = std:("local");
                any embeddingModel;
                if (!isNonInteractive) {
                    database = std::async([=]() { selectDatabase(); });
                    aiModel = std::async([=]() { selectAIModel(); });
                    if (OR((aiModel == std:("claude")), (aiModel == std:("openrouter")))) {
                        embeddingModel = std::async([=]() { selectEmbeddingModel(); });
                    }
                }
                std::async([=]() { createTEEProject(projectName, targetDir, database, aiModel, embeddingModel, isNonInteractive); });
                break;
            }
        case 4:
        default:
            {
                auto database = std:("pglite");
                auto aiModel = std:("local");
                any embeddingModel;
                if (!isNonInteractive) {
                    database = std::async([=]() { selectDatabase(); });
                    aiModel = std::async([=]() { selectAIModel(); });
                    if (OR((aiModel == std:("claude")), (aiModel == std:("openrouter")))) {
                        embeddingModel = std::async([=]() { selectEmbeddingModel(); });
                    }
                }
                std::async([=]() { createProject(projectName, targetDir, database, aiModel, embeddingModel, isNonInteractive); });
                break;
            }
        }
        if (!isNonInteractive) {
            clack->outro(colors->green(std:("Project created successfully! 🎉")));
        }
    }
    catch (const any& error)
    {
        if (!opts["yes"]) {
            clack->cancel(std:("Failed to create project."));
        }
        logger->error(std:("Create command failed:"), error);
        handleError(error);
        process->exit(1);
    }
}
);

void Main(void)
{
}

MAIN
