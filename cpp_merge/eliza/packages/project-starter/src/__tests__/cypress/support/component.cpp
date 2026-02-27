#include "component.hpp"
#include <string>

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->Commands->add(std::string("mount"), mount);
}

MAIN
