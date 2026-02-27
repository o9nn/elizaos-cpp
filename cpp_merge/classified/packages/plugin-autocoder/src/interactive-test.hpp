#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_AUTOCODER_SRC_INTERACTIVE_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_AUTOCODER_SRC_INTERACTIVE_TEST_H
#include "core.hpp"
// External dependency removed
#include "readline.hpp"
#include "uuid.hpp"
using uuidv4 = v4;

class TestSession;
class CodeGenService;
class E2bService;
class InteractiveClaudeCodeTester;

class TestSession : public object, public std::enable_shared_from_this<TestSession> {
public:
    using std::enable_shared_from_this<TestSession>::shared_from_this;
    string sessionId;

    std::shared_ptr<IAgentRuntime> runtime;

    string projectPath;

    object currentProject;
};

class CodeGenService : public object, public std::enable_shared_from_this<CodeGenService> {
public:
    using std::enable_shared_from_this<CodeGenService>::shared_from_this;
    virtual std::shared_ptr<Promise<any>> generateCode(any request) = 0;
    virtual std::shared_ptr<Promise<void>> stop() = 0;
};

class E2bService : public object, public std::enable_shared_from_this<E2bService> {
public:
    using std::enable_shared_from_this<E2bService>::shared_from_this;
    virtual std::shared_ptr<Promise<any>> executeCode(string code, any options = undefined) = 0;
    virtual std::shared_ptr<Promise<void>> stop() = 0;
};

class InteractiveClaudeCodeTester : public object, public std::enable_shared_from_this<InteractiveClaudeCodeTester> {
public:
    using std::enable_shared_from_this<InteractiveClaudeCodeTester>::shared_from_this;
    std::shared_ptr<TestSession> session;

    std::shared_ptr<Interface> rl;

    InteractiveClaudeCodeTester();
    virtual void start();
    virtual void showMainMenu();
    virtual void handleMenuChoice(string choice);
    virtual void callClaude(string prompt);
    virtual void generateProject(string description);
    virtual void runSandboxCommand(string command);
    virtual void writeFile(string filename, string content);
    virtual void readFile(string filename);
    virtual void listFiles(string path);
    virtual void cleanup();
};

void main();

#endif
