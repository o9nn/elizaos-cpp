#include "deploy.hpp"

any anchor = require(std:("@coral-xyz/anchor"));

void Main(void)
{
    module->exports = [=](any provider) mutable
    {
        anchor["setProvider"](provider);
    };
}

MAIN
