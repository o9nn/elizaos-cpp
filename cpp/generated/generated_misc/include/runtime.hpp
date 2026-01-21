#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_ENVIRONMENT_RUNTIME_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_ENVIRONMENT_RUNTIME_H
#include "core.h"

class BashAction;
class BashActionResult;
class BashInterruptAction;
class CreateBashSessionRequest;
class Command;
class CommandResult;
class ReadFileRequest;
class ReadFileResponse;
class WriteFileRequest;
class UploadRequest;
class AbstractRuntime;

class BashAction : public object, public std::enable_shared_from_this<BashAction> {
public:
    using std::enable_shared_from_this<BashAction>::shared_from_this;
    string command;

    double timeout;

    any check;
};

class BashActionResult : public object, public std::enable_shared_from_this<BashActionResult> {
public:
    using std::enable_shared_from_this<BashActionResult>::shared_from_this;
    string output;

    double exitCode;
};

class BashInterruptAction : public object, public std::enable_shared_from_this<BashInterruptAction> {
public:
    using std::enable_shared_from_this<BashInterruptAction>::shared_from_this;
    string type;
};

class CreateBashSessionRequest : public object, public std::enable_shared_from_this<CreateBashSessionRequest> {
public:
    using std::enable_shared_from_this<CreateBashSessionRequest>::shared_from_this;
    array<string> startupSource;

    double startupTimeout;
};

class Command : public object, public std::enable_shared_from_this<Command> {
public:
    using std::enable_shared_from_this<Command>::shared_from_this;
    string command;

    boolean shell;

    boolean check;

    Record<string, string> env;

    string cwd;

    double timeout;
};

class CommandResult : public object, public std::enable_shared_from_this<CommandResult> {
public:
    using std::enable_shared_from_this<CommandResult>::shared_from_this;
    double exitCode;

    string stdout;

    string stderr;
};

class ReadFileRequest : public object, public std::enable_shared_from_this<ReadFileRequest> {
public:
    using std::enable_shared_from_this<ReadFileRequest>::shared_from_this;
    string path;

    string encoding;

    string errors;
};

class ReadFileResponse : public object, public std::enable_shared_from_this<ReadFileResponse> {
public:
    using std::enable_shared_from_this<ReadFileResponse>::shared_from_this;
    string content;
};

class WriteFileRequest : public object, public std::enable_shared_from_this<WriteFileRequest> {
public:
    using std::enable_shared_from_this<WriteFileRequest>::shared_from_this;
    string path;

    string content;
};

class UploadRequest : public object, public std::enable_shared_from_this<UploadRequest> {
public:
    using std::enable_shared_from_this<UploadRequest>::shared_from_this;
    string sourcePath;

    string targetPath;
};

class AbstractRuntime : public object, public std::enable_shared_from_this<AbstractRuntime> {
public:
    using std::enable_shared_from_this<AbstractRuntime>::shared_from_this;
    virtual std::shared_ptr<Promise<void>> createSession(std::shared_ptr<CreateBashSessionRequest> request) = 0;
    template <typename P0>
    std::shared_ptr<Promise<std::shared_ptr<BashActionResult>>> runInSession(P0 action) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<CommandResult>>> execute(std::shared_ptr<Command> command) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<ReadFileResponse>>> readFile(std::shared_ptr<ReadFileRequest> request) = 0;
    virtual std::shared_ptr<Promise<void>> writeFile(std::shared_ptr<WriteFileRequest> request) = 0;
    virtual std::shared_ptr<Promise<void>> upload(std::shared_ptr<UploadRequest> request) = 0;
};

#endif
