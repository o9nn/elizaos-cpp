#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/interactive-test.h"

InteractiveClaudeCodeTester::InteractiveClaudeCodeTester() {
    this->session = object{
        object::pair{std::string("sessionId"), uuidv4()}, 
        object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(object{})}
    };
    this->rl = createInterface(object{
        object::pair{std::string("input"), process->stdin}, 
        object::pair{std::string("output"), process->stdout}
    });
}

void InteractiveClaudeCodeTester::start()
{
    console->log(std::string("🚀 Starting Interactive Claude Code Tester"));
    console->log(std::string("═")->repeat(50));
    try
    {
        auto runtime = std::make_shared<AgentRuntime>(object{
            object::pair{std::string("plugins"), array<any>()}
        });
        auto getValue = [=](auto key) mutable
        {
            if (key == std::string("OPENAI_API_KEY")) return process->env->OPENAI_API_KEY;
            if (key == std::string("E2B_API_KEY")) return process->env->E2B_API_KEY;
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
        auto codeGenService = runtime->getService(std::string("code-generation"));
        auto e2bService = runtime->getService(std::string("e2b"));
        auto formsService = runtime->getService(std::string("forms"));
        if (!codeGenService) {
            console->log(std::string("⚠️  Code generation service not available"));
        }
        if (AND((!e2bService), (process->env->E2B_API_KEY))) {
            console->log(std::string("⚠️  E2B service not available (but API key is set)"));
        }
        console->log(std::string("\
📋 Available Services:"));
        console->log(std::string("─")->repeat(30));
        console->log(std::string("   ") + (formsService) ? std::string("✅") : std::string("❌") + std::string(" forms"));
        console->log(std::string("   ") + (codeGenService) ? std::string("✅") : std::string("❌") + std::string(" code-generation"));
        console->log(std::string("   ") + (e2bService) ? std::string("✅") : std::string("❌") + std::string(" e2b"));
        console->log(std::string("\
✅ Runtime initialized successfully"));
        this->showMainMenu();
    }
    catch (const any& error)
    {
        console->error(std::string("❌ Failed to initialize runtime:"), error);
        process->exit(1);
    }
}

void InteractiveClaudeCodeTester::showMainMenu()
{
    console->log(std::string("\
🎯 Main Menu"));
    console->log(std::string("─")->repeat(20));
    console->log(std::string("1. Generate Project"));
    console->log(std::string("2. Run Sandbox Command"));
    console->log(std::string("3. Write File"));
    console->log(std::string("4. Read File"));
    console->log(std::string("5. List Files"));
    console->log(std::string("6. Exit"));
    console->log(string_empty);
    this->rl->question(std::string("Choose an option (1-6): "), [=](auto answer) mutable
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
            { any(std::string("1")), 1 },
            { any(std::string("2")), 2 },
            { any(std::string("3")), 3 },
            { any(std::string("4")), 4 },
            { any(std::string("5")), 5 },
            { any(std::string("6")), 6 }
        };
        switch (__switch3403_4597[choice])
        {
        case 1:
            this->rl->question(std::string("Enter project description: "), [=](auto description) mutable
            {
                this->generateProject(description);
            }
            );
            break;
        case 2:
            this->rl->question(std::string("Enter command to run: "), [=](auto command) mutable
            {
                this->runSandboxCommand(command);
            }
            );
            break;
        case 3:
            this->rl->question(std::string("Enter filename: "), [=](auto filename) mutable
            {
                this->rl->question(std::string("Enter file content: "), [=](auto content) mutable
                {
                    this->writeFile(filename, content);
                }
                );
            }
            );
            break;
        case 4:
            this->rl->question(std::string("Enter filename to read: "), [=](auto filename) mutable
            {
                this->readFile(filename);
            }
            );
            break;
        case 5:
            this->rl->question(std::string("Enter path to list (default: .): "), [=](auto path) mutable
            {
                this->listFiles(OR((path), (std::string("."))));
            }
            );
            break;
        case 6:
            std::async([=]() { this->cleanup(); });
            console->log(std::string("👋 Goodbye!"));
            process->exit(0);
            break;
        default:
            console->log(std::string("❌ Invalid choice. Please try again."));
            this->showMainMenu();
            break;
        }
    }
    catch (const any& error)
    {
        console->error(std::string("❌ Error:"), error);
        this->showMainMenu();
    }
}

void InteractiveClaudeCodeTester::callClaude(string prompt)
{
    console->log(std::string("🤖 Calling Claude..."));
    console->log(std::string("📝 Prompt: ") + prompt + string_empty);
    try
    {
        console->log(std::string("✅ Claude responded (mock response)"));
        this->showMainMenu();
    }
    catch (const any& error)
    {
        console->error(std::string("❌ Error calling Claude:"), error);
    }
}

void InteractiveClaudeCodeTester::generateProject(string description)
{
    console->log(std::string("\
🚀 Generating project..."));
    console->log(std::string("📝 Description: ") + description + string_empty);
    auto startTime = Date->now();
    auto codeGenService = as<std::shared_ptr<CodeGenService>>(as<any>(this->session->runtime->getService(std::string("code-generation"))));
    if (!codeGenService) {
        throw any(std::make_shared<Error>(std::string("Code generation service not available")));
    }
    auto request = object{
        object::pair{std::string("projectName"), std::string("generated-project-") + Date->now() + string_empty}, 
        object::pair{std::string("description"), std::string("description")}, 
        object::pair{std::string("requirements"), array<string>{ description }}, 
        object::pair{std::string("apis"), array<any>()}, 
        object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
        object::pair{std::string("testScenarios"), array<string>{ std::string("Basic functionality test") }}
    };
    auto result = std::async([=]() { codeGenService->generateCode(request); });
    auto duration = Date->now() - startTime;
    console->log(std::string("\
✅ Project Generated (") + duration + std::string("ms):"));
    console->log(std::string("─")->repeat(50));
    console->log(std::string("📁 Project: ") + request["projectName"] + string_empty);
    console->log(std::string("✅ Success: ") + result["success"] + string_empty);
    if (result["projectPath"]) {
        console->log(std::string("📂 Path: ") + result["projectPath"] + string_empty);
        this->session->currentProject = object{
            object::pair{std::string("id"), OR((result["id"]), (uuidv4()))}, 
            object::pair{std::string("name"), request["projectName"]}, 
            object::pair{std::string("type"), request["targetType"]}, 
            object::pair{std::string("status"), (result["success"]) ? std::string("completed") : std::string("failed")}, 
            object::pair{std::string("path"), result["projectPath"]}
        };
        this->session->projectPath = result["projectPath"];
    }
    this->showMainMenu();
}

void InteractiveClaudeCodeTester::runSandboxCommand(string command)
{
    console->log(std::string("\
🔧 Running: ") + command + string_empty);
    auto e2bService = as<std::shared_ptr<E2bService>>(as<any>(this->session->runtime->getService(std::string("e2b"))));
    if (!e2bService) {
        console->log(std::string("❌ E2B service not available"));
        return std::shared_ptr<Promise<void>>();
    }
    auto result = std::async([=]() { e2bService->executeCode(std::string("\
import subprocess\
result = subprocess.run('") + command + std::string("'.split(), capture_output=True, text=True)\
print("STDOUT:", result.stdout)\
print("STDERR:", result.stderr)\
print("EXIT_CODE:", result.returncode)\
      "), object{
        object::pair{std::string("timeout"), 30000}
    }); });
    if (result["text"]) {
        console->log(result["text"]);
    }
    if (result["error"]) {
        console->error(std::string("Error:"), result["error"]);
    }
    this->showMainMenu();
}

void InteractiveClaudeCodeTester::writeFile(string filename, string content)
{
    console->log(std::string("\
📝 Writing file: ") + filename + string_empty);
    auto e2bService = as<std::shared_ptr<E2bService>>(as<any>(this->session->runtime->getService(std::string("e2b"))));
    if (!e2bService) {
        console->log(std::string("❌ E2B service not available"));
        return std::shared_ptr<Promise<void>>();
    }
    std::async([=]() { e2bService->executeCode(std::string("\
with open('") + filename + std::string("', 'w') as f:\
    f.write('''") + content + std::string("''')\
print(f"✅ File '{filename}' written successfully")\
      "), object{
        object::pair{std::string("timeout"), 30000}
    }); });
    this->showMainMenu();
}

void InteractiveClaudeCodeTester::readFile(string filename)
{
    console->log(std::string("\
📖 Reading file: ") + filename + string_empty);
    auto e2bService = as<std::shared_ptr<E2bService>>(as<any>(this->session->runtime->getService(std::string("e2b"))));
    if (!e2bService) {
        console->log(std::string("❌ E2B service not available"));
        return std::shared_ptr<Promise<void>>();
    }
    auto result = std::async([=]() { e2bService->executeCode(std::string("\
try:\
    with open('") + filename + std::string("', 'r') as f:\
        content = f.read()\
    print("─" * 50)\
    print(content)\
    print("─" * 50)\
except FileNotFoundError:\
    print(f"❌ File '{filename}' not found")\
      "), object{
        object::pair{std::string("timeout"), 30000}
    }); });
    if (result["text"]) {
        console->log(result["text"]);
    }
    this->showMainMenu();
}

void InteractiveClaudeCodeTester::listFiles(string path)
{
    console->log(std::string("\
📁 Listing files in: ") + path + string_empty);
    auto e2bService = as<std::shared_ptr<E2bService>>(as<any>(this->session->runtime->getService(std::string("e2b"))));
    if (!e2bService) {
        console->log(std::string("❌ E2B service not available"));
        return std::shared_ptr<Promise<void>>();
    }
    auto result = std::async([=]() { e2bService->executeCode(std::string("\
import os\
import subprocess\
\
# Try to use ls -la for better output\
result = subprocess.run(['ls', '-la', '") + path + std::string("'], capture_output=True, text=True)\
if result.returncode == 0:\
    print(result.stdout)\
else:\
    print(f"❌ Error: {result.stderr}")\
      "), object{
        object::pair{std::string("timeout"), 30000}
    }); });
    if (result["text"]) {
        console->log(result["text"]);
    }
    this->showMainMenu();
}

void InteractiveClaudeCodeTester::cleanup()
{
    console->log(std::string("\
🧹 Cleaning up..."));
    if (this->session->runtime) {
        auto e2bService = as<std::shared_ptr<E2bService>>(as<any>(this->session->runtime->getService(std::string("e2b"))));
        if (AND((e2bService), (e2bService->stop))) {
            std::async([=]() { e2bService->stop(); });
            console->log(std::string("✅ E2B service stopped"));
        }
        auto codeGenService = as<std::shared_ptr<CodeGenService>>(as<any>(this->session->runtime->getService(std::string("code-generation"))));
        if (AND((codeGenService), (codeGenService->stop))) {
            std::async([=]() { codeGenService->stop(); });
            console->log(std::string("✅ Code generation service stopped"));
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
            console->error(std::string("💥 Fatal error:"), error);
            process->exit(1);
        }
        );
    }
}

MAIN
