#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/migrations/deploy.h"

any anchor = require(std::string("@coral-xyz/anchor"));

void Main(void)
{
    module->exports = [=](any provider) mutable
    {
        anchor["setProvider"](provider);
    };
}

MAIN
