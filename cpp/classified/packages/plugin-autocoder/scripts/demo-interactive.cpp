#include "demo-interactive.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> runDemo() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🎬 Interactive Claude Code Test Demo" << std::endl;
    std::cout << "========================\n" << std::endl;

    // Check environment
    const auto requiredKeys = ["ANTHROPIC_API_KEY"];
    const auto missingKeys = requiredKeys.filter((key) => !process.env[key]);

    if (missingKeys.length > 0) {
        std::cerr << "❌ Missing required environment variables for demo:" << std::endl;
        missingKeys.forEach((key) => console.error(`   - ${key}`));
        std::cout << "\nTo run the demo:" << std::endl;
        std::cout << "1. Set your Anthropic API key:" << std::endl;
        std::cout << "   export ANTHROPIC_API_KEY="your_api_key_here"" << std::endl;
        std::cout << "2. Run the demo:" << std::endl;
        std::cout << "   bun run scripts/demo-interactive.ts" << std::endl;
        std::cout << "\nTo run the interactive test:" << std::endl;
        std::cout << "   bun run test:interactive" << std::endl;
        std::cout << "   # or" << std::endl;
        std::cout << "   ./scripts/test-interactive.sh" << std::endl;
        return;
    }

    std::cout << "✅ Environment is ready for demo!\n" << std::endl;

    std::cout << "🌟 Features of the Interactive Test:" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "1. 🧠 Direct Claude Code Integration" << std::endl;
    std::cout << "   - Send prompts directly to Claude Code SDK" << std::endl;
    std::cout << "   - Real-time response streaming" << std::endl;
    std::cout << "   - Example: claude Create a TypeScript function" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "2. 🚀 Complete Project Generation" << std::endl;
    std::cout << "   - Generate full ElizaOS plugins and agents" << std::endl;
    std::cout << "   - Automatic quality assurance workflow" << std::endl;
    std::cout << "   - Example: generate A weather plugin for OpenWeatherMap" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "3. 🏗️  Live Sandbox Environment" << std::endl;
    std::cout << "   - Real E2B sandbox (if API key provided)" << std::endl;
    std::cout << "   - Mock sandbox for testing without E2B" << std::endl;
    std::cout << "   - File operations and command execution" << std::endl;
    std::cout << "   - Example: run npm install" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "4. 📁 File Operations" << std::endl;
    std::cout << "   - Write files to sandbox" << std::endl;
    std::cout << "   - Read files from sandbox" << std::endl;
    std::cout << "   - List directory contents" << std::endl;
    std::cout << "   - Example: write src/index.ts std::cout << "hello" << std::endl" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "📚 Available Commands:" << std::endl;
    std::cout << "   help                     - Show help message" << std::endl;
    std::cout << "   status                   - Show session status" << std::endl;
    std::cout << "   claude <prompt>          - Send direct prompt to Claude Code" << std::endl;
    std::cout << "   generate <description>   - Generate complete project" << std::endl;
    std::cout << "   run <command>            - Run command in sandbox" << std::endl;
    std::cout << "   write <file> <content>   - Write file to sandbox" << std::endl;
    std::cout << "   read <file>              - Read file from sandbox" << std::endl;
    std::cout << "   ls [path]                - List files in sandbox" << std::endl;
    std::cout << "   clear                    - Clear terminal" << std::endl;
    std::cout << "   exit                     - Exit test session" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "🎯 Example Session Flow:" << std::endl;
    std::cout << "1. Start: bun run test:interactive" << std::endl;
    std::cout << "2. Generate: generate A calculator plugin" << std::endl;
    std::cout << "3. Check: ls src/" << std::endl;
    std::cout << "4. Read: read src/index.ts" << std::endl;
    std::cout << "5. Test: run npm test" << std::endl;
    std::cout << "6. Exit: exit" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "🚀 Ready to start? Run one of these commands:" << std::endl;
    std::cout << "   bun run test:interactive" << std::endl;
    std::cout << "   ./scripts/test-interactive.sh" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "💡 Pro Tips:" << std::endl;
    std::cout << "   - Use Ctrl+C to exit gracefully" << std::endl;
    std::cout << "   - The sandbox persists during the session" << std::endl;
    std::cout << "   - Generated projects are saved to the sandbox" << std::endl;
    std::cout << "   - All Claude Code responses are logged" << std::endl;
    std::cout << "   - Mock services are used when APIs are not available" << std::endl;
    std::cout << "" << std::endl;

}

} // namespace elizaos
