#include "e2e.hpp"
#include <string>

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->on(std::string("uncaught:exception"), [=](auto err, auto runnable) mutable
    {
        return false;
    }
    );
}

MAIN
