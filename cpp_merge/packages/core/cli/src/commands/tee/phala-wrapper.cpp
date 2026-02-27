#include "phala-wrapper.h"

any phalaCliCommand = ((std::make_shared<Command>(std:("phala"))))->description(std:("Official Phala Cloud CLI - Manage TEE deployments on Phala Cloud"))->allowUnknownOption()->helpOption(false)->action([=](auto _, auto command) mutable
{
    shared args = command["args"];
    try
    {
        elizaLogger->info(std:("Running Phala CLI command:"), (array<string>{ std:("phala"), args })->join(std:(" ")));
        auto phalaProcess = spawn(std:("npx"), array<string>{ std:("--yes"), std:("phala"), args }, object{
            object::pair{std:("stdio"), std:("inherit")}, 
            object::pair{std:("shell"), true}
        });
        phalaProcess->on(std:("error"), [=](auto error) mutable
        {
            elizaLogger->error(std:("Failed to execute Phala CLI:"), error);
            if (error["message"]["includes"](std:("ENOENT"))) {
                elizaLogger->error(std:("\
") + emoji["error"](std:("Error: npx not found. Please install Node.js and npm:")) + string_empty);
                elizaLogger->error(std:("   Visit https://nodejs.org or use a version manager like nvm"));
                elizaLogger->error(std:("   curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.0/install.sh | bash"));
            } else {
                elizaLogger->error(std:("\
") + emoji["error"](std:("Error: Failed to execute Phala CLI")) + string_empty);
                elizaLogger->error(std:("   Try running directly: npx phala"), args["join"](std:(" ")));
            }
            process->exit(1);
        }
        );
        phalaProcess->on(std:("exit"), [=](auto code) mutable
        {
            if (code != 0) {
                elizaLogger->warn(std:("Phala CLI exited with code: ") + code + string_empty);
            }
            process->exit(OR((code), (0)));
        }
        );
    }
    catch (const any& error)
    {
        elizaLogger->error(std:("Error running Phala CLI:"), error);
        elizaLogger->error(std:("\
") + emoji["error"](std:("Error: Failed to run Phala CLI")) + string_empty);
        elizaLogger->error(std:("   Try running Phala CLI directly with: npx phala"), args["join"](std:(" ")));
        elizaLogger->error(std:("   Or visit https://www.npmjs.com/package/phala for more information"));
        process->exit(1);
    }
}
)->configureHelp(object{
    object::pair{std:("helpWidth"), 100}
})->on(std:("--help"), [=]() mutable
{
    console->log(string_empty);
    console->log(std:("This command wraps the official Phala Cloud CLI."));
    console->log(std:("The Phala CLI will be automatically downloaded if not already installed."));
    console->log(std:("All arguments are passed directly to the Phala CLI."));
    console->log(string_empty);
    console->log(std:("Examples:"));
    console->log(std:("  $ elizaos tee phala help"));
    console->log(std:("  $ elizaos tee phala auth login <api-key>"));
    console->log(std:("  $ elizaos tee phala cvms list"));
    console->log(std:("  $ elizaos tee phala cvms create --name my-app --compose ./docker-compose.yml"));
    console->log(string_empty);
    console->log(std:("For full Phala CLI documentation, run:"));
    console->log(std:("  $ npx phala help"));
}
);

void Main(void)
{
}

MAIN
