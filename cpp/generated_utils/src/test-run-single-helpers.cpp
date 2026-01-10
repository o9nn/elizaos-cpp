#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-run-single-helpers.h"

std::shared_ptr<Promise<void>> MockRuntime::createSession(std::shared_ptr<CreateBashSessionRequest> _request)
{
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<std::shared_ptr<CommandResult>>> MockRuntime::execute(std::shared_ptr<Command> _command)
{
    return object{
        object::pair{std::string("exitCode"), 0}, 
        object::pair{std::string("stdout"), string_empty}, 
        object::pair{std::string("stderr"), string_empty}
    };
}

std::shared_ptr<Promise<std::shared_ptr<ReadFileResponse>>> MockRuntime::readFile(std::shared_ptr<ReadFileRequest> request)
{
    auto content = OR((this->files->get(request->path)), (string_empty));
    return object{
        object::pair{std::string("content"), std::string("content")}
    };
}

std::shared_ptr<Promise<void>> MockRuntime::writeFile(std::shared_ptr<WriteFileRequest> request)
{
    this->files->set(request->path, request->content);
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> MockRuntime::upload(std::shared_ptr<UploadRequest> _request)
{
    return std::shared_ptr<Promise<void>>();
}

MockDeployment::MockDeployment() : AbstractDeployment() {
    this->runtime = std::make_shared<MockRuntime>();
}

std::shared_ptr<Promise<void>> MockDeployment::start()
{
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> MockDeployment::stop()
{
    return std::shared_ptr<Promise<void>>();
}

