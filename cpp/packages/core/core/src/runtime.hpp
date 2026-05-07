// Runtime abstraction for ElizaOS-CPP core
// Real, working implementation (replaces previous broken transpiled stub)
#ifndef ELIZAOS_CORE_RUNTIME_HPP
#define ELIZAOS_CORE_RUNTIME_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <future>
#include <functional>
#include <chrono>
#include <stdexcept>

namespace elizaos {
namespace runtime {

// ============================================================================
// Bash session abstraction
// ============================================================================
struct BashAction {
    std::string command;
    double timeout = 30.0;     // seconds
    bool check = true;         // throw on non-zero exit
};

struct BashActionResult {
    std::string output;
    int exitCode = 0;
};

struct BashInterruptAction {
    std::string type = "interrupt";
};

struct CreateBashSessionRequest {
    std::vector<std::string> startupSource;
    double startupTimeout = 30.0;
};

// ============================================================================
// Process command abstraction
// ============================================================================
struct Command {
    std::string command;
    bool shell = true;
    bool check = true;
    std::map<std::string, std::string> env;
    std::string cwd;
    double timeout = 30.0;
};

struct CommandResult {
    int exitCode = 0;
    std::string stdoutText;
    std::string stderrText;
};

// ============================================================================
// File transfer abstraction
// ============================================================================
struct ReadFileRequest {
    std::string path;
    std::string encoding = "utf-8";
    std::string errors = "strict";
};

struct ReadFileResponse {
    std::string content;
};

struct WriteFileRequest {
    std::string path;
    std::string content;
};

struct UploadRequest {
    std::string sourcePath;
    std::string targetPath;
};

// ============================================================================
// Abstract runtime interface
// ============================================================================
class AbstractRuntime {
public:
    virtual ~AbstractRuntime() = default;

    virtual std::future<void> createSession(const CreateBashSessionRequest& request) = 0;
    virtual std::future<BashActionResult> runInSession(const BashAction& action) = 0;
    virtual std::future<CommandResult> execute(const Command& command) = 0;
    virtual std::future<ReadFileResponse> readFile(const ReadFileRequest& request) = 0;
    virtual std::future<void> writeFile(const WriteFileRequest& request) = 0;
    virtual std::future<void> upload(const UploadRequest& request) = 0;
};

// ============================================================================
// Concrete LocalRuntime - executes via std::system / fstream on the host
// ============================================================================
class LocalRuntime : public AbstractRuntime {
public:
    LocalRuntime();
    ~LocalRuntime() override;

    std::future<void> createSession(const CreateBashSessionRequest& request) override;
    std::future<BashActionResult> runInSession(const BashAction& action) override;
    std::future<CommandResult> execute(const Command& command) override;
    std::future<ReadFileResponse> readFile(const ReadFileRequest& request) override;
    std::future<void> writeFile(const WriteFileRequest& request) override;
    std::future<void> upload(const UploadRequest& request) override;

    bool sessionActive() const { return sessionActive_; }

private:
    bool sessionActive_ = false;
    std::vector<std::string> startupSource_;
};

} // namespace runtime
} // namespace elizaos

#endif // ELIZAOS_CORE_RUNTIME_HPP
