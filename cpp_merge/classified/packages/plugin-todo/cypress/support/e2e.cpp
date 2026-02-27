#include "e2e.hpp"

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->on(std:("uncaught:exception"), [=](auto err, auto runnable) mutable
    {
        return false;
    }
    );
}

MAIN
