#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_INTERACTIVE-TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_INTERACTIVE-TEST_H
#include "core.h"
#include "@elizaos/core.h"
#include "readline.h"
#include "uuid.h"
using uuidv4 = v4;

class TestSession;
class CodeGenService;
class E2bService;
class InteractiveClaudeCodeTester;

class TestSession : public object, public std::enable_shared_from_this<TestSession> {
public:
    using std::enable_shared_from_this<TestSession>::shared_from_this;
    std::string sessionId;

    std::shared_ptr<IAgentRuntime> runtime;

    std::string projectPath;

    object currentProject;
};

class CodeGenService : public object, public std::enable_shared_from_this<CodeGenService> {
public:
    using std::enable_shared_from_this<CodeGenService>::shared_from_this;
    virtual std::shared_ptr<Promise<any>> generateCode(std::any request) = 0;
    virtual std::shared_ptr<Promise<void>> stop() = 0;
};

class E2bService : public object, public std::enable_shared_from_this<E2bService> {
public:
    using std::enable_shared_from_this<E2bService>::shared_from_this;
    virtual std::shared_ptr<Promise<any>> executeCode(std::string code, std::any options = undefined) = 0;
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
    virtual void handleMenuChoice(std::string choice);
    virtual void callClaude(std::string prompt);
    virtual void generateProject(std::string description);
    virtual void runSandboxCommand(std::string command);
    virtual void writeFile(std::string filename, std::string content);
    virtual void readFile(std::string filename);
    virtual void listFiles(std::string path);
    virtual void cleanup();
};

void main();

#endif
