#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/scripts/test-sandbox-generation.h"

array<string> requiredKeys = array<string>{ std::string("ANTHROPIC_API_KEY"), std::string("E2B_API_KEY") };
array<string> missingKeys = requiredKeys->filter([=](auto key) mutable
{
    return !const_(process->env)[key];
}
);

void Main(void)
{
    if (missingKeys->get_length() > 0) {
        console->error(std::string("❌ Missing required environment variables:"), missingKeys->join(std::string(", ")));
        console->log(std::string("\
Usage:"));
        console->log(std::string("  ANTHROPIC_API_KEY=your_key E2B_API_KEY=your_key bun run scripts/test-sandbox-generation.ts"));
        process->exit(1);
    }
    console->log(std::string("🔑 API Keys configured:"));
    console->log(std::string("  ANTHROPIC_API_KEY: ") + process->env->ANTHROPIC_API_KEY->substring(0, 10) + std::string("..."));
    console->log(std::string("  E2B_API_KEY: ") + process->env->E2B_API_KEY->substring(0, 10) + std::string("..."));
    console->log(string_empty);
    testSandboxGeneration()->then([=]() mutable
    {
        console->log(std::string("\
✅ Sandbox generation test completed successfully!"));
        process->exit(0);
    }
    )->_catch([=](auto error) mutable
    {
        console->error(std::string("\
❌ Sandbox generation test failed:"), error);
        process->exit(1);
    }
    );
}

MAIN
