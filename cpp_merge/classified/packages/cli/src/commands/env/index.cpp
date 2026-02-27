#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/commands/env/index.h"

any env = ((std::make_shared<Command>()))->name(std:("env"))->description(std:("Manage environment variables and secrets"));

void Main(void)
{
    env->command(std:("list"))->description(std:("List all environment variables"))->option(std:("--system"), std:("List only system information"))->option(std:("--local"), std:("List only local environment variables"))->action([=](auto options) mutable
    {
        try
        {
            std::async([=]() { handleListCommand(options); });
        }
        catch (const any& error)
        {
            handleError(error);
        }
    }
    );
    env->command(std:("edit-local"))->description(std:("Edit local environment variables"))->option(std:("-y, --yes"), std:("Automatically confirm prompts"))->action([=](auto options) mutable
    {
        try
        {
            std::async([=]() { editEnvVars(options); });
        }
        catch (const any& error)
        {
            handleError(error);
        }
    }
    );
    env->command(std:("reset"))->description(std:("Reset environment variables and clean up database/cache files (interactive selection)"))->option(std:("-y, --yes"), std:("Automatically reset using default selections"))->action([=](auto options) mutable
    {
        try
        {
            std::async([=]() { resetEnv(options); });
        }
        catch (const any& error)
        {
            handleError(error);
        }
    }
    );
    env->command(std:("interactive"))->description(std:("Interactive environment variable management"))->option(std:("-y, --yes"), std:("Automatically confirm prompts"))->action([=](auto options) mutable
    {
        try
        {
            std::async([=]() { showMainMenu(options); });
        }
        catch (const any& error)
        {
            handleError(error);
        }
    }
    );
    env->action([=]() mutable
    {
        console->log(colors->bold(std:("\
Eliza Environment Variable Manager")));
        console->log(std:("\
Available commands:"));
        console->log(std:("  list                  List all environment variables"));
        console->log(std:("  edit-local            Edit local environment variables"));
        console->log(std:("  reset                 Reset environment variables and clean up database/cache files (interactive selection)"));
        console->log(std:("  interactive           Start interactive environment variable manager"));
        console->log(std:("\
You can also edit environment variables in the web UI:"));
        console->log(std:("  http://localhost:3000/settings"));
    }
    );
}

MAIN
