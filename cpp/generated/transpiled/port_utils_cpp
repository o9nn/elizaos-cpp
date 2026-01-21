#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/test/utils/port-utils.h"

std::shared_ptr<Promise<boolean>> checkPortAvailable(double port)
{
    return std::make_shared<Promise>([=](auto resolve) mutable
    {
        shared server = net->createServer();
        server->once(std::string("error"), [=]() mutable
        {
            resolve(false);
        }
        );
        server->once(std::string("listening"), [=]() mutable
        {
            server->close();
            resolve(true);
        }
        );
        server->listen(port);
    }
    );
};


