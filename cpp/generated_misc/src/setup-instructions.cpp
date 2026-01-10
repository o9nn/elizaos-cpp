#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/monorepo/utils/setup-instructions.h"

std::shared_ptr<PlatformInstructions> getBunInstallInstructions()
{
    auto platform = process->platform;
    if (platform == std::string("win32")) {
        return object{
            object::pair{std::string("platform"), std::string("Windows")}, 
            object::pair{std::string("commands"), array<string>{ std::string("powershell -c "irm bun.sh/install.ps1 | iex"") }}, 
            object::pair{std::string("alternatives"), array<string>{ std::string("scoop install bun (if you have Scoop)") }}
        };
    } else {
        auto commands = array<string>{ std::string("curl -fsSL https://bun.sh/install | bash") };
        auto alternatives = array<string>();
        if (platform == std::string("darwin")) {
            alternatives->push(std::string("brew install bun (if you have Homebrew)"));
        }
        return object{
            object::pair{std::string("platform"), (platform == std::string("darwin")) ? std::string("macOS") : std::string("Linux")}, 
            object::pair{std::string("commands"), std::string("commands")}, 
            object::pair{std::string("alternatives"), (alternatives->get_length() > 0) ? any(alternatives) : any(undefined)}
        };
    }
};


void displayBasicSteps(string cdPath)
{
    console->log(std::string("\
To complete the ElizaOS setup, follow these steps:\
"));
    console->log(std::string("1. Navigate to the project directory:"));
    console->log(std::string("   cd ") + cdPath + string_empty);
    console->log(std::string("\
2. Install dependencies:"));
    console->log(std::string("   bun install"));
    console->log(std::string("\
3. Build the project:"));
    console->log(std::string("   bun run build"));
    console->log(std::string("\
4. Start ElizaOS:"));
    console->log(std::string("   bun run start or bun run dev"));
};


void displayPrerequisites()
{
    console->log(std::string("\
") + emoji->list(std::string("Prerequisites:")) + string_empty);
    console->log(std::string("   ") + emoji->bullet(std::string("Node.js 23.3.0+")) + string_empty);
    console->log(std::string("   ") + emoji->bullet(std::string("Bun (JavaScript runtime & package manager)")) + string_empty);
};


void displayBunInstructions()
{
    console->log(std::string("\
") + emoji->rocket(std::string("If you don't have Bun installed:")) + string_empty);
    auto instructions = getBunInstallInstructions();
    instructions->commands->forEach([=](auto command) mutable
    {
        console->log(std::string("   ") + command + string_empty);
    }
    );
    if (instructions->alternatives) {
        instructions->alternatives->forEach([=](auto alt) mutable
        {
            console->log(std::string("   Alternative: ") + alt + string_empty);
        }
        );
    }
    console->log(std::string("   More options: https://bun.sh/docs/installation"));
    console->log(std::string("   After installation, restart your terminal"));
};


void displayNextSteps(string targetDir)
{
    auto cdPath = path->relative(process->cwd(), targetDir);
    displayBasicSteps(cdPath);
    displayPrerequisites();
    displayBunInstructions();
};


