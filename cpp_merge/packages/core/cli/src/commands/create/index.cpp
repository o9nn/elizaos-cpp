#include "index.hpp"
#include <string>

any create = ((std::make_shared<Command>(std::string("create"))))->description(std::string("Create a new ElizaOS project, plugin, agent, or TEE project"))->argument(std::string("[name]"), std::string("name of the project/plugin/agent to create"))->option(std::string("--dir <dir>"), std::string("directory to create the project in"), std::string("."))->option(std::string("--yes, -y"), std::string("skip prompts and use defaults"))->option(std::string("--type <type>"), std::string("type of project to create (project, plugin, agent, tee)"), std::string("project"))->action([=](auto name = std::nullopt, auto opts = std::nullopt) mutable
{
    try
    {
        if (OR((OR((OR((process->env->ELIZA_NONINTERACTIVE == std::string("1")), (process->env->ELIZA_NONINTERACTIVE == std::string("true")))), (process->argv->includes(std::string("-y"))))), (process->argv->includes(std::string("--yes"))))) {
            if (opts) {
                opts["yes"] = true;
            } else {
                opts = object{
                    object::pair{std::string("yes"), true}
                };
            }
        }
        auto options = validateCreateOptions(OR((opts), (object{})));
        auto isNonInteractive = options->yes;
        if (!isNonInteractive) {
            std::async([=]() { displayBanner(); });
            clack->intro(colors->inverse(std::string(" Creating ElizaOS Project ")));
        }
        shared projectType = options->type;
        auto projectName = name;
        if (!projectName) {
            if (!isNonInteractive) {
                auto selectedType = std::async([=]() { clack->select(object{
                    object::pair{std::string("message"), std::string("What would you like to create?")}, 
                    object::pair{std::string("options"), array<object>{ object{
                        object::pair{std::string("label"), std::string("Project - Full ElizaOS application")}, 
                        object::pair{std::string("value"), std::string("project")}, 
                        object::pair{std::string("hint"), std::string("Complete project with runtime, agents, and all features")}
                    }, object{
                        object::pair{std::string("label"), std::string("Plugin - Custom ElizaOS plugin")}, 
                        object::pair{std::string("value"), std::string("plugin")}, 
                        object::pair{std::string("hint"), std::string("Extend ElizaOS functionality with custom plugins")}
                    }, object{
                        object::pair{std::string("label"), std::string("Agent - Character definition file")}, 
                        object::pair{std::string("value"), std::string("agent")}, 
                        object::pair{std::string("hint"), std::string("Create a new agent character file")}
                    }, object{
                        object::pair{std::string("label"), std::string("TEE Project - Trusted Execution Environment project")}, 
                        object::pair{std::string("value"), std::string("tee")}, 
                        object::pair{std::string("hint"), std::string("Secure computing environment for privacy-focused applications")}
                    } }}, 
                    object::pair{std::string("initialValue"), std::string("project")}
                }); });
                if (clack->isCancel(selectedType)) {
                    clack->cancel(std::string("Operation cancelled."));
                    process->exit(0);
                }
                projectType = as<any>(selectedType);
            }
            if (!isNonInteractive) {
                auto nameInput = std::async([=]() { clack->text(object{
                    object::pair{std::string("message"), std::string("What is the name of your ") + projectType + std::string("?")}, 
                    object::pair{std::string("placeholder"), std::string("my-") + projectType + string_empty}, 
                    object::pair{std::string("validate"), [=](auto value) mutable
                    {
                        if (!value) return std::string("Name is required");
                        if (projectType == std::string("agent")) {
                            return (value["length"] > 0) ? any(std::nullopt) (std::string("Agent name cannot be empty"));
                        }
                        auto validation = validateProjectName(value);
                        return (validation["isValid"]) ? any(std::nullopt) (validation["error"]);
                    }
                    }
                }); });
                if (clack->isCancel(nameInput)) {
                    clack->cancel(std::string("Operation cancelled."));
                    process->exit(0);
                }
                projectName = as<string>(nameInput);
            } else {
                throw any(std::make_shared<Error>(std::string("Project name is required. Usage: elizaos create [name]")));
            }
        }
        if (projectType != std::string("agent")) {
            auto nameValidation = validateProjectName(projectName);
            if (!nameValidation["isValid"]) {
                throw any(std::make_shared<Error>(nameValidation["error"]));
            }
        }
        auto targetDir = options->dir;
        static switch_type __switch4413_6308 = {
            { any(std::string("plugin")), 1 },
            { any(std::string("agent")), 2 },
            { any(std::string("tee")), 3 },
            { any(std::string("project")), 4 }
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
                auto database = std::string("pglite");
                auto aiModel = std::string("local");
                any embeddingModel;
                if (!isNonInteractive) {
                    database = std::async([=]() { selectDatabase(); });
                    aiModel = std::async([=]() { selectAIModel(); });
                    if (OR((aiModel == std::string("claude")), (aiModel == std::string("openrouter")))) {
                        embeddingModel = std::async([=]() { selectEmbeddingModel(); });
                    }
                }
                std::async([=]() { createTEEProject(projectName, targetDir, database, aiModel, embeddingModel, isNonInteractive); });
                break;
            }
        case 4:
        default:
            {
                auto database = std::string("pglite");
                auto aiModel = std::string("local");
                any embeddingModel;
                if (!isNonInteractive) {
                    database = std::async([=]() { selectDatabase(); });
                    aiModel = std::async([=]() { selectAIModel(); });
                    if (OR((aiModel == std::string("claude")), (aiModel == std::string("openrouter")))) {
                        embeddingModel = std::async([=]() { selectEmbeddingModel(); });
                    }
                }
                std::async([=]() { createProject(projectName, targetDir, database, aiModel, embeddingModel, isNonInteractive); });
                break;
            }
        }
        if (!isNonInteractive) {
            clack->outro(colors->green(std::string("Project created successfully! 🎉")));
        }
    }
    catch (const any& error)
    {
        if (!opts["yes"]) {
            clack->cancel(std::string("Failed to create project."));
        }
        logger->error(std::string("Create command failed:"), error);
        handleError(error);
        process->exit(1);
    }
}
);

void Main(void)
{
}

MAIN
