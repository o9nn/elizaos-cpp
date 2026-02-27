#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/scripts/demo-interactive.h"

void runDemo()
{
    console->log(std:("🎬 Interactive Claude Code Test Demo"));
    console->log(std:("===========\
"));
    auto requiredKeys = array<string>{ std:("ANTHROPIC_API_KEY") };
    auto missingKeys = requiredKeys->filter([=](auto key) mutable
    {
        return !const_(process->env)[key];
    }
    );
    if (missingKeys->get_length() > 0) {
        console->error(std:("❌ Missing required environment variables for demo:"));
        missingKeys->forEach([=](auto key) mutable
        {
            return console->error(std:("   - ") + key + string_empty);
        }
        );
        console->log(std:("\
To run the demo:"));
        console->log(std:("1. Set your Anthropic API key:"));
        console->log(std:("   ANTHROPIC_API_KEY="your_api_key_here""));
        console->log(std:("2. Run the demo:"));
        console->log(std:("   bun run scripts/demo-interactive.ts"));
        console->log(std:("\
To run the interactive test:"));
        console->log(std:("   bun run test:interactive"));
        console->log(std:("   # or"));
        console->log(std:("   ./scripts/test-interactive.sh"));
        return std::shared_ptr<Promise<void>>();
    }
    console->log(std:("✅ Environment is ready for demo!\
"));
    console->log(std:("🌟 Features of the Interactive Test:"));
    console->log(string_empty);
    console->log(std:("1. 🧠 Direct Claude Code Integration"));
    console->log(std:("   - Send prompts directly to Claude Code SDK"));
    console->log(std:("   - Real-time response streaming"));
    console->log(std:("   - Example: claude Create a TypeScript function"));
    console->log(string_empty);
    console->log(std:("2. 🚀 Complete Project Generation"));
    console->log(std:("   - Generate full ElizaOS plugins and agents"));
    console->log(std:("   - Automatic quality assurance workflow"));
    console->log(std:("   - Example: generate A weather plugin for OpenWeatherMap"));
    console->log(string_empty);
    console->log(std:("3. 🏗️  Live Sandbox Environment"));
    console->log(std:("   - Real E2B sandbox (if API key provided)"));
    console->log(std:("   - Mock sandbox for testing without E2B"));
    console->log(std:("   - File operations and command execution"));
    console->log(std:("   - Example: run npm install"));
    console->log(string_empty);
    console->log(std:("4. 📁 File Operations"));
    console->log(std:("   - Write files to sandbox"));
    console->log(std:("   - Read files from sandbox"));
    console->log(std:("   - List directory contents"));
    console->log(std:("   - Example: write src/index.ts std::cout << "hello" << std::endl"));
    console->log(string_empty);
    console->log(std:("📚 Available Commands:"));
    console->log(std:("   help                     - Show help message"));
    console->log(std:("   status                   - Show session status"));
    console->log(std:("   claude <prompt>          - Send direct prompt to Claude Code"));
    console->log(std:("   generate <description>   - Generate complete project"));
    console->log(std:("   run <command>            - Run command in sandbox"));
    console->log(std:("   write <file> <content>   - Write file to sandbox"));
    console->log(std:("   read <file>              - Read file from sandbox"));
    console->log(std:("   ls [path]                - List files in sandbox"));
    console->log(std:("   clear                    - Clear terminal"));
    console->log(std:("   exit                     - Exit test session"));
    console->log(string_empty);
    console->log(std:("🎯 Example Session Flow:"));
    console->log(std:("1. Start: bun run test:interactive"));
    console->log(std:("2. Generate: generate A calculator plugin"));
    console->log(std:("3. Check: ls src/"));
    console->log(std:("4. Read: read src/index.ts"));
    console->log(std:("5. Test: run npm test"));
    console->log(std:("6. Exit: exit"));
    console->log(string_empty);
    console->log(std:("🚀 Ready to start? Run one of these commands:"));
    console->log(std:("   bun run test:interactive"));
    console->log(std:("   ./scripts/test-interactive.sh"));
    console->log(string_empty);
    console->log(std:("💡 Pro Tips:"));
    console->log(std:("   - Use Ctrl+C to exit gracefully"));
    console->log(std:("   - The sandbox persists during the session"));
    console->log(std:("   - Generated projects are saved to the sandbox"));
    console->log(std:("   - All Claude Code responses are logged"));
    console->log(std:("   - Mock services are used when APIs are not available"));
    console->log(string_empty);
};



void Main(void)
{
    runDemo()->_catch(console->error);
}

MAIN
