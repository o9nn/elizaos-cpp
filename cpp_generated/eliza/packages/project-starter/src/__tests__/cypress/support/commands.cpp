#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/cypress/support/commands.h"

namespace global {
}
namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->Commands->add(std::string("shouldBeDarkMode"), [=]() mutable
    {
        cy->get(std::string("html"))->should(std::string("have.class"), std::string("dark"));
    }
    );
    Cypress->Commands->add(std::string("setElizaConfig"), [=](auto config) mutable
    {
        cy->window()->then([=](auto win) mutable
        {
            win["ELIZA_CONFIG"] = config;
        }
        );
    }
    );
}

MAIN
