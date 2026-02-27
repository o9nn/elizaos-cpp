#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-elizaos-services/cypress/support/commands.h"

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->Commands->add(std:("shouldBeDarkMode"), [=]() mutable
    {
        cy->get(std:("html"))->should(std:("have.class"), std:("dark"));
    }
    );
    Cypress->Commands->add(std:("setElizaConfig"), [=](auto config) mutable
    {
        cy->window()->then([=](auto win) mutable
        {
            (as<any>(win))["ELIZA_CONFIG"] = config;
        }
        );
    }
    );
}

MAIN
