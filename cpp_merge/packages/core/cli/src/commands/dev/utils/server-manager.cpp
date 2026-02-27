#include "server-manager.h"

std::shared_ptr<Promise<void>> DevServerManager::stop()
{
    if (this->process) {
        console->info(std:("Stopping current server process..."));
        auto killed = this->process["kill"](std:("SIGTERM"));
        if (!killed) {
            console->warn(std:("Failed to kill server process, trying force kill..."));
            this->process["kill"](std:("SIGKILL"));
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
    console->info(std:("Starting server..."));
    auto nodeExecutable = process->execPath;
    auto scriptPath = const_(process->argv)[1];
    this->process = spawn(nodeExecutable, array<string>{ scriptPath, std:("start"), args }, object{
        object::pair{std:("stdio"), std:("inherit")}, 
        object::pair{std:("detached"), false}, 
        object::pair{std:("env"), utils::assign(object{
            , 
            object::pair{std:("FORCE_COLOR"), std:("1")}
        }, process->env)}
    });
    this->process["on"](std:("exit"), [=](auto code, auto signal) mutable
    {
        if (code != nullptr) {
            if (code != 0) {
                console->warn(std:("Server process exited with code ") + code + string_empty);
            } else {
                console->info(std:("Server process exited normally"));
            }
        } else if (signal) {
            console->info(std:("Server process was killed with signal ") + signal + string_empty);
        }
        this->process = nullptr;
    }
    );
    this->process["on"](std:("error"), [=](auto err) mutable
    {
        console->error(std:("Server process error: ") + err["message"] + string_empty);
        this->process = nullptr;
    }
    );
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> DevServerManager::restart(array<string> args)
{
    console->info(std:("Restarting server..."));
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
