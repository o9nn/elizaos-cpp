#include "creators.hpp"
#include <string>

std::shared_ptr<Promise<void>> handleInteractiveConfiguration(string targetDir, string database, string aiModel, string embeddingModel)
{
    auto envFilePath = string_empty + targetDir + std::string("/.env");
    if (database == std::string("postgres")) {
        std::async([=]() { promptAndStorePostgresUrl(envFilePath); });
    }
    if (OR((aiModel != std::string("local")), (embeddingModel))) {
        if (aiModel != std::string("local")) {
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
        throw any(std::make_shared<Error>(OR((nameResult["error"]), (std::string("Invalid plugin name")))));
    }
    auto processedName = nameResult["processedName"];
    shared pluginDirName = (processedName->startsWith(std::string("plugin-"))) ? processedName : std::string("plugin-") + processedName + string_empty;
    if (pluginDirName != pluginName) {
        console->warn(std::string("\
Warning: changing "") + pluginName + std::string("" to "") + pluginDirName + std::string("" to conform to plugin naming conventions\
"));
    }
    shared pluginTargetDir = join(targetDir, pluginDirName);
    auto dirResult = std::async([=]() { validateTargetDirectory(pluginTargetDir); });
    if (!dirResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((dirResult["error"]), (std::string("Invalid target directory")))));
    }
    if (!isNonInteractive) {
        auto displayDir = getDisplayDirectory(targetDir);
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std::string("message"), std::string("Create plugin "") + pluginDirName + std::string("" in ") + displayDir + std::string("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std::string("Plugin creation cancelled."));
            process->exit(0);
        }
    }
    std::async([=]() { withCleanupOnInterrupt(pluginTargetDir, pluginDirName, [=]() mutable
    {
        shared templateName = (pluginType == std::string("quick")) ? std::string("plugin-quick") : std::string("plugin");
        std::async([=]() { runTasks(array<any>{ createTask(std::string("Copying plugin template"), [=]() mutable
        {
            return copyTemplateUtil(as<any>(templateName), pluginTargetDir);
        }
        ), createTask(std::string("Installing dependencies"), [=]() mutable
        {
            return installDependenciesWithSpinner(pluginTargetDir);
        }
        ) }); });
        console->info(std::string("\
") + colors->green(std::string("✓")) + std::string(" Plugin "") + pluginDirName + std::string("" created successfully!"));
        console->info(std::string("\
Next steps:"));
        console->info(std::string("  cd ") + pluginDirName + string_empty);
        console->info(std::string("  bun run build   # Build the plugin"));
        console->info(std::string("\
  Common commands:"));
        console->info(std::string("  elizaos dev    # Start development mode with hot reloading"));
        console->info(std::string("  elizaos start  # Start in production mode"));
        console->info(std::string("\
") + colors->yellow(std::string("⚠️")) + std::string("  Security reminder:"));
        console->info(std::string("  - Check .gitignore is present before committing"));
        console->info(std::string("  - Never commit .env files or API keys"));
        console->info(std::string("  - Add sensitive files to .gitignore if needed\
"));
    }
    ); });
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
        auto displayDir = getDisplayDirectory(targetDir);
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std::string("message"), std::string("Create agent "") + agentName + std::string("" in ") + displayDir + std::string("?")}
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
    console->info(std::string("\
") + colors->green(std::string("✓")) + std::string(" Agent "") + agentName + std::string("" created successfully!"));
    console->info(std::string("Agent character created successfully at: ") + agentFilePath + string_empty);
    console->info(std::string("\
To use this agent:"));
    console->info(std::string("  1. Start ElizaOS server with this character:"));
    console->info(std::string("     elizaos start --character ") + agentFilePath + string_empty);
    console->info(std::string("\
  OR if a server is already running:"));
    console->info(std::string("     elizaos agent start --path ") + agentFilePath + string_empty);
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createTEEProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel, boolean isNonInteractive)
{
    process->env.Delete("PGLITE_DATA_DIR");
    shared teeTargetDir = join(targetDir, projectName);
    auto dirResult = std::async([=]() { validateTargetDirectory(teeTargetDir); });
    if (!dirResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((dirResult["error"]), (std::string("Invalid target directory")))));
    }
    if (!isNonInteractive) {
        auto displayDir = getDisplayDirectory(targetDir);
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std::string("message"), std::string("Create TEE project "") + projectName + std::string("" in ") + displayDir + std::string("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std::string("TEE project creation cancelled."));
            process->exit(0);
        }
    }
    std::async([=]() { withCleanupOnInterrupt(teeTargetDir, projectName, [=]() mutable
    {
        std::async([=]() { fs->mkdir(teeTargetDir, object{
            object::pair{std::string("recursive"), true}
        }); });
        if (!isNonInteractive) {
            std::async([=]() { handleInteractiveConfiguration(teeTargetDir, database, aiModel, embeddingModel); });
        }
        std::async([=]() { runTasks(array<any>{ createTask(std::string("Copying TEE template"), [=]() mutable
        {
            return copyTemplateUtil(std::string("project-tee-starter"), teeTargetDir);
        }
        ), createTask(std::string("Setting up project environment"), [=]() mutable
        {
            return setupProjectEnvironment(teeTargetDir, database, aiModel, embeddingModel, true);
        }
        ), createTask(std::string("Installing dependencies"), [=]() mutable
        {
            return installDependenciesWithSpinner(teeTargetDir);
        }
        ), createTask(std::string("Building project"), [=]() mutable
        {
            return buildProjectWithSpinner(teeTargetDir, false);
        }
        ) }); });
        console->info(std::string("\
") + colors->green(std::string("✓")) + std::string(" TEE project "") + projectName + std::string("" created successfully!"));
        console->info(std::string("\
Next steps:"));
        console->info(std::string("  cd ") + projectName + string_empty);
        console->info(std::string("\
  Common commands:"));
        console->info(std::string("  elizaos dev    # Start development mode with hot reloading"));
        console->info(std::string("  elizaos start  # Start in production mode\
"));
    }
    ); });
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel, boolean isNonInteractive)
{
    process->env.Delete("PGLITE_DATA_DIR");
    shared projectTargetDir = (projectName == std::string(".")) ? targetDir : join(targetDir, projectName);
    auto dirResult = std::async([=]() { validateTargetDirectory(projectTargetDir); });
    if (!dirResult["isValid"]) {
        throw any(std::make_shared<Error>(OR((dirResult["error"]), (std::string("Invalid target directory")))));
    }
    if (!isNonInteractive) {
        auto displayDir = getDisplayDirectory(targetDir);
        auto displayProjectName = (projectName == std::string(".")) ? any(std::string("project")) (std::string("project "") + projectName + std::string("""));
        auto confirmCreate = std::async([=]() { clack->confirm(object{
            object::pair{std::string("message"), std::string("Create ") + displayProjectName + std::string(" in ") + displayDir + std::string("?")}
        }); });
        if (OR((clack->isCancel(confirmCreate)), (!confirmCreate))) {
            clack->cancel(std::string("Project creation cancelled."));
            process->exit(0);
        }
    }
    auto createFn = [=]() mutable
    {
        if (projectName != std::string(".")) {
            std::async([=]() { fs->mkdir(projectTargetDir, object{
                object::pair{std::string("recursive"), true}
            }); });
        }
        if (!isNonInteractive) {
            std::async([=]() { handleInteractiveConfiguration(projectTargetDir, database, aiModel, embeddingModel); });
        }
        std::async([=]() { runTasks(array<any>{ createTask(std::string("Copying project template"), [=]() mutable
        {
            return copyTemplateUtil(std::string("project-starter"), projectTargetDir);
        }
        ), createTask(std::string("Setting up project environment"), [=]() mutable
        {
            return setupProjectEnvironment(projectTargetDir, database, aiModel, embeddingModel, true);
        }
        ), createTask(std::string("Installing dependencies"), [=]() mutable
        {
            return installDependenciesWithSpinner(projectTargetDir);
        }
        ), createTask(std::string("Building project"), [=]() mutable
        {
            return buildProjectWithSpinner(projectTargetDir, false);
        }
        ) }); });
        auto displayName = (projectName == std::string(".")) ? any(std::string("Project")) (std::string("Project "") + projectName + std::string("""));
        console->info(std::string("\
") + colors->green(std::string("✓")) + std::string(" ") + displayName + std::string(" initialized successfully!"));
        console->info(std::string("\
Next steps:"));
        if (projectName != std::string(".")) {
            console->info(std::string("  cd ") + projectName + string_empty);
        }
        console->info(std::string("\
  Common commands:"));
        console->info(std::string("  elizaos dev    # Start development mode with hot reloading"));
        console->info(std::string("  elizaos start  # Start in production mode\
"));
    };
    if (projectName == std::string(".")) {
        std::async([=]() { createFn(); });
    } else {
        std::async([=]() { withCleanupOnInterrupt(projectTargetDir, projectName, createFn); });
    }
    return std::shared_ptr<Promise<void>>();
};


