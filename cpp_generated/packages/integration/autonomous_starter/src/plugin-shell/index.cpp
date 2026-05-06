#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-shell/index.h"

std::shared_ptr<Plugin> shellPlugin = object{
    object::pair{std::string("name"), std::string("plugin-shell")}, 
    object::pair{std::string("description"), std::string("Provides shell access to the agent, allowing it to run commands and view history.")}, 
    object::pair{std::string("actions"), array<any>{ runShellCommandAction, clearShellHistoryAction, killAutonomousAction }}, 
    object::pair{std::string("providers"), array<any>{ shellProvider }}, 
    object::pair{std::string("services"), array<ShellService>{ ShellService }}, 
    object::pair{std::string("init"), [=](auto config, auto runtime) mutable
    {
    }
    }
};

void Main(void)
{
}

MAIN
