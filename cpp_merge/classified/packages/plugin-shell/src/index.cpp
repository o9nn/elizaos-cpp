#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-shell/src/index.h"

std::shared_ptr<Plugin> shellPlugin = object{
    object::pair{std:("name"), std:("plugin-shell")}, 
    object::pair{std:("description"), std:("Provides shell access to the agent, allowing it to run commands and view history.")}, 
    object::pair{std:("actions"), array<any>{ runShellCommandAction, clearShellHistoryAction }}, 
    object::pair{std:("providers"), array<any>{ shellProvider }}, 
    object::pair{std:("services"), array<ShellService>{ ShellService }}
};

void Main(void)
{
}

MAIN
