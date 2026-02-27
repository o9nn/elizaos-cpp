#include "index.hpp"

any plugins = ((std::make_shared<Command>()))->name(std:("plugins"))->description(std:("Manage ElizaOS plugins"))->action([=]() mutable
{
    plugins->help();
}
);
any pluginsCommand = plugins->command(std:("list"))->aliases(array<string>{ std:("l"), std:("ls") })->description(std:("List available plugins to install into the project (shows v1.x plugins by default)"))->option(std:("--all"), std:("List all plugins from the registry with detailed version info"))->option(std:("--v0"), std:("List only v0.x compatible plugins"))->action([=](auto opts) mutable
{
    try
    {
        std::async([=]() { listAvailablePlugins(opts); });
    }
    catch (const any& error)
    {
        handleError(error);
    }
}
);

void Main(void)
{
    plugins->command(std:("add"))->alias(std:("install"))->description(std:("Add a plugin to the project"))->argument(std:("<plugin>"), std:("plugin name (e.g., "abc", "plugin-abc", "elizaos/plugin-abc")"))->option(std:("-s, --skip-env-prompt"), std:("Skip prompting for environment variables"))->option(std:("--skip-verification"), std:("Skip plugin import verification after installation"))->option(std:("-b, --branch <branchName>"), std:("Branch to install from when using monorepo source"), std:("main"))->option(std:("-T, --tag <tagname>"), std:("Specify a tag to install (e.g., beta)"))->action([=](auto pluginArg, auto opts) mutable
    {
        try
        {
            std::async([=]() { addPlugin(pluginArg, opts); });
        }
        catch (const any& error)
        {
            handleError(error);
        }
    }
    );
    plugins->command(std:("installed-plugins"))->description(std:("List plugins found in the project dependencies"))->action([=]() mutable
    {
        try
        {
            std::async([=]() { listInstalledPlugins(); });
        }
        catch (const any& error)
        {
            if (is<SyntaxError>(error)) {
                console->error(std:("Error parsing package.json: ") + error->message + string_empty);
                process->exit(1);
            }
            handleError(error);
            process->exit(1);
        }
    }
    );
    plugins->command(std:("remove"))->aliases(array<string>{ std:("delete"), std:("del"), std:("rm") })->description(std:("Remove a plugin from the project"))->argument(std:("<plugin>"), std:("plugins name (e.g., "abc", "plugin-abc", "elizaos/plugin-abc")"))->action([=](auto plugin, auto _opts) mutable
    {
        try
        {
            std::async([=]() { removePlugin(plugin); });
        }
        catch (const any& error)
        {
            handleError(error);
            process->exit(1);
        }
    }
    );
    plugins->command(std:("upgrade"))->description(std:("Upgrade a plugin from version 0.x to 1.x using AI-powered migration"))->argument(std:("<path>"), std:("GitHub repository URL or local folder path"))->option(std:("--api-key <key>"), std:("Anthropic API key (or use ANTHROPIC_API_KEY env var)"))->option(std:("--skip-tests"), std:("Skip test validation loop"))->option(std:("--skip-validation"), std:("Skip production readiness validation"))->action([=](auto pluginPath, auto opts) mutable
    {
        std::async([=]() { upgradePlugin(pluginPath, opts); });
    }
    );
    plugins->command(std:("generate"))->description(std:("Generate a new plugin using AI-powered code generation"))->option(std:("--api-key <key>"), std:("Anthropic API key (or use ANTHROPIC_API_KEY env var)"))->option(std:("--skip-tests"), std:("Skip test validation loop"))->option(std:("--skip-validation"), std:("Skip production readiness validation"))->option(std:("--skip-prompts"), std:("Skip interactive prompts (requires --spec-file)"))->option(std:("--spec-file <path>"), std:("Path to JSON file containing plugin specification"))->action([=](auto opts) mutable
    {
        std::async([=]() { generatePlugin(opts); });
    }
    );
}

MAIN
