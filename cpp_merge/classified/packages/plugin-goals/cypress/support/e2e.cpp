#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/cypress/support/e2e.h"

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
