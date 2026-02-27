#include "interactive-test.h"

InteractiveClaudeCodeTester::InteractiveClaudeCodeTester() {
    this->session = object{
        object::pair{std:("sessionId"), uuidv4()}, 
        object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(object{})}
    };
    this->rl = createInterface(object{
        object::pair{std:("input"), process->stdin}, 
        object::pair{std:("output"), process->stdout}
    });
}

void InteractiveClaudeCodeTester::start()
{
    console->log(std:("🚀 Starting Interactive Claude Code Tester"));
    console->log(std:("═")->repeat(50));
    try
    {
        auto runtime = std::make_shared<AgentRuntime>(object{
            object::pair{std:("plugins"), array<any>()}
        });
        auto getValue = [=](auto key) mutable
        {
            if (key == std:("OPENAI_API_KEY")) return process->env->OPENAI_API_KEY;
            if (key == std:("E2B_API_KEY")) return process->env->E2B_API_KEY;
            return string_empty;
        };
        runtime->getSetting = getValue;
        auto setValue = [=](auto key, auto value) mutable
        {
            if (value == nullptr) {
                (as<any>(process->env)).Delete(key);
            } else {
                (as<any>(process->env))[key] = value;
            }
        };
        runtime->setSetting = setValue;
        (as<any>(runtime))["isInitialized"] = true;
        this->session->runtime = runtime;
        auto codeGenService = runtime->getService(std:("code-generation"));
        auto e2bService = runtime->getService(std:("e2b"));
        auto formsService = runtime->getService(std:("forms"));
        if (!codeGenService) {
            console->log(std:("⚠️  Code generation service not available"));
        }
        if (AND((!e2bService), (process->env->E2B_API_KEY))) {
            console->log(std:("⚠️  E2B service not available (but API key is set)"));
        }
        console->log(std:("\
📋 Available Services:"));
        console->log(std:("─")->repeat(30));
        console->log(std:("   ") + (formsService) ? std:("✅") : std:("❌") + std:(" forms"));
        console->log(std:("   ") + (codeGenService) ? std:("✅") : std:("❌") + std:(" code-generation"));
        console->log(std:("   ") + (e2bService) ? std:("✅") : std:("❌") + std:(" e2b"));
        console->log(std:("\
✅ Runtime initialized successfully"));
        this->showMainMenu();
    }
    catch (const any& error)
    {
        console->error(std:("❌ Failed to initialize runtime:"), error);
        process->exit(1);
    }
}

void InteractiveClaudeCodeTester::showMainMenu()
{
    console->log(std:("\
🎯 Main Menu"));
    console->log(std:("─")->repeat(20));
    console->log(std:("1. Generate Project"));
    console->log(std:("2. Run Sandbox Command"));
    console->log(std:("3. Write File"));
    console->log(std:("4. Read File"));
    console->log(std:("5. List Files"));
    console->log(std:("6. Exit"));
    console->log(string_empty);
    this->rl->question(std:("Choose an option (1-6): "), [=](auto answer) mutable
    {
        this->handleMenuChoice(answer->trim());
    }
    );
}

void InteractiveClaudeCodeTester::handleMenuChoice(string choice)
{
    try
    {
        static switch_type __switch3403_4597 = {
            { any(std:("1")), 1 },
            { any(std:("2")), 2 },
            { any(std:("3")), 3 },
            { any(std:("4")), 4 },
            { any(std:("5")), 5 },
            { any(std:("6")), 6 }
        };
        switch (__switch3403_4597[choice])
        {
        case 1:
            this->rl->question(std:("Enter project description: "), [=](auto description) mutable
            {
                this->generateProject(description);
            }
            );
            break;
        case 2:
            this->rl->question(std:("Enter command to run: "), [=](auto command) mutable
            {
                this->runSandboxCommand(command);
            }
            );
            break;
        case 3:
            this->rl->question(std:("Enter filename: "), [=](auto filename) mutable
            {
                this->rl->question(std:("Enter file content: "), [=](auto content) mutable
                {
                    this->writeFile(filename, content);
                }
                );
            }
            );
            break;
        case 4:
            this->rl->question(std:("Enter filename to read: "), [=](auto filename) mutable
            {
                this->readFile(filename);
            }
            );
            break;
        case 5:
            this->rl->question(std:("Enter path to list (default: .): "), [=](auto path) mutable
            {
                this->listFiles(OR((path), (std:("."))));
            }
            );
            break;
        case 6:
            std::async([=]() { this->cleanup(); });
            console->log(std:("👋 Goodbye!"));
            process->exit(0);
            break;
        default:
            console->log(std:("❌ Invalid choice. Please try again."));
            this->showMainMenu();
            break;
        }
    }
    catch (const any& error)
    {
        console->error(std:("❌ Error:"), error);
        this->showMainMenu();
    }
}

void InteractiveClaudeCodeTester::callClaude(string prompt)
{
    console->log(std:("🤖 Calling Claude..."));
    console->log(std:("📝 Prompt: ") + prompt + string_empty);
    try
    {
        console->log(std:("✅ Claude responded (mock response)"));
        this->showMainMenu();
    }
    catch (const any& error)
    {
        console->error(std:("❌ Error calling Claude:"), error);
    }
}

void InteractiveClaudeCodeTester::generateProject(string description)
{
    console->log(std:("\
🚀 Generating project..."));
    console->log(std:("📝 Description: ") + description + string_empty);
    auto startTime = Date->now();
    auto codeGenService = as<std::shared_ptr<CodeGenService>>(as<any>(this->session->runtime->getService(std:("code-generation"))));
    if (!codeGenService) {
        throw any(std::make_shared<Error>(std:("Code generation service not available")));
    }
    auto request = object{
        object::pair{std:("projectName"), std:("generated-project-") + Date->now() + string_empty}, 
        object::pair{std:("description"), std:("description")}, 
        object::pair{std:("requirements"), array<string>{ description }}, 
        object::pair{std:("apis"), array<any>()}, 
        object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
        object::pair{std:("testScenarios"), array<string>{ std:("Basic functionality test") }}
    };
    auto result = std::async([=]() { codeGenService->generateCode(request); });
    auto duration = Date->now() - startTime;
    console->log(std:("\
✅ Project Generated (") + duration + std:("ms):"));
    console->log(std:("─")->repeat(50));
    console->log(std:("📁 Project: ") + request["projectName"] + string_empty);
    console->log(std:("✅ Success: ") + result["success"] + string_empty);
    if (result["projectPath"]) {
        console->log(std:("📂 Path: ") + result["projectPath"] + string_empty);
        this->session->currentProject = object{
            object::pair{std:("id"), OR((result["id"]), (uuidv4()))}, 
            object::pair{std:("name"), request["projectName"]}, 
            object::pair{std:("type"), request["targetType"]}, 
            object::pair{std:("status"), (result["success"]) ? std:("completed") : std:("failed")}, 
            object::pair{std:("path"), result["projectPath"]}
        };
        this->session->projectPath = result["projectPath"];
    }
    this->showMainMenu();
}

void InteractiveClaudeCodeTester::runSandboxCommand(string command)
{
    console->log(std:("\
🔧 Running: ") + command + string_empty);
    auto e2bService = as<std::shared_ptr<E2bService>>(as<any>(this->session->runtime->getService(std:("e2b"))));
    if (!e2bService) {
        console->log(std:("❌ E2B service not available"));
        return std::shared_ptr<Promise<void>>();
    }
    auto result = std::async([=]() { e2bService->executeCode(std:("\
import subprocess\
result = subprocess.run('") + command + std:("'.split(), capture_output=True, text=True)\
print("STDOUT:", result.stdout)\
print("STDERR:", result.stderr)\
print("EXIT_CODE:", result.returncode)\
      "), object{
        object::pair{std:("timeout"), 30000}
    }); });
    if (result["text"]) {
        console->log(result["text"]);
    }
    if (result["error"]) {
        console->error(std:("Error:"), result["error"]);
    }
    this->showMainMenu();
}

void InteractiveClaudeCodeTester::writeFile(string filename, string content)
{
    console->log(std:("\
📝 Writing file: ") + filename + string_empty);
    auto e2bService = as<std::shared_ptr<E2bService>>(as<any>(this->session->runtime->getService(std:("e2b"))));
    if (!e2bService) {
        console->log(std:("❌ E2B service not available"));
        return std::shared_ptr<Promise<void>>();
    }
    std::async([=]() { e2bService->executeCode(std:("\
with open('") + filename + std:("', 'w') as f:\
    f.write('''") + content + std:("''')\
print(f"✅ File '{filename}' written successfully")\
      "), object{
        object::pair{std:("timeout"), 30000}
    }); });
    this->showMainMenu();
}

void InteractiveClaudeCodeTester::readFile(string filename)
{
    console->log(std:("\
📖 Reading file: ") + filename + string_empty);
    auto e2bService = as<std::shared_ptr<E2bService>>(as<any>(this->session->runtime->getService(std:("e2b"))));
    if (!e2bService) {
        console->log(std:("❌ E2B service not available"));
        return std::shared_ptr<Promise<void>>();
    }
    auto result = std::async([=]() { e2bService->executeCode(std:("\
try:\
    with open('") + filename + std:("', 'r') as f:\
        content = f.read()\
    print("─" * 50)\
    print(content)\
    print("─" * 50)\
except FileNotFoundError:\
    print(f"❌ File '{filename}' not found")\
      "), object{
        object::pair{std:("timeout"), 30000}
    }); });
    if (result["text"]) {
        console->log(result["text"]);
    }
    this->showMainMenu();
}

void InteractiveClaudeCodeTester::listFiles(string path)
{
    console->log(std:("\
📁 Listing files in: ") + path + string_empty);
    auto e2bService = as<std::shared_ptr<E2bService>>(as<any>(this->session->runtime->getService(std:("e2b"))));
    if (!e2bService) {
        console->log(std:("❌ E2B service not available"));
        return std::shared_ptr<Promise<void>>();
    }
    auto result = std::async([=]() { e2bService->executeCode(std:("\
import os\
import subprocess\
\
# Try to use ls -la for better output\
result = subprocess.run(['ls', '-la', '") + path + std:("'], capture_output=True, text=True)\
if result.returncode == 0:\
    print(result.stdout)\
else:\
    print(f"❌ Error: {result.stderr}")\
      "), object{
        object::pair{std:("timeout"), 30000}
    }); });
    if (result["text"]) {
        console->log(result["text"]);
    }
    this->showMainMenu();
}

void InteractiveClaudeCodeTester::cleanup()
{
    console->log(std:("\
🧹 Cleaning up..."));
    if (this->session->runtime) {
        auto e2bService = as<std::shared_ptr<E2bService>>(as<any>(this->session->runtime->getService(std:("e2b"))));
        if (AND((e2bService), (e2bService->stop))) {
            std::async([=]() { e2bService->stop(); });
            console->log(std:("✅ E2B service stopped"));
        }
        auto codeGenService = as<std::shared_ptr<CodeGenService>>(as<any>(this->session->runtime->getService(std:("code-generation"))));
        if (AND((codeGenService), (codeGenService->stop))) {
            std::async([=]() { codeGenService->stop(); });
            console->log(std:("✅ Code generation service stopped"));
        }
    }
}

void main()
{
    auto tester = std::make_shared<InteractiveClaudeCodeTester>();
    std::async([=]() { tester->start(); });
};



void Main(void)
{
    if (require->main == module) {
        main()->_catch([=](auto error) mutable
        {
            console->error(std:("💥 Fatal error:"), error);
            process->exit(1);
        }
        );
    }
}

MAIN
