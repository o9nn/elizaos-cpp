#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/dev/utils/server-manager.h"

std::shared_ptr<Promise<void>> DevServerManager::stop()
{
    if (this->process) {
        console->info(std::string("Stopping current server process..."));
        auto killed = this->process["kill"](std::string("SIGTERM"));
        if (!killed) {
            console->warn(std::string("Failed to kill server process, trying force kill..."));
            this->process["kill"](std::string("SIGKILL"));
        }
        this->process = nullptr;
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, 500);
        }
        ); });
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> DevServerManager::start(array<string> args)
{
    std::async([=]() { this->stop(); });
    console->info(std::string("Starting server..."));
    auto nodeExecutable = process->execPath;
    auto scriptPath = const_(process->argv)[1];
    this->process = spawn(nodeExecutable, array<string>{ scriptPath, std::string("start"), args }, object{
        object::pair{std::string("stdio"), std::string("inherit")}, 
        object::pair{std::string("detached"), false}, 
        object::pair{std::string("env"), utils::assign(object{
            , 
            object::pair{std::string("FORCE_COLOR"), std::string("1")}
        }, process->env)}
    });
    this->process["on"](std::string("exit"), [=](auto code, auto signal) mutable
    {
        if (code != nullptr) {
            if (code != 0) {
                console->warn(std::string("Server process exited with code ") + code + string_empty);
            } else {
                console->info(std::string("Server process exited normally"));
            }
        } else if (signal) {
            console->info(std::string("Server process was killed with signal ") + signal + string_empty);
        }
        this->process = nullptr;
    }
    );
    this->process["on"](std::string("error"), [=](auto err) mutable
    {
        console->error(std::string("Server process error: ") + err["message"] + string_empty);
        this->process = nullptr;
    }
    );
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> DevServerManager::restart(array<string> args)
{
    console->info(std::string("Restarting server..."));
    std::async([=]() { this->start(args); });
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<DevServerManager> getServerManager()
{
    if (!serverInstance) {
        serverInstance = std::make_shared<DevServerManager>();
    }
    return serverInstance;
};


std::shared_ptr<Promise<void>> stopServer()
{
    auto server = getServerManager();
    std::async([=]() { server->stop(); });
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> startServer(array<string> args)
{
    auto server = getServerManager();
    std::async([=]() { server->start(args); });
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> restartServer(array<string> args)
{
    auto server = getServerManager();
    std::async([=]() { server->restart(args); });
    return std::shared_ptr<Promise<void>>();
};


any serverInstance = nullptr;

void Main(void)
{
}

MAIN
