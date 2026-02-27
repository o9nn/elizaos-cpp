#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/cypress/e2e/dashboard.cy.h"

void Main(void)
{
    describe(std:("Dashboard E2E Tests"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            cy->visit(std:("/"));
        }
        );
        describe(std:("Dashboard Loading"), [=]() mutable
        {
            it(std:("should load the dashboard successfully"), [=]() mutable
            {
                cy->url()->should(std:("include"), std:("localhost:3000"));
                cy->get(std:("body"))->should(std:("be.visible"));
                cy->get(std:("[data-testid="loading"]"), object{
                    object::pair{std:("timeout"), 5000}
                })->should(std:("not.exist"));
            }
            );
            it(std:("should display the application title or logo"), [=]() mutable
            {
                cy->contains((new RegExp(std:("eliza|agent"))))->should(std:("be.visible"));
            }
            );
            it(std:("should have proper meta tags"), [=]() mutable
            {
                cy->get(std:("meta[name="viewport"]"))->should(std:("exist"))->should(std:("have.attr"), std:("content"))->and(std:("include"), std:("width=device-width"));
            }
            );
        }
        );
        describe(std:("Navigation"), [=]() mutable
        {
            it(std:("should navigate to different sections"), [=]() mutable
            {
                cy->get(std:("nav, [role="navigation"], .navigation"))->should(std:("exist"));
                auto navItems = array<string>{ std:("agents"), std:("chat"), std:("settings"), std:("docs") };
                navItems->forEach([=](auto item) mutable
                {
                    cy->get(std:("a[href*="") + item + std:(""], button:contains("") + item + std:("")"), object{
                        object::pair{std:("timeout"), 2000}
                    })->should(std:("exist"));
                }
                );
            }
            );
            it(std:("should handle navigation clicks"), [=]() mutable
            {
                cy->get(std:("a[href*="agents"], button:contains("agents")"), object{
                    object::pair{std:("timeout"), 2000}
                })->first()->click(object{
                    object::pair{std:("force"), true}
                });
                cy->url()->should(std:("match"), (new RegExp(std:("agents|agent"))));
            }
            );
        }
        );
        describe(std:("Responsive Design"), [=]() mutable
        {
            it(std:("should be responsive on mobile"), [=]() mutable
            {
                cy->viewport(375, 667);
                cy->wait(500);
                cy->get(std:("body"))->should(std:("be.visible"));
                cy->get(std:("nav, [role="navigation"]"))->then([=](auto $nav) mutable
                {
                    if ($nav["is"](std:(":visible"))) {
                        expect($nav)->to->be->visible;
                    } else {
                        cy->get(std:("[aria-label*="menu"], button[class*="menu"]"))->should(std:("be.visible"));
                    }
                }
                );
            }
            );
            it(std:("should be responsive on tablet"), [=]() mutable
            {
                cy->viewport(768, 1024);
                cy->wait(500);
                cy->get(std:("body"))->should(std:("be.visible"));
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should handle 404 pages gracefully"), [=]() mutable
            {
                cy->visit(std:("/non-existent-page"), object{
                    object::pair{std:("failOnStatusCode"), false}
                });
                cy->contains((new RegExp(std:("404|not found|error"))), object{
                    object::pair{std:("timeout"), 5000}
                })->should(std:("be.visible"));
            }
            );
            it(std:("should handle network errors"), [=]() mutable
            {
                cy->intercept(std:("GET"), std:("/api/**"), object{
                    object::pair{std:("forceNetworkError"), true}
                })->as(std:("networkError"));
                cy->visit(std:("/"));
                cy->get(std:("body"))->should(std:("be.visible"));
            }
            );
        }
        );
    }
    );
}

MAIN
