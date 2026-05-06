#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/cypress/e2e/02-chat-functionality.cy.h"

void Main(void)
{
    describe(std::string("Chat Functionality"), [=]() mutable
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
        it(std::string("can navigate to chat interface"), [=]() mutable
        {
            cy->get(std::string("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std::string("[data-testid="agent-card"]"))["length"] > 0) {
                    cy->get(std::string("[data-testid="agent-card"]"))->first()->click();
                    cy->url()->should(std::string("not.eq"), string_empty + Cypress->config(std::string("baseUrl")) + std::string("/"));
                } else if ($body["find"](std::string("[data-testid="add-agent-button"]"))["length"] > 0) {
                    cy->get(std::string("[data-testid="add-agent-button"]"))->should(std::string("exist"));
                } else {
                    cy->get(std::string("[data-testid="app-sidebar"]"))->should(std::string("exist"));
                }
            }
            );
        }
        );
        it(std::string("displays basic interface elements"), [=]() mutable
        {
            cy->get(std::string("[data-testid="app-sidebar"]"))->should(std::string("exist"));
            cy->get(std::string("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std::string("[data-testid="sidebar-toggle"]"))["length"] > 0) {
                    cy->get(std::string("[data-testid="sidebar-toggle"]"))->should(std::string("exist"));
                }
            }
            );
            cy->get(std::string("body"))->should(std::string("satisfy"), [=](auto $body) mutable
            {
                return (OR(($body["find"](std::string("[data-testid="agent-card"]"))["length"] > 0), ($body["find"](std::string("[data-testid="add-agent-button"]"))["length"] > 0)));
            }
            );
        }
        );
        it(std::string("can interact with sidebar"), [=]() mutable
        {
            cy->get(std::string("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std::string("[data-testid="sidebar-toggle"]"))["length"] > 0) {
                    cy->get(std::string("[data-testid="sidebar-toggle"]"))->should(std::string("exist"))->click();
                    cy->wait(300);
                    cy->get(std::string("[data-testid="sidebar-toggle"]"))->click();
                } else {
                    cy->get(std::string("[data-testid="mobile-menu-button"]"))->should(std::string("exist"))->click(object{
                        object::pair{std::string("force"), true}
                    });
                    cy->wait(300);
                }
            }
            );
            cy->get(std::string("[data-testid="app-sidebar"]"))->should(std::string("exist"));
        }
        );
        it(std::string("handles API interactions"), [=]() mutable
        {
            cy->intercept(std::string("GET"), std::string("/api/agents"), object{
                object::pair{std::string("body"), object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("agents"), array<object>{ object{
                            object::pair{std::string("id"), std::string("12345678-1234-1234-1234-123456789012")}, 
                            object::pair{std::string("name"), std::string("Test Agent")}, 
                            object::pair{std::string("status"), std::string("active")}
                        } }}
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
            cy->get(std::string("[data-testid="app-sidebar"]"))->should(std::string("exist"));
        }
        );
        it(std::string("handles error states gracefully"), [=]() mutable
        {
            cy->intercept(std::string("GET"), std::string("/api/agents"), object{
                object::pair{std::string("statusCode"), 500}, 
                object::pair{std::string("body"), object{
                    object::pair{std::string("error"), std::string("Internal Server Error")}
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
        it(std::string("supports mobile navigation"), [=]() mutable
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
        it(std::string("loads without critical errors"), [=]() mutable
        {
            cy->get(std::string("body"))->should(std::string("not.contain.text"), std::string("Uncaught"));
            cy->get(std::string("body"))->should(std::string("not.contain.text"), std::string("TypeError"));
            cy->get(std::string("body"))->should(std::string("not.contain.text"), std::string("ReferenceError"));
            cy->get(std::string("#root"))->should(std::string("exist"));
            cy->get(std::string("[data-testid="app-sidebar"]"))->should(std::string("exist"));
        }
        );
        it(std::string("has working connection status"), [=]() mutable
        {
            cy->get(std::string("[data-testid="connection-status"]"), object{
                object::pair{std::string("timeout"), 10000}
            })->should(std::string("exist"));
            cy->get(std::string("[data-testid="connection-status"]"))->click();
            cy->get(std::string("[data-testid="connection-status"]"))->should(std::string("exist"));
        }
        );
        it(std::string("maintains state during navigation"), [=]() mutable
        {
            cy->get(std::string("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std::string("[data-testid="sidebar-toggle"]"))["length"] > 0) {
                    cy->get(std::string("[data-testid="sidebar-toggle"]"))->click();
                }
            }
            );
            cy->get(std::string("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std::string("[data-testid="agent-card"]"))["length"] > 0) {
                    cy->get(std::string("[data-testid="agent-card"]"))->first()->click();
                    cy->wait(1000);
                }
            }
            );
            cy->get(std::string("#root"))->should(std::string("exist"));
        }
        );
        it(std::string("handles concurrent requests"), [=]() mutable
        {
            cy->intercept(std::string("GET"), std::string("/api/agents"), object{
                object::pair{std::string("delay"), 500}, 
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
            cy->get(std::string("[data-testid="app-sidebar"]"))->should(std::string("exist"));
            cy->get(std::string("[data-testid="add-agent-button"]"))->should(std::string("exist"));
            cy->get(std::string("[data-testid="mobile-menu-button"]"))->should(std::string("exist"));
        }
        );
    }
    );
}

MAIN
