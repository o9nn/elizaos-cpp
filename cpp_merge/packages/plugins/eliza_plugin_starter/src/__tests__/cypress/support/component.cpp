#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-starter/src/__tests__/cypress/support/component.h"

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->Commands->add(std:("mount"), mount);
}

MAIN
