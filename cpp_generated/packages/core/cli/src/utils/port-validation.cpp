#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/port-validation.h"

double validatePort(string value)
{
    auto port = Number->parseInt(value, 10);
    if (OR((OR((Number->isNaN(port)), (port <= 0))), (port > 65535))) {
        throw any(std::make_shared<Error>(std::string("Port must be a number between 1 and 65535")));
    }
    return port;
};


