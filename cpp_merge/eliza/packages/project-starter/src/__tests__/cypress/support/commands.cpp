#include "commands.hpp"

namespace global {
}
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
            win["ELIZA_CONFIG"] = config;
        }
        );
    }
    );
}

MAIN
