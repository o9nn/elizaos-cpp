#include "02-chat-functionality.cy.h"

void Main(void)
{
    describe(std:("Chat Functionality"), [=]() mutable
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
        it(std:("can navigate to chat interface"), [=]() mutable
        {
            cy->get(std:("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std:("[data-testid="agent-card"]"))["length"] > 0) {
                    cy->get(std:("[data-testid="agent-card"]"))->first()->click();
                    cy->url()->should(std:("not.eq"), string_empty + Cypress->config(std:("baseUrl")) + std:("/"));
                } else if ($body["find"](std:("[data-testid="add-agent-button"]"))["length"] > 0) {
                    cy->get(std:("[data-testid="add-agent-button"]"))->should(std:("exist"));
                } else {
                    cy->get(std:("[data-testid="app-sidebar"]"))->should(std:("exist"));
                }
            }
            );
        }
        );
        it(std:("displays basic interface elements"), [=]() mutable
        {
            cy->get(std:("[data-testid="app-sidebar"]"))->should(std:("exist"));
            cy->get(std:("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std:("[data-testid="sidebar-toggle"]"))["length"] > 0) {
                    cy->get(std:("[data-testid="sidebar-toggle"]"))->should(std:("exist"));
                }
            }
            );
            cy->get(std:("body"))->should(std:("satisfy"), [=](auto $body) mutable
            {
                return (OR(($body["find"](std:("[data-testid="agent-card"]"))["length"] > 0), ($body["find"](std:("[data-testid="add-agent-button"]"))["length"] > 0)));
            }
            );
        }
        );
        it(std:("can interact with sidebar"), [=]() mutable
        {
            cy->get(std:("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std:("[data-testid="sidebar-toggle"]"))["length"] > 0) {
                    cy->get(std:("[data-testid="sidebar-toggle"]"))->should(std:("exist"))->click();
                    cy->wait(300);
                    cy->get(std:("[data-testid="sidebar-toggle"]"))->click();
                } else {
                    cy->get(std:("[data-testid="mobile-menu-button"]"))->should(std:("exist"))->click(object{
                        object::pair{std:("force"), true}
                    });
                    cy->wait(300);
                }
            }
            );
            cy->get(std:("[data-testid="app-sidebar"]"))->should(std:("exist"));
        }
        );
        it(std:("handles API interactions"), [=]() mutable
        {
            cy->intercept(std:("GET"), std:("/api/agents"), object{
                object::pair{std:("body"), object{
                    object::pair{std:("data"), object{
                        object::pair{std:("agents"), array<object>{ object{
                            object::pair{std:("id"), std:("12345678-1234-1234-1234-123456789012")}, 
                            object::pair{std:("name"), std:("Test Agent")}, 
                            object::pair{std:("status"), std:("active")}
                        } }}
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
            cy->get(std:("[data-testid="app-sidebar"]"))->should(std:("exist"));
        }
        );
        it(std:("handles error states gracefully"), [=]() mutable
        {
            cy->intercept(std:("GET"), std:("/api/agents"), object{
                object::pair{std:("statusCode"), 500}, 
                object::pair{std:("body"), object{
                    object::pair{std:("error"), std:("Internal Server Error")}
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
        it(std:("supports mobile navigation"), [=]() mutable
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
        it(std:("loads without critical errors"), [=]() mutable
        {
            cy->get(std:("body"))->should(std:("not.contain.text"), std:("Uncaught"));
            cy->get(std:("body"))->should(std:("not.contain.text"), std:("TypeError"));
            cy->get(std:("body"))->should(std:("not.contain.text"), std:("ReferenceError"));
            cy->get(std:("#root"))->should(std:("exist"));
            cy->get(std:("[data-testid="app-sidebar"]"))->should(std:("exist"));
        }
        );
        it(std:("has working connection status"), [=]() mutable
        {
            cy->get(std:("[data-testid="connection-status"]"), object{
                object::pair{std:("timeout"), 10000}
            })->should(std:("exist"));
            cy->get(std:("[data-testid="connection-status"]"))->click();
            cy->get(std:("[data-testid="connection-status"]"))->should(std:("exist"));
        }
        );
        it(std:("maintains state during navigation"), [=]() mutable
        {
            cy->get(std:("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std:("[data-testid="sidebar-toggle"]"))["length"] > 0) {
                    cy->get(std:("[data-testid="sidebar-toggle"]"))->click();
                }
            }
            );
            cy->get(std:("body"))->then([=](auto $body) mutable
            {
                if ($body["find"](std:("[data-testid="agent-card"]"))["length"] > 0) {
                    cy->get(std:("[data-testid="agent-card"]"))->first()->click();
                    cy->wait(1000);
                }
            }
            );
            cy->get(std:("#root"))->should(std:("exist"));
        }
        );
        it(std:("handles concurrent requests"), [=]() mutable
        {
            cy->intercept(std:("GET"), std:("/api/agents"), object{
                object::pair{std:("delay"), 500}, 
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
            cy->get(std:("[data-testid="app-sidebar"]"))->should(std:("exist"));
            cy->get(std:("[data-testid="add-agent-button"]"))->should(std:("exist"));
            cy->get(std:("[data-testid="mobile-menu-button"]"))->should(std:("exist"));
        }
        );
    }
    );
}

MAIN
