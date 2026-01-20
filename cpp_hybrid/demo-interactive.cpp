#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/scripts/demo-interactive.h"

void runDemo()
{
    console->log(std::string("🎬 Interactive Claude Code Test Demo"));
    console->log(std::string("===================================\
"));
    auto requiredKeys = array<string>{ std::string("ANTHROPIC_API_KEY") };
    auto missingKeys = requiredKeys->filter([=](auto key) mutable
    {
        return !const_(process->env)[key];
    }
    );
    if (missingKeys->get_length() > 0) {
        console->error(std::string("❌ Missing required environment variables for demo:"));
        missingKeys->forEach([=](auto key) mutable
        {
            return console->error(std::string("   - ") + key + string_empty);
        }
        );
        console->log(std::string("\
To run the demo:"));
        console->log(std::string("1. Set your Anthropic API key:"));
        console->log(std::string("   export ANTHROPIC_API_KEY="your_api_key_here""));
        console->log(std::string("2. Run the demo:"));
        console->log(std::string("   bun run scripts/demo-interactive.ts"));
        console->log(std::string("\
To run the interactive test:"));
        console->log(std::string("   bun run test:interactive"));
        console->log(std::string("   # or"));
        console->log(std::string("   ./scripts/test-interactive.sh"));
        return std::shared_ptr<Promise<void>>();
    }
    console->log(std::string("✅ Environment is ready for demo!\
"));
    console->log(std::string("🌟 Features of the Interactive Test:"));
    console->log(string_empty);
    console->log(std::string("1. 🧠 Direct Claude Code Integration"));
    console->log(std::string("   - Send prompts directly to Claude Code SDK"));
    console->log(std::string("   - Real-time response streaming"));
    console->log(std::string("   - Example: claude Create a TypeScript function"));
    console->log(string_empty);
    console->log(std::string("2. 🚀 Complete Project Generation"));
    console->log(std::string("   - Generate full ElizaOS plugins and agents"));
    console->log(std::string("   - Automatic quality assurance workflow"));
    console->log(std::string("   - Example: generate A weather plugin for OpenWeatherMap"));
    console->log(string_empty);
    console->log(std::string("3. 🏗️  Live Sandbox Environment"));
    console->log(std::string("   - Real E2B sandbox (if API key provided)"));
    console->log(std::string("   - Mock sandbox for testing without E2B"));
    console->log(std::string("   - File operations and command execution"));
    console->log(std::string("   - Example: run npm install"));
    console->log(string_empty);
    console->log(std::string("4. 📁 File Operations"));
    console->log(std::string("   - Write files to sandbox"));
    console->log(std::string("   - Read files from sandbox"));
    console->log(std::string("   - List directory contents"));
    console->log(std::string("   - Example: write src/index.ts console.log("hello")"));
    console->log(string_empty);
    console->log(std::string("📚 Available Commands:"));
    console->log(std::string("   help                     - Show help message"));
    console->log(std::string("   status                   - Show session status"));
    console->log(std::string("   claude <prompt>          - Send direct prompt to Claude Code"));
    console->log(std::string("   generate <description>   - Generate complete project"));
    console->log(std::string("   run <command>            - Run command in sandbox"));
    console->log(std::string("   write <file> <content>   - Write file to sandbox"));
    console->log(std::string("   read <file>              - Read file from sandbox"));
    console->log(std::string("   ls [path]                - List files in sandbox"));
    console->log(std::string("   clear                    - Clear terminal"));
    console->log(std::string("   exit                     - Exit test session"));
    console->log(string_empty);
    console->log(std::string("🎯 Example Session Flow:"));
    console->log(std::string("1. Start: bun run test:interactive"));
    console->log(std::string("2. Generate: generate A calculator plugin"));
    console->log(std::string("3. Check: ls src/"));
    console->log(std::string("4. Read: read src/index.ts"));
    console->log(std::string("5. Test: run npm test"));
    console->log(std::string("6. Exit: exit"));
    console->log(string_empty);
    console->log(std::string("🚀 Ready to start? Run one of these commands:"));
    console->log(std::string("   bun run test:interactive"));
    console->log(std::string("   ./scripts/test-interactive.sh"));
    console->log(string_empty);
    console->log(std::string("💡 Pro Tips:"));
    console->log(std::string("   - Use Ctrl+C to exit gracefully"));
    console->log(std::string("   - The sandbox persists during the session"));
    console->log(std::string("   - Generated projects are saved to the sandbox"));
    console->log(std::string("   - All Claude Code responses are logged"));
    console->log(std::string("   - Mock services are used when APIs are not available"));
    console->log(string_empty);
};



void Main(void)
{
    runDemo()->_catch(console->error);
}

MAIN
