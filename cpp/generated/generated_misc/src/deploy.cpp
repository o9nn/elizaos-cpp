#include "auto.fun/packages/program/migrations/deploy.h"

std::any anchor = require(std::string("@coral-xyz/anchor"));

void Main(void)
{
    module->exports = [=](std::any provider) mutable
    {
        anchor["setProvider"](provider);
    };
}

MAIN
