#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/create/actions/creators.h"

std::shared_ptr<Promise<void>> createPlugin(string pluginName, string targetDir, boolean isNonInteractive)
{
    auto nameResult = processPluginName(pluginName);
    if (!nameResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((nameResult["error"]), (std::string("Invalid plugin name")))));
    }
    auto processedName = nameResult["processedName"];
    auto pluginDirName = (processedName->startsWith(std::string("plugin-"))) ? processedName : std::string("plugin-") + processedName + string_empty;
    auto pluginTargetDir = join(targetDir, pluginDirName);
    auto dirResult = std::async([=]() { validateTargetDirectory(pluginTargetDir); });
    if (!dirResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((dirResult["error"]), (std::string("Invalid target directory")))));
    }
    if (!isNonInteractive) {
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std::string("message"), std::string("Create plugin "") + pluginDirName + std::string("" in ") + pluginTargetDir + std::string("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std::string("Plugin creation cancelled."));
            process->exit(0);
        }
    }
    std::async([=]() { copyTemplateUtil(std::string("plugin"), pluginTargetDir); });
    std::async([=]() { installDependencies(pluginTargetDir); });
    console->info(std::string("\
") + colors->green(std::string("✓")) + std::string(" Plugin "") + pluginDirName + std::string("" created successfully!"));
    console->info(std::string("\
Next steps:"));
    console->info(std::string("  cd ") + pluginDirName + string_empty);
    console->info(std::string("  bun run build"));
    console->info(std::string("  bun run test\
"));
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createAgent(string agentName, string targetDir, boolean isNonInteractive)
{
    auto agentFilePath = join(targetDir, string_empty + agentName + std::string(".json"));
    try
    {
        std::async([=]() { fs->access(agentFilePath); });
        throw any(std::make_shared<Error>(std::string("Agent file ") + agentFilePath + std::string(" already exists")));
    }
    catch (const any& error)
    {
        if (error["code"] != std::string("ENOENT")) {
            throw any(error);
        }
    }
    if (!isNonInteractive) {
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std::string("message"), std::string("Create agent "") + agentName + std::string("" at ") + agentFilePath + std::string("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std::string("Agent creation cancelled."));
            process->exit(0);
        }
    }
    auto agentCharacter = utils::assign(object{
        , 
        object::pair{std::string("name"), agentName}, 
        object::pair{std::string("bio"), array<string>{ string_empty + agentName + std::string(" is a helpful AI assistant created to provide assistance and engage in meaningful conversations."), string_empty + agentName + std::string(" is knowledgeable, creative, and always eager to help users with their questions and tasks.") }}
    }, getElizaCharacter());
    std::async([=]() { fs->writeFile(agentFilePath, JSON->stringify(agentCharacter, nullptr, 2)); });
    if (!isNonInteractive) {
        console->info(std::string("\
") + colors->green(std::string("✓")) + std::string(" Agent "") + agentName + std::string("" created successfully!"));
    }
    console->info(std::string("Agent character created successfully at: ") + agentFilePath + string_empty);
    console->info(std::string("\
To use this agent:"));
    console->info(std::string("  elizaos agent start --path ") + agentFilePath + std::string("\
"));
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createTEEProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel, boolean isNonInteractive)
{
    auto teeTargetDir = join(targetDir, projectName);
    auto dirResult = std::async([=]() { validateTargetDirectory(teeTargetDir); });
    if (!dirResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((dirResult["error"]), (std::string("Invalid target directory")))));
    }
    if (!isNonInteractive) {
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std::string("message"), std::string("Create TEE project "") + projectName + std::string("" in ") + teeTargetDir + std::string("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std::string("TEE project creation cancelled."));
            process->exit(0);
        }
    }
    std::async([=]() { copyTemplateUtil(std::string("project-tee-starter"), teeTargetDir); });
    std::async([=]() { setupProjectEnvironment(teeTargetDir, database, aiModel, embeddingModel, isNonInteractive); });
    std::async([=]() { installDependencies(teeTargetDir); });
    std::async([=]() { buildProject(teeTargetDir); });
    console->info(std::string("\
") + colors->green(std::string("✓")) + std::string(" TEE project "") + projectName + std::string("" created successfully!"));
    console->info(std::string("\
Next steps:"));
    console->info(std::string("  cd ") + projectName + string_empty);
    console->info(std::string("  bun run dev\
"));
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel, boolean isNonInteractive)
{
    auto projectTargetDir = (projectName == std::string(".")) ? targetDir : join(targetDir, projectName);
    auto dirResult = std::async([=]() { validateTargetDirectory(projectTargetDir); });
    if (!dirResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((dirResult["error"]), (std::string("Invalid target directory")))));
    }
    if (!isNonInteractive) {
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std::string("message"), std::string("Create project "") + projectName + std::string("" in ") + projectTargetDir + std::string("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std::string("Project creation cancelled."));
            process->exit(0);
        }
    }
    std::async([=]() { copyTemplateUtil(std::string("project-starter"), projectTargetDir); });
    std::async([=]() { setupProjectEnvironment(projectTargetDir, database, aiModel, embeddingModel, isNonInteractive); });
    std::async([=]() { installDependencies(projectTargetDir); });
    std::async([=]() { buildProject(projectTargetDir); });
    auto displayName = (projectName == std::string(".")) ? any(std::string("Project")) : any(std::string("Project "") + projectName + std::string("""));
    console->info(std::string("\
") + colors->green(std::string("✓")) + std::string(" ") + displayName + std::string(" initialized successfully!"));
    console->info(std::string("\
Next steps:"));
    console->info(std::string("  cd ") + projectName + string_empty);
    console->info(std::string("  bun run dev\
"));
    return std::shared_ptr<Promise<void>>();
};


