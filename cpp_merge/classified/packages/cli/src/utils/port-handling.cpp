#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/utils/port-handling.h"

std::shared_ptr<Promise<boolean>> isPortFree(double port)
{
    return std::make_shared<Promise>([=](auto resolve) mutable
    {
        shared server = net->createServer();
        server->once(std:("error"), [=]() mutable
        {
            return resolve(false);
        }
        );
        server->once(std:("listening"), [=]() mutable
        {
            server->close();
            resolve(true);
        }
        );
        server->listen(port);
    }
    );
};


std::shared_ptr<Promise<double>> findNextAvailablePort(double startPort)
{
    auto port = startPort;
    while (!(std::async([=]() { isPortFree(port); })))
    {
        port++;
    }
    return port;
};


