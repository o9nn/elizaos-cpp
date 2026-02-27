#include "creators.hpp"

std::shared_ptr<Promise<void>> createPlugin(string pluginName, string targetDir, boolean isNonInteractive)
{
    auto nameResult = processPluginName(pluginName);
    if (!nameResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((nameResult["error"]), (std:("Invalid plugin name")))));
    }
    auto processedName = nameResult["processedName"];
    auto pluginDirName = (processedName->startsWith(std:("plugin-"))) ? processedName : std:("plugin-") + processedName + string_empty;
    auto pluginTargetDir = join(targetDir, pluginDirName);
    auto dirResult = std::async([=]() { validateTargetDirectory(pluginTargetDir); });
    if (!dirResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((dirResult["error"]), (std:("Invalid target directory")))));
    }
    if (!isNonInteractive) {
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std:("message"), std:("Create plugin "") + pluginDirName + std:("" in ") + pluginTargetDir + std:("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std:("Plugin creation cancelled."));
            process->exit(0);
        }
    }
    std::async([=]() { copyTemplateUtil(std:("plugin"), pluginTargetDir); });
    std::async([=]() { installDependencies(pluginTargetDir); });
    console->info(std:("\
") + colors->green(std:("✓")) + std:(" Plugin "") + pluginDirName + std:("" created successfully!"));
    console->info(std:("\
Next steps:"));
    console->info(std:("  cd ") + pluginDirName + string_empty);
    console->info(std:("  bun run build"));
    console->info(std:("  bun run test\
"));
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createAgent(string agentName, string targetDir, boolean isNonInteractive)
{
    auto agentFilePath = join(targetDir, string_empty + agentName + std:(".json"));
    try
    {
        std::async([=]() { fs->access(agentFilePath); });
        throw any(std::make_shared<Error>(std:("Agent file ") + agentFilePath + std:(" already exists")));
    }
    catch (const any& error)
    {
        if (error["code"] != std:("ENOENT")) {
            throw any(error);
        }
    }
    if (!isNonInteractive) {
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std:("message"), std:("Create agent "") + agentName + std:("" at ") + agentFilePath + std:("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std:("Agent creation cancelled."));
            process->exit(0);
        }
    }
    auto agentCharacter = utils::assign(object{
        , 
        object::pair{std:("name"), agentName}, 
        object::pair{std:("bio"), array<string>{ string_empty + agentName + std:(" is a helpful AI assistant created to provide assistance and engage in meaningful conversations."), string_empty + agentName + std:(" is knowledgeable, creative, and always eager to help users with their questions and tasks.") }}
    }, getElizaCharacter());
    std::async([=]() { fs->writeFile(agentFilePath, JSON->stringify(agentCharacter, nullptr, 2)); });
    if (!isNonInteractive) {
        console->info(std:("\
") + colors->green(std:("✓")) + std:(" Agent "") + agentName + std:("" created successfully!"));
    }
    console->info(std:("Agent character created successfully at: ") + agentFilePath + string_empty);
    console->info(std:("\
To use this agent:"));
    console->info(std:("  elizaos agent start --path ") + agentFilePath + std:("\
"));
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createTEEProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel, boolean isNonInteractive)
{
    auto teeTargetDir = join(targetDir, projectName);
    auto dirResult = std::async([=]() { validateTargetDirectory(teeTargetDir); });
    if (!dirResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((dirResult["error"]), (std:("Invalid target directory")))));
    }
    if (!isNonInteractive) {
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std:("message"), std:("Create TEE project "") + projectName + std:("" in ") + teeTargetDir + std:("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std:("TEE project creation cancelled."));
            process->exit(0);
        }
    }
    std::async([=]() { copyTemplateUtil(std:("project-tee-starter"), teeTargetDir); });
    std::async([=]() { setupProjectEnvironment(teeTargetDir, database, aiModel, embeddingModel, isNonInteractive); });
    std::async([=]() { installDependencies(teeTargetDir); });
    std::async([=]() { buildProject(teeTargetDir); });
    console->info(std:("\
") + colors->green(std:("✓")) + std:(" TEE project "") + projectName + std:("" created successfully!"));
    console->info(std:("\
Next steps:"));
    console->info(std:("  cd ") + projectName + string_empty);
    console->info(std:("  bun run dev\
"));
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel, boolean isNonInteractive)
{
    auto projectTargetDir = (projectName == std:(".")) ? targetDir : join(targetDir, projectName);
    auto dirResult = std::async([=]() { validateTargetDirectory(projectTargetDir); });
    if (!dirResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((dirResult["error"]), (std:("Invalid target directory")))));
    }
    if (!isNonInteractive) {
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std:("message"), std:("Create project "") + projectName + std:("" in ") + projectTargetDir + std:("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std:("Project creation cancelled."));
            process->exit(0);
        }
    }
    std::async([=]() { copyTemplateUtil(std:("project-starter"), projectTargetDir); });
    std::async([=]() { setupProjectEnvironment(projectTargetDir, database, aiModel, embeddingModel, isNonInteractive); });
    std::async([=]() { installDependencies(projectTargetDir); });
    std::async([=]() { buildProject(projectTargetDir); });
    auto displayName = (projectName == std:(".")) ? any(std:("Project")) (std:("Project "") + projectName + std:("""));
    console->info(std:("\
") + colors->green(std:("✓")) + std:(" ") + displayName + std:(" initialized successfully!"));
    console->info(std:("\
Next steps:"));
    console->info(std:("  cd ") + projectName + string_empty);
    console->info(std:("  bun run dev\
"));
    return std::shared_ptr<Promise<void>>();
};


