#include "index.hpp"
#include <string>

any plugins = ((std::make_shared<Command>()))->name(std::string("plugins"))->description(std::string("Manage ElizaOS plugins"))->action([=]() mutable
{
    plugins->help();
}
);
any pluginsCommand = plugins->command(std::string("list"))->aliases(array<string>{ std::string("l"), std::string("ls") })->description(std::string("List available plugins to install into the project (shows v1.x plugins by default)"))->option(std::string("--all"), std::string("List all plugins from the registry with detailed version info"))->option(std::string("--v0"), std::string("List only v0.x compatible plugins"))->action([=](auto opts) mutable
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
    plugins->command(std::string("add"))->alias(std::string("install"))->description(std::string("Add a plugin to the project"))->argument(std::string("<plugin>"), std::string("plugin name (e.g., "abc", "plugin-abc", "elizaos/plugin-abc")"))->option(std::string("-s, --skip-env-prompt"), std::string("Skip prompting for environment variables"))->option(std::string("--skip-verification"), std::string("Skip plugin import verification after installation"))->option(std::string("-b, --branch <branchName>"), std::string("Branch to install from when using monorepo source"), std::string("main"))->option(std::string("-T, --tag <tagname>"), std::string("Specify a tag to install (e.g., beta)"))->action([=](auto pluginArg, auto opts) mutable
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
    plugins->command(std::string("installed-plugins"))->description(std::string("List plugins found in the project dependencies"))->action([=]() mutable
    {
        try
        {
            std::async([=]() { listInstalledPlugins(); });
        }
        catch (const any& error)
        {
            if (is<SyntaxError>(error)) {
                console->error(std::string("Error parsing package.json: ") + error->message + string_empty);
                process->exit(1);
            }
            handleError(error);
            process->exit(1);
        }
    }
    );
    plugins->command(std::string("remove"))->aliases(array<string>{ std::string("delete"), std::string("del"), std::string("rm") })->description(std::string("Remove a plugin from the project"))->argument(std::string("<plugin>"), std::string("plugins name (e.g., "abc", "plugin-abc", "elizaos/plugin-abc")"))->action([=](auto plugin, auto _opts) mutable
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
    plugins->command(std::string("upgrade"))->description(std::string("Upgrade a plugin from version 0.x to 1.x using AI-powered migration (requires Claude Code CLI)"))->argument(std::string("<path>"), std::string("GitHub repository URL or local folder path"))->option(std::string("--api-key <key>"), std::string("Anthropic API key (or use ANTHROPIC_API_KEY env var)"))->option(std::string("--skip-tests"), std::string("Skip test validation loop"))->option(std::string("--skip-validation"), std::string("Skip production readiness validation"))->option(std::string("--quiet"), std::string("Suppress progress display"))->option(std::string("--verbose"), std::string("Show detailed information"))->option(std::string("--debug"), std::string("Show debug information"))->option(std::string("--skip-confirmation"), std::string("Skip user confirmation"))->action([=](auto pluginPath, auto opts) mutable
    {
        std::async([=]() { upgradePlugin(pluginPath, opts); });
    }
    );
    plugins->command(std::string("generate"))->description(std::string("Generate a new plugin using AI-powered code generation"))->option(std::string("--api-key <key>"), std::string("Anthropic API key (or use ANTHROPIC_API_KEY env var)"))->option(std::string("--skip-tests"), std::string("Skip test validation loop"))->option(std::string("--skip-validation"), std::string("Skip production readiness validation"))->option(std::string("--skip-prompts"), std::string("Skip interactive prompts (requires --spec-file)"))->option(std::string("--spec-file <path>"), std::string("Path to JSON file containing plugin specification"))->action([=](auto opts) mutable
    {
        std::async([=]() { generatePlugin(opts); });
    }
    );
}

MAIN
