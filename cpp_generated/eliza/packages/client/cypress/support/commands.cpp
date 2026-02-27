#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/cypress/support/commands.h"

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->Commands->add(std::string("waitForApp"), [=]() mutable
    {
        cy->get(std::string("#root"), object{
            object::pair{std::string("timeout"), 30000}
        })->should(std::string("exist"));
        cy->document()->its(std::string("readyState"))->should(std::string("equal"), std::string("complete"));
        cy->wait(1000);
        cy->get(std::string("body"))->then([=](auto $body) mutable
        {
            if ($body["find"](std::string("[data-testid="loading"]"))["length"] > 0) {
                cy->get(std::string("[data-testid="loading"]"), object{
                    object::pair{std::string("timeout"), 30000}
                })->should(std::string("not.exist"));
            }
        }
        );
    }
    );
    Cypress->Commands->add(std::string("login"), [=](auto email, auto password) mutable
    {
        cy->visit(std::string("/login"));
        cy->get(std::string("input[name="email"]"))->type(email);
        cy->get(std::string("input[name="password"]"))->type(password);
        cy->get(std::string("button[type="submit"]"))->click();
        cy->url()->should(std::string("not.include"), std::string("/login"));
    }
    );
    Cypress->Commands->add(std::string("connectWebSocket"), [=]() mutable
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
    Cypress->Commands->add(std::string("cleanupTestData"), [=]() mutable
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
    Cypress->Commands->add(std::string("getByTestId"), [=](auto testId) mutable
    {
        return cy->get(std::string("[data-testid="") + testId + std::string(""]"));
    }
    );
    Cypress->Commands->add(std::string("waitForApi"), [=](auto alias, auto timeout = 10000) mutable
    {
        return cy->wait(std::string("@") + alias + string_empty, object{
            object::pair{std::string("timeout"), std::string("timeout")}
        });
    }
    );
}

MAIN
