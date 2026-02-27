#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-shell/src/index.h"

std::shared_ptr<Plugin> shellPlugin = object{
    object::pair{std::string("name"), std::string("plugin-shell")}, 
    object::pair{std::string("description"), std::string("Provides shell access to the agent, allowing it to run commands and view history.")}, 
    object::pair{std::string("actions"), array<any>{ runShellCommandAction, clearShellHistoryAction }}, 
    object::pair{std::string("providers"), array<any>{ shellProvider }}, 
    object::pair{std::string("services"), array<ShellService>{ ShellService }}
};

void Main(void)
{
}

MAIN
