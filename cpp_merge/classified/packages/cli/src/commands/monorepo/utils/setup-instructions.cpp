#include "setup-instructions.h"

std::shared_ptr<PlatformInstructions> getBunInstallInstructions()
{
    auto platform = process->platform;
    if (platform == std:("win32")) {
        return object{
            object::pair{std:("platform"), std:("Windows")}, 
            object::pair{std:("commands"), array<string>{ std:("powershell -c "irm bun.sh/install.ps1 | iex"") }}, 
            object::pair{std:("alternatives"), array<string>{ std:("scoop install bun (if you have Scoop)") }}
        };
    } else {
        auto commands = array<string>{ std:("curl -fsSL https://bun.sh/install | bash") };
        auto alternatives = array<string>();
        if (platform == std:("darwin")) {
            alternatives->push(std:("brew install bun (if you have Homebrew)"));
        }
        return object{
            object::pair{std:("platform"), (platform == std:("darwin")) ? std:("macOS") : std:("Linux")}, 
            object::pair{std:("commands"), std:("commands")}, 
            object::pair{std:("alternatives"), (alternatives->get_length() > 0) ? any(alternatives) (undefined)}
        };
    }
};


void displayBasicSteps(string cdPath)
{
    console->log(std:("\
To complete the ElizaOS setup, follow these steps:\
"));
    console->log(std:("1. Navigate to the project directory:"));
    console->log(std:("   cd ") + cdPath + string_empty);
    console->log(std:("\
2. Install dependencies:"));
    console->log(std:("   bun install"));
    console->log(std:("\
3. Build the project:"));
    console->log(std:("   bun run build"));
    console->log(std:("\
4. Start ElizaOS:"));
    console->log(std:("   bun run start or bun run dev"));
};


void displayPrerequisites()
{
    console->log(std:("\
") + emoji->list(std:("Prerequisites:")) + string_empty);
    console->log(std:("   ") + emoji->bullet(std:("Node.js 23.3.0+")) + string_empty);
    console->log(std:("   ") + emoji->bullet(std:("Bun (JavaScript runtime & package manager)")) + string_empty);
};


void displayBunInstructions()
{
    console->log(std:("\
") + emoji->rocket(std:("If you don't have Bun installed:")) + string_empty);
    auto instructions = getBunInstallInstructions();
    instructions->commands->forEach([=](auto command) mutable
    {
        console->log(std:("   ") + command + string_empty);
    }
    );
    if (instructions->alternatives) {
        instructions->alternatives->forEach([=](auto alt) mutable
        {
            console->log(std:("   Alternative: ") + alt + string_empty);
        }
        );
    }
    console->log(std:("   More options: https://bun.sh/docs/installation"));
    console->log(std:("   After installation, restart your terminal"));
};


void displayNextSteps(string targetDir)
{
    auto cdPath = path->relative(process->cwd(), targetDir);
    displayBasicSteps(cdPath);
    displayPrerequisites();
    displayBunInstructions();
};


