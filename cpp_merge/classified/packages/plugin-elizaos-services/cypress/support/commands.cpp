#include "commands.hpp"
#include <string>

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
            (as<any>(win))["ELIZA_CONFIG"] = config;
        }
        );
    }
    );
}

MAIN
