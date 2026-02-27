#include "deploy.hpp"
#include <string>

any anchor = require(std::string("@coral-xyz/anchor"));

void Main(void)
{
    module->exports = [=](any provider) mutable
    {
        anchor["setProvider"](provider);
    };
}

MAIN
