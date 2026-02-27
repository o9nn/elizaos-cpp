#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/agent/index.h"

any agent = ((std::make_shared<Command>()))->name(std::string("agent"))->description(std::string("Manage ElizaOS agents"));

void Main(void)
{
    agent->command(std::string("list"))->alias(std::string("ls"))->description(std::string("List available agents"))->option(std::string("-j, --json"), std::string("output as JSON"))->option(std::string("-r, --remote-url <url>"), std::string("URL of the remote agent runtime"))->option(std::string("-p, --port <port>"), std::string("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->action(listAgents);
    agent->command(std::string("get"))->alias(std::string("g"))->description(std::string("Get agent details"))->requiredOption(std::string("-n, --name <name>"), std::string("agent id, name, or index number from list"))->option(std::string("-j, --json"), std::string("display agent configuration as JSON in the console"))->option(std::string("-o, --output [file]"), std::string("save agent config to JSON (defaults to {name}.json)"))->option(std::string("-r, --remote-url <url>"), std::string("URL of the remote agent runtime"))->option(std::string("-p, --port <port>"), std::string("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->action(getAgent);
    agent->command(std::string("start"))->alias(std::string("s"))->description(std::string("Start an agent with a character profile"))->option(std::string("-n, --name <name>"), std::string("Name of an existing agent to start"))->option(std::string("--path <path>"), std::string("Path to local character JSON file"))->option(std::string("--remote-character <url>"), std::string("URL to remote character JSON file"))->option(std::string("-r, --remote-url <url>"), std::string("URL of the remote agent runtime"))->option(std::string("-p, --port <port>"), std::string("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->addHelpText(std::string("after"), std::string("\
Examples:\
  $ elizaos agent start -n "Agent Name"     Start an existing agent by name\
  $ elizaos agent start --path ./char.json  Start with a local character file\
  $ elizaos agent start --remote-character https://example.com/char.json\
\
To create a new agent:\
  $ elizaos create -t agent my-agent-name   Create a new agent using Eliza template\
\
Required configuration:\
  You must provide one of these options: --name, --path, or --remote-character\
"))->action([=](auto options) mutable
    {
        try
        {
            std::async([=]() { startAgent(options); });
        }
        catch (const any& error)
        {
            if (is<Error>(error)) {
                auto errorMsg = error->message;
                if (errorMsg == std::string("MISSING_CHARACTER_CONFIG")) {
                    auto cmd = agent->commands->find([=](auto cmd) mutable
                    {
                        return cmd["name"]() == std::string("start");
                    }
                    );
                    cmd->help();
                    process->exit(1);
                } else if (errorMsg == std::string("AGENT_NOT_FOUND_WITH_HELP")) {
                    auto cmd = agent->commands->find([=](auto cmd) mutable
                    {
                        return cmd["name"]() == std::string("start");
                    }
                    );
                    cmd->help();
                    process->exit(1);
                }
            }
            throw any(error);
        }
    }
    );
    agent->command(std::string("stop"))->alias(std::string("st"))->description(std::string("Stop an agent"))->option(std::string("-n, --name <name>"), std::string("agent id, name, or index number from list"))->option(std::string("--all"), std::string("stop all running agents"))->option(std::string("-r, --remote-url <url>"), std::string("URL of the remote agent runtime"))->option(std::string("-p, --port <port>"), std::string("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->action(stopAgent);
    agent->command(std::string("remove"))->alias(std::string("rm"))->description(std::string("Remove an agent"))->requiredOption(std::string("-n, --name <name>"), std::string("agent id, name, or index number from list"))->option(std::string("-r, --remote-url <url>"), std::string("URL of the remote agent runtime"))->option(std::string("-p, --port <port>"), std::string("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->action(removeAgent);
    agent->command(std::string("set"))->description(std::string("Update agent configuration"))->requiredOption(std::string("-n, --name <name>"), std::string("agent id, name, or index number from list"))->option(std::string("-c, --config <json>"), std::string("agent configuration as JSON string"))->option(std::string("-f, --file <path>"), std::string("path to agent configuration JSON file"))->option(std::string("-r, --remote-url <url>"), std::string("URL of the remote agent runtime"))->option(std::string("-p, --port <port>"), std::string("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->action(setAgentConfig);
    agent->command(std::string("clear-memories"))->alias(std::string("clear"))->description(std::string("Clear all memories for an agent"))->requiredOption(std::string("-n, --name <name>"), std::string("agent id, name, or index number from list"))->option(std::string("-r, --remote-url <url>"), std::string("URL of the remote agent runtime"))->option(std::string("-p, --port <port>"), std::string("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->action(clearAgentMemories);
}

MAIN
