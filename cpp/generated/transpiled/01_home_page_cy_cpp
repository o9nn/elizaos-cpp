#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/cypress/e2e/01-home-page.cy.h"

void Main(void)
{
    describe(std::string("Home Page"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            cy->visit(std::string("/"));
            cy->get(std::string("#root"), object{
                object::pair{std::string("timeout"), 30000}
            })->should(std::string("exist"));
            cy->document()->its(std::string("readyState"))->should(std::string("equal"), std::string("complete"));
            cy->wait(1000);
        }
        );
        it(std::string("loads successfully"), [=]() mutable
        {
            cy->url()->should(std::string("eq"), string_empty + Cypress->config(std::string("baseUrl")) + std::string("/"));
            cy->get(std::string("#root"))->should(std::string("exist"));
            cy->get(std::string("body"))->should(std::string("be.visible"));
        }
        );
        it(std::string("displays the main navigation"), [=]() mutable
        {
            cy->get(std::string("[data-testid="app-sidebar"]"))->should(std::string("exist"));
            cy->get(std::string("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std::string("[data-testid="sidebar-toggle"]"))["length"] > 0) {
                    cy->get(std::string("[data-testid="sidebar-toggle"]"))->should(std::string("exist"));
                } else {
                    cy->get(std::string("[data-testid="mobile-menu-button"]"))->should(std::string("exist"));
                }
            }
            );
        }
        );
        it(std::string("displays the add agent button"), [=]() mutable
        {
            cy->get(std::string("[data-testid="add-agent-button"]"), object{
                object::pair{std::string("timeout"), 10000}
            })->should(std::string("exist"));
        }
        );
        it(std::string("displays connection status"), [=]() mutable
        {
            cy->get(std::string("[data-testid="connection-status"]"), object{
                object::pair{std::string("timeout"), 10000}
            })->should(std::string("exist"));
        }
        );
        it(std::string("can toggle sidebar"), [=]() mutable
        {
            cy->get(std::string("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std::string("[data-testid="sidebar-toggle"]"))["length"] > 0) {
                    cy->get(std::string("[data-testid="sidebar-toggle"]"))->click();
                    cy->wait(500);
                    cy->get(std::string("[data-testid="sidebar-toggle"]"))->click();
                    cy->get(std::string("[data-testid="sidebar-toggle"]"))->should(std::string("exist"));
                } else {
                    cy->get(std::string("[data-testid="mobile-menu-button"]"))->should(std::string("exist"));
                    cy->log(std::string("Sidebar toggle not available in current layout"));
                }
            }
            );
        }
        );
        it(std::string("handles responsive design"), [=]() mutable
        {
            cy->viewport(std::string("iphone-x"));
            cy->wait(1000);
            cy->get(std::string("[data-testid="mobile-menu-button"]"))->should(std::string("be.visible"));
            cy->get(std::string("[data-testid="mobile-menu-button"]"))->click(object{
                object::pair{std::string("force"), true}
            });
            cy->wait(500);
            cy->get(std::string("[data-testid="app-sidebar"]"))->should(std::string("exist"));
            cy->viewport(1280, 720);
            cy->wait(500);
        }
        );
        it(std::string("shows loading states properly"), [=]() mutable
        {
            cy->intercept(std::string("GET"), std::string("/api/agents"), object{
                object::pair{std::string("delay"), 1000}, 
                object::pair{std::string("body"), object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("agents"), array<any>()}
                    }}
                }}
            })->as(std::string("getAgents"));
            cy->reload();
            cy->get(std::string("#root"), object{
                object::pair{std::string("timeout"), 30000}
            })->should(std::string("exist"));
            cy->document()->its(std::string("readyState"))->should(std::string("equal"), std::string("complete"));
            cy->wait(500);
            cy->wait(std::string("@getAgents"));
            cy->get(std::string("#root"))->should(std::string("exist"));
        }
        );
        it(std::string("handles errors gracefully"), [=]() mutable
        {
            cy->intercept(std::string("GET"), std::string("/api/agents"), object{
                object::pair{std::string("statusCode"), 500}, 
                object::pair{std::string("body"), object{
                    object::pair{std::string("error"), std::string("Server error")}
                }}
            })->as(std::string("getAgentsError"));
            cy->reload();
            cy->get(std::string("#root"), object{
                object::pair{std::string("timeout"), 30000}
            })->should(std::string("exist"));
            cy->document()->its(std::string("readyState"))->should(std::string("equal"), std::string("complete"));
            cy->wait(500);
            cy->wait(std::string("@getAgentsError"));
            cy->get(std::string("#root"))->should(std::string("exist"));
            cy->get(std::string("[data-testid="app-sidebar"]"))->should(std::string("exist"));
        }
        );
        it(std::string("loads basic page structure"), [=]() mutable
        {
            cy->get(std::string("#root"))->should(std::string("exist"));
            cy->get(std::string("[data-testid="app-sidebar"]"))->should(std::string("exist"));
            cy->get(std::string("body"))->should(std::string("not.contain.text"), std::string("Error:"));
            cy->get(std::string("body"))->should(std::string("not.contain.text"), std::string("TypeError:"));
        }
        );
        it(std::string("has working navigation elements"), [=]() mutable
        {
            cy->get(std::string("[data-testid="app-sidebar"]"))->should(std::string("exist"));
            cy->get(std::string("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std::string("[data-testid="sidebar-toggle"]"))["length"] > 0) {
                    cy->get(std::string("[data-testid="sidebar-toggle"]"))->should(std::string("exist"))->click();
                    cy->get(std::string("[data-testid="sidebar-toggle"]"))->click();
                    cy->get(std::string("[data-testid="app-sidebar"]"))->should(std::string("exist"));
                } else {
                    cy->get(std::string("[data-testid="add-agent-button"]"))->should(std::string("exist"));
                    cy->get(std::string("[data-testid="mobile-menu-button"]"))->should(std::string("exist"));
                    cy->log(std::string("Sidebar toggle not available, verified other navigation elements"));
                }
            }
            );
        }
        );
    }
    );
}

MAIN
