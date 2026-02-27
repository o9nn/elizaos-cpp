#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/commands/create/actions/creators.h"

std::shared_ptr<Promise<void>> handleInteractiveConfiguration(string targetDir, string database, string aiModel, string embeddingModel)
{
    auto envFilePath = string_empty + targetDir + std:("/.env");
    if (database == std:("postgres")) {
        std::async([=]() { promptAndStorePostgresUrl(envFilePath); });
    }
    if (OR((aiModel != std:("local")), (embeddingModel))) {
        if (aiModel != std:("local")) {
            std::async([=]() { setupAIModelConfig(aiModel, envFilePath, false); });
        }
        if (embeddingModel) {
            std::async([=]() { setupEmbeddingModelConfig(embeddingModel, envFilePath, false); });
        }
    }
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createPlugin(string pluginName, string targetDir, string pluginType, boolean isNonInteractive)
{
    auto nameResult = processPluginName(pluginName);
    if (!nameResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((nameResult["error"]), (std:("Invalid plugin name")))));
    }
    auto processedName = nameResult["processedName"];
    shared pluginDirName = (processedName->startsWith(std:("plugin-"))) ? processedName : std:("plugin-") + processedName + string_empty;
    if (pluginDirName != pluginName) {
        console->warn(std:("\
Warning: changing "") + pluginName + std:("" to "") + pluginDirName + std:("" to conform to plugin naming conventions\
"));
    }
    shared pluginTargetDir = join(targetDir, pluginDirName);
    auto dirResult = std::async([=]() { validateTargetDirectory(pluginTargetDir); });
    if (!dirResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((dirResult["error"]), (std:("Invalid target directory")))));
    }
    if (!isNonInteractive) {
        auto displayDir = getDisplayDirectory(targetDir);
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std:("message"), std:("Create plugin "") + pluginDirName + std:("" in ") + displayDir + std:("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std:("Plugin creation cancelled."));
            process->exit(0);
        }
    }
    std::async([=]() { withCleanupOnInterrupt(pluginTargetDir, pluginDirName, [=]() mutable
    {
        shared templateName = (pluginType == std:("quick")) ? std:("plugin-quick") : std:("plugin");
        std::async([=]() { runTasks(array<any>{ createTask(std:("Copying plugin template"), [=]() mutable
        {
            return copyTemplateUtil(as<any>(templateName), pluginTargetDir);
        }
        ), createTask(std:("Installing dependencies"), [=]() mutable
        {
            return installDependenciesWithSpinner(pluginTargetDir);
        }
        ) }); });
        console->info(std:("\
") + colors->green(std:("✓")) + std:(" Plugin "") + pluginDirName + std:("" created successfully!"));
        console->info(std:("\
Next steps:"));
        console->info(std:("  cd ") + pluginDirName + string_empty);
        console->info(std:("  bun run build   # Build the plugin"));
        console->info(std:("\
  Common commands:"));
        console->info(std:("  elizaos dev    # Start development mode with hot reloading"));
        console->info(std:("  elizaos start  # Start in production mode"));
        console->info(std:("\
") + colors->yellow(std:("⚠️")) + std:("  Security reminder:"));
        console->info(std:("  - Check .gitignore is present before committing"));
        console->info(std:("  - Never commit .env files or API keys"));
        console->info(std:("  - Add sensitive files to .gitignore if needed\
"));
    }
    ); });
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
        auto displayDir = getDisplayDirectory(targetDir);
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std:("message"), std:("Create agent "") + agentName + std:("" in ") + displayDir + std:("?")}
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
    console->info(std:("\
") + colors->green(std:("✓")) + std:(" Agent "") + agentName + std:("" created successfully!"));
    console->info(std:("Agent character created successfully at: ") + agentFilePath + string_empty);
    console->info(std:("\
To use this agent:"));
    console->info(std:("  1. Start ElizaOS server with this character:"));
    console->info(std:("     elizaos start --character ") + agentFilePath + string_empty);
    console->info(std:("\
  OR if a server is already running:"));
    console->info(std:("     elizaos agent start --path ") + agentFilePath + string_empty);
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createTEEProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel, boolean isNonInteractive)
{
    process->env.Delete("PGLITE_DATA_DIR");
    shared teeTargetDir = join(targetDir, projectName);
    auto dirResult = std::async([=]() { validateTargetDirectory(teeTargetDir); });
    if (!dirResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((dirResult["error"]), (std:("Invalid target directory")))));
    }
    if (!isNonInteractive) {
        auto displayDir = getDisplayDirectory(targetDir);
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std:("message"), std:("Create TEE project "") + projectName + std:("" in ") + displayDir + std:("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std:("TEE project creation cancelled."));
            process->exit(0);
        }
    }
    std::async([=]() { withCleanupOnInterrupt(teeTargetDir, projectName, [=]() mutable
    {
        std::async([=]() { fs->mkdir(teeTargetDir, object{
            object::pair{std:("recursive"), true}
        }); });
        if (!isNonInteractive) {
            std::async([=]() { handleInteractiveConfiguration(teeTargetDir, database, aiModel, embeddingModel); });
        }
        std::async([=]() { runTasks(array<any>{ createTask(std:("Copying TEE template"), [=]() mutable
        {
            return copyTemplateUtil(std:("project-tee-starter"), teeTargetDir);
        }
        ), createTask(std:("Setting up project environment"), [=]() mutable
        {
            return setupProjectEnvironment(teeTargetDir, database, aiModel, embeddingModel, true);
        }
        ), createTask(std:("Installing dependencies"), [=]() mutable
        {
            return installDependenciesWithSpinner(teeTargetDir);
        }
        ), createTask(std:("Building project"), [=]() mutable
        {
            return buildProjectWithSpinner(teeTargetDir, false);
        }
        ) }); });
        console->info(std:("\
") + colors->green(std:("✓")) + std:(" TEE project "") + projectName + std:("" created successfully!"));
        console->info(std:("\
Next steps:"));
        console->info(std:("  cd ") + projectName + string_empty);
        console->info(std:("\
  Common commands:"));
        console->info(std:("  elizaos dev    # Start development mode with hot reloading"));
        console->info(std:("  elizaos start  # Start in production mode\
"));
    }
    ); });
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel, boolean isNonInteractive)
{
    process->env.Delete("PGLITE_DATA_DIR");
    shared projectTargetDir = (projectName == std:(".")) ? targetDir : join(targetDir, projectName);
    auto dirResult = std::async([=]() { validateTargetDirectory(projectTargetDir); });
    if (!dirResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((dirResult["error"]), (std:("Invalid target directory")))));
    }
    if (!isNonInteractive) {
        auto displayDir = getDisplayDirectory(targetDir);
        auto displayProjectName = (projectName == std:(".")) ? any(std:("project")) (std:("project "") + projectName + std:("""));
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std:("message"), std:("Create ") + displayProjectName + std:(" in ") + displayDir + std:("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std:("Project creation cancelled."));
            process->exit(0);
        }
    }
    auto createFn = [=]() mutable
    {
        if (projectName != std:(".")) {
            std::async([=]() { fs->mkdir(projectTargetDir, object{
                object::pair{std:("recursive"), true}
            }); });
        }
        if (!isNonInteractive) {
            std::async([=]() { handleInteractiveConfiguration(projectTargetDir, database, aiModel, embeddingModel); });
        }
        std::async([=]() { runTasks(array<any>{ createTask(std:("Copying project template"), [=]() mutable
        {
            return copyTemplateUtil(std:("project-starter"), projectTargetDir);
        }
        ), createTask(std:("Setting up project environment"), [=]() mutable
        {
            return setupProjectEnvironment(projectTargetDir, database, aiModel, embeddingModel, true);
        }
        ), createTask(std:("Installing dependencies"), [=]() mutable
        {
            return installDependenciesWithSpinner(projectTargetDir);
        }
        ), createTask(std:("Building project"), [=]() mutable
        {
            return buildProjectWithSpinner(projectTargetDir, false);
        }
        ) }); });
        auto displayName = (projectName == std:(".")) ? any(std:("Project")) (std:("Project "") + projectName + std:("""));
        console->info(std:("\
") + colors->green(std:("✓")) + std:(" ") + displayName + std:(" initialized successfully!"));
        console->info(std:("\
Next steps:"));
        if (projectName != std:(".")) {
            console->info(std:("  cd ") + projectName + string_empty);
        }
        console->info(std:("\
  Common commands:"));
        console->info(std:("  elizaos dev    # Start development mode with hot reloading"));
        console->info(std:("  elizaos start  # Start in production mode\
"));
    };
    if (projectName == std:(".")) {
        std::async([=]() { createFn(); });
    } else {
        std::async([=]() { withCleanupOnInterrupt(projectTargetDir, projectName, createFn); });
    }
    return std::shared_ptr<Promise<void>>();
};


