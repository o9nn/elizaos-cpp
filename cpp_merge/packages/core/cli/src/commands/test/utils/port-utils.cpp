#include "port-utils.h"
#include <string>

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


