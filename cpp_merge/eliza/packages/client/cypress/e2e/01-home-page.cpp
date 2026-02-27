#include "01-home-page.cy.h"

void Main(void)
{
    describe(std:("Home Page"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            cy->visit(std:("/"));
            cy->get(std:("#root"), object{
                object::pair{std:("timeout"), 30000}
            })->should(std:("exist"));
            cy->document()->its(std:("readyState"))->should(std:("equal"), std:("complete"));
            cy->wait(1000);
        }
        );
        it(std:("loads successfully"), [=]() mutable
        {
            cy->url()->should(std:("eq"), string_empty + Cypress->config(std:("baseUrl")) + std:("/"));
            cy->get(std:("#root"))->should(std:("exist"));
            cy->get(std:("body"))->should(std:("be.visible"));
        }
        );
        it(std:("displays the main navigation"), [=]() mutable
        {
            cy->get(std:("[data-testid="app-sidebar"]"))->should(std:("exist"));
            cy->get(std:("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std:("[data-testid="sidebar-toggle"]"))["length"] > 0) {
                    cy->get(std:("[data-testid="sidebar-toggle"]"))->should(std:("exist"));
                } else {
                    cy->get(std:("[data-testid="mobile-menu-button"]"))->should(std:("exist"));
                }
            }
            );
        }
        );
        it(std:("displays the add agent button"), [=]() mutable
        {
            cy->get(std:("[data-testid="add-agent-button"]"), object{
                object::pair{std:("timeout"), 10000}
            })->should(std:("exist"));
        }
        );
        it(std:("displays connection status"), [=]() mutable
        {
            cy->get(std:("[data-testid="connection-status"]"), object{
                object::pair{std:("timeout"), 10000}
            })->should(std:("exist"));
        }
        );
        it(std:("can toggle sidebar"), [=]() mutable
        {
            cy->get(std:("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std:("[data-testid="sidebar-toggle"]"))["length"] > 0) {
                    cy->get(std:("[data-testid="sidebar-toggle"]"))->click();
                    cy->wait(500);
                    cy->get(std:("[data-testid="sidebar-toggle"]"))->click();
                    cy->get(std:("[data-testid="sidebar-toggle"]"))->should(std:("exist"));
                } else {
                    cy->get(std:("[data-testid="mobile-menu-button"]"))->should(std:("exist"));
                    cy->log(std:("Sidebar toggle not available in current layout"));
                }
            }
            );
        }
        );
        it(std:("handles responsive design"), [=]() mutable
        {
            cy->viewport(std:("iphone-x"));
            cy->wait(1000);
            cy->get(std:("[data-testid="mobile-menu-button"]"))->should(std:("be.visible"));
            cy->get(std:("[data-testid="mobile-menu-button"]"))->click(object{
                object::pair{std:("force"), true}
            });
            cy->wait(500);
            cy->get(std:("[data-testid="app-sidebar"]"))->should(std:("exist"));
            cy->viewport(1280, 720);
            cy->wait(500);
        }
        );
        it(std:("shows loading states properly"), [=]() mutable
        {
            cy->intercept(std:("GET"), std:("/api/agents"), object{
                object::pair{std:("delay"), 1000}, 
                object::pair{std:("body"), object{
                    object::pair{std:("data"), object{
                        object::pair{std:("agents"), array<any>()}
                    }}
                }}
            })->as(std:("getAgents"));
            cy->reload();
            cy->get(std:("#root"), object{
                object::pair{std:("timeout"), 30000}
            })->should(std:("exist"));
            cy->document()->its(std:("readyState"))->should(std:("equal"), std:("complete"));
            cy->wait(500);
            cy->wait(std:("@getAgents"));
            cy->get(std:("#root"))->should(std:("exist"));
        }
        );
        it(std:("handles errors gracefully"), [=]() mutable
        {
            cy->intercept(std:("GET"), std:("/api/agents"), object{
                object::pair{std:("statusCode"), 500}, 
                object::pair{std:("body"), object{
                    object::pair{std:("error"), std:("Server error")}
                }}
            })->as(std:("getAgentsError"));
            cy->reload();
            cy->get(std:("#root"), object{
                object::pair{std:("timeout"), 30000}
            })->should(std:("exist"));
            cy->document()->its(std:("readyState"))->should(std:("equal"), std:("complete"));
            cy->wait(500);
            cy->wait(std:("@getAgentsError"));
            cy->get(std:("#root"))->should(std:("exist"));
            cy->get(std:("[data-testid="app-sidebar"]"))->should(std:("exist"));
        }
        );
        it(std:("loads basic page structure"), [=]() mutable
        {
            cy->get(std:("#root"))->should(std:("exist"));
            cy->get(std:("[data-testid="app-sidebar"]"))->should(std:("exist"));
            cy->get(std:("body"))->should(std:("not.contain.text"), std:("Error:"));
            cy->get(std:("body"))->should(std:("not.contain.text"), std:("TypeError:"));
        }
        );
        it(std:("has working navigation elements"), [=]() mutable
        {
            cy->get(std:("[data-testid="app-sidebar"]"))->should(std:("exist"));
            cy->get(std:("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std:("[data-testid="sidebar-toggle"]"))["length"] > 0) {
                    cy->get(std:("[data-testid="sidebar-toggle"]"))->should(std:("exist"))->click();
                    cy->get(std:("[data-testid="sidebar-toggle"]"))->click();
                    cy->get(std:("[data-testid="app-sidebar"]"))->should(std:("exist"));
                } else {
                    cy->get(std:("[data-testid="add-agent-button"]"))->should(std:("exist"));
                    cy->get(std:("[data-testid="mobile-menu-button"]"))->should(std:("exist"));
                    cy->log(std:("Sidebar toggle not available, verified other navigation elements"));
                }
            }
            );
        }
        );
    }
    );
}

MAIN
