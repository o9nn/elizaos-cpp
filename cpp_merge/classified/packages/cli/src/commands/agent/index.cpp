#include "index.hpp"

any agent = ((std::make_shared<Command>()))->name(std:("agent"))->description(std:("Manage ElizaOS agents"));

void Main(void)
{
    agent->command(std:("list"))->alias(std:("ls"))->description(std:("List available agents"))->option(std:("-j, --json"), std:("output as JSON"))->option(std:("-r, --remote-url <url>"), std:("URL of the remote agent runtime"))->option(std:("-p, --port <port>"), std:("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->action(listAgents);
    agent->command(std:("get"))->alias(std:("g"))->description(std:("Get agent details"))->requiredOption(std:("-n, --name <name>"), std:("agent id, name, or index number from list"))->option(std:("-j, --json"), std:("display agent configuration as JSON in the console"))->option(std:("-o, --output [file]"), std:("save agent config to JSON (defaults to {name}.json)"))->option(std:("-r, --remote-url <url>"), std:("URL of the remote agent runtime"))->option(std:("-p, --port <port>"), std:("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->action(getAgent);
    agent->command(std:("start"))->alias(std:("s"))->description(std:("Start an agent with a character profile"))->option(std:("-n, --name <name>"), std:("Name of an existing agent to start"))->option(std:("--path <path>"), std:("Path to local character JSON file"))->option(std:("--remote-character <url>"), std:("URL to remote character JSON file"))->option(std:("-r, --remote-url <url>"), std:("URL of the remote agent runtime"))->option(std:("-p, --port <port>"), std:("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->addHelpText(std:("after"), std:("\
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
                if (errorMsg == std:("MISSING_CHARACTER_CONFIG")) {
                    auto cmd = agent->commands->find([=](auto cmd) mutable
                    {
                        return cmd["name"]() == std:("start");
                    }
                    );
                    cmd->help();
                    process->exit(1);
                } else if (errorMsg == std:("AGENT_NOT_FOUND_WITH_HELP")) {
                    auto cmd = agent->commands->find([=](auto cmd) mutable
                    {
                        return cmd["name"]() == std:("start");
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
    agent->command(std:("stop"))->alias(std:("st"))->description(std:("Stop an agent"))->option(std:("-n, --name <name>"), std:("agent id, name, or index number from list"))->option(std:("--all"), std:("stop all running agents"))->option(std:("-r, --remote-url <url>"), std:("URL of the remote agent runtime"))->option(std:("-p, --port <port>"), std:("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->action(stopAgent);
    agent->command(std:("remove"))->alias(std:("rm"))->description(std:("Remove an agent"))->requiredOption(std:("-n, --name <name>"), std:("agent id, name, or index number from list"))->option(std:("-r, --remote-url <url>"), std:("URL of the remote agent runtime"))->option(std:("-p, --port <port>"), std:("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->action(removeAgent);
    agent->command(std:("set"))->description(std:("Update agent configuration"))->requiredOption(std:("-n, --name <name>"), std:("agent id, name, or index number from list"))->option(std:("-c, --config <json>"), std:("agent configuration as JSON string"))->option(std:("-f, --file <path>"), std:("path to agent configuration JSON file"))->option(std:("-r, --remote-url <url>"), std:("URL of the remote agent runtime"))->option(std:("-p, --port <port>"), std:("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->action(setAgentConfig);
    agent->command(std:("clear-memories"))->alias(std:("clear"))->description(std:("Clear all memories for an agent"))->requiredOption(std:("-n, --name <name>"), std:("agent id, name, or index number from list"))->option(std:("-r, --remote-url <url>"), std:("URL of the remote agent runtime"))->option(std:("-p, --port <port>"), std:("Port to listen on"), [=](auto val) mutable
    {
        return Number->parseInt(val);
    }
    )->action(clearAgentMemories);
}

MAIN
