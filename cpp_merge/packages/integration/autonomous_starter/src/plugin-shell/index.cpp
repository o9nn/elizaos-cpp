#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-shell/index.h"

std::shared_ptr<Plugin> shellPlugin = object{
    object::pair{std:("name"), std:("plugin-shell")}, 
    object::pair{std:("description"), std:("Provides shell access to the agent, allowing it to run commands and view history.")}, 
    object::pair{std:("actions"), array<any>{ runShellCommandAction, clearShellHistoryAction, killAutonomousAction }}, 
    object::pair{std:("providers"), array<any>{ shellProvider }}, 
    object::pair{std:("services"), array<ShellService>{ ShellService }}, 
    object::pair{std:("init"), [=](auto config, auto runtime) mutable
    {
    }
    }
};

void Main(void)
{
}

MAIN
