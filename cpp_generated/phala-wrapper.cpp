#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/tee/phala-wrapper.h"

any phalaCliCommand = ((std::make_shared<Command>(std::string("phala"))))->description(std::string("Official Phala Cloud CLI - Manage TEE deployments on Phala Cloud"))->allowUnknownOption()->helpOption(false)->action([=](auto _, auto command) mutable
{
    shared args = command["args"];
    try
    {
        elizaLogger->info(std::string("Running Phala CLI command:"), (array<string>{ std::string("phala"), args })->join(std::string(" ")));
        auto phalaProcess = spawn(std::string("npx"), array<string>{ std::string("--yes"), std::string("phala"), args }, object{
            object::pair{std::string("stdio"), std::string("inherit")}, 
            object::pair{std::string("shell"), true}
        });
        phalaProcess->on(std::string("error"), [=](auto error) mutable
        {
            elizaLogger->error(std::string("Failed to execute Phala CLI:"), error);
            if (error["message"]["includes"](std::string("ENOENT"))) {
                elizaLogger->error(std::string("\
") + emoji["error"](std::string("Error: npx not found. Please install Node.js and npm:")) + string_empty);
                elizaLogger->error(std::string("   Visit https://nodejs.org or use a version manager like nvm"));
                elizaLogger->error(std::string("   curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.0/install.sh | bash"));
            } else {
                elizaLogger->error(std::string("\
") + emoji["error"](std::string("Error: Failed to execute Phala CLI")) + string_empty);
                elizaLogger->error(std::string("   Try running directly: npx phala"), args["join"](std::string(" ")));
            }
            process->exit(1);
        }
        );
        phalaProcess->on(std::string("exit"), [=](auto code) mutable
        {
            if (code != 0) {
                elizaLogger->warn(std::string("Phala CLI exited with code: ") + code + string_empty);
            }
            process->exit(OR((code), (0)));
        }
        );
    }
    catch (const any& error)
    {
        elizaLogger->error(std::string("Error running Phala CLI:"), error);
        elizaLogger->error(std::string("\
") + emoji["error"](std::string("Error: Failed to run Phala CLI")) + string_empty);
        elizaLogger->error(std::string("   Try running Phala CLI directly with: npx phala"), args["join"](std::string(" ")));
        elizaLogger->error(std::string("   Or visit https://www.npmjs.com/package/phala for more information"));
        process->exit(1);
    }
}
)->configureHelp(object{
    object::pair{std::string("helpWidth"), 100}
})->on(std::string("--help"), [=]() mutable
{
    console->log(string_empty);
    console->log(std::string("This command wraps the official Phala Cloud CLI."));
    console->log(std::string("The Phala CLI will be automatically downloaded if not already installed."));
    console->log(std::string("All arguments are passed directly to the Phala CLI."));
    console->log(string_empty);
    console->log(std::string("Examples:"));
    console->log(std::string("  $ elizaos tee phala help"));
    console->log(std::string("  $ elizaos tee phala auth login <api-key>"));
    console->log(std::string("  $ elizaos tee phala cvms list"));
    console->log(std::string("  $ elizaos tee phala cvms create --name my-app --compose ./docker-compose.yml"));
    console->log(string_empty);
    console->log(std::string("For full Phala CLI documentation, run:"));
    console->log(std::string("  $ npx phala help"));
}
);

void Main(void)
{
}

MAIN
