#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/cypress/support/commands.h"

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->Commands->add(std:("waitForApp"), [=]() mutable
    {
        cy->get(std:("#root"), object{
            object::pair{std:("timeout"), 30000}
        })->should(std:("exist"));
        cy->document()->its(std:("readyState"))->should(std:("equal"), std:("complete"));
        cy->wait(1000);
        cy->get(std:("body"))->then([=](auto $body) mutable
        {
            if ($body["find"](std:("[data-testid="loading"]"))["length"] > 0) {
                cy->get(std:("[data-testid="loading"]"), object{
                    object::pair{std:("timeout"), 30000}
                })->should(std:("not.exist"));
            }
        }
        );
    }
    );
    Cypress->Commands->add(std:("login"), [=](auto email, auto password) mutable
    {
        cy->visit(std:("/login"));
        cy->get(std:("input[name="email"]"))->type(email);
        cy->get(std:("input[name="password"]"))->type(password);
        cy->get(std:("button[type="submit"]"))->click();
        cy->url()->should(std:("not.include"), std:("/login"));
    }
    );
    Cypress->Commands->add(std:("connectWebSocket"), [=]() mutable
    {
        cy->window()->then([=](auto win) mutable
        {
            cy->wrap(nullptr)->then([=]() mutable
            {
                return std::make_shared<Cypress->Promise>([=](auto resolve) mutable
                {
                    shared checkConnection = [=]() mutable
                    {
                        if ((as<any>(win))["socket"]["connected"]) {
                            resolve(undefined);
                        } else {
                            setTimeout(checkConnection, 100);
                        }
                    };
                    checkConnection();
                }
                );
            }
            );
        }
        );
    }
    );
    Cypress->Commands->add(std:("cleanupTestData"), [=]() mutable
    {
        cy->window()->then([=](auto win) mutable
        {
            win["localStorage"]["clear"]();
            win["sessionStorage"]["clear"]();
            cy->clearCookies();
        }
        );
    }
    );
    Cypress->Commands->add(std:("getByTestId"), [=](auto testId) mutable
    {
        return cy->get(std:("[data-testid="") + testId + std:(""]"));
    }
    );
    Cypress->Commands->add(std:("waitForApi"), [=](auto alias, auto timeout = 10000) mutable
    {
        return cy->wait(std:("@") + alias + string_empty, object{
            object::pair{std:("timeout"), std:("timeout")}
        });
    }
    );
}

MAIN
