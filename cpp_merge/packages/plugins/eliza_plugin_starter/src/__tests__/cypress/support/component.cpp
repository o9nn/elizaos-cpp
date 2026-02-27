#include "component.hpp"

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->Commands->add(std:("mount"), mount);
}

MAIN
