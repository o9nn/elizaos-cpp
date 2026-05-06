#include "index.hpp"
#include <string>

any env = ((std::make_shared<Command>()))->name(std::string("env"))->description(std::string("Manage environment variables and secrets"));

void Main(void)
{
    env->command(std::string("list"))->description(std::string("List all environment variables"))->option(std::string("--system"), std::string("List only system information"))->option(std::string("--local"), std::string("List only local environment variables"))->action([=](auto options) mutable
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
    env->command(std::string("edit-local"))->description(std::string("Edit local environment variables"))->option(std::string("-y, --yes"), std::string("Automatically confirm prompts"))->action([=](auto options) mutable
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
    env->command(std::string("reset"))->description(std::string("Reset environment variables and clean up database/cache files (interactive selection)"))->option(std::string("-y, --yes"), std::string("Automatically reset using default selections"))->action([=](auto options) mutable
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
    env->command(std::string("interactive"))->description(std::string("Interactive environment variable management"))->option(std::string("-y, --yes"), std::string("Automatically confirm prompts"))->action([=](auto options) mutable
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
        console->log(colors->bold(std::string("\
Eliza Environment Variable Manager")));
        console->log(std::string("\
Available commands:"));
        console->log(std::string("  list                  List all environment variables"));
        console->log(std::string("  edit-local            Edit local environment variables"));
        console->log(std::string("  reset                 Reset environment variables and clean up database/cache files (interactive selection)"));
        console->log(std::string("  interactive           Start interactive environment variable manager"));
        console->log(std::string("\
You can also edit environment variables in the web UI:"));
        console->log(std::string("  http://localhost:3000/settings"));
    }
    );
}

MAIN
