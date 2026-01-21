#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/cypress/e2e/dashboard.cy.h"

void Main(void)
{
    describe(std::string("Dashboard E2E Tests"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            cy->visit(std::string("/"));
        }
        );
        describe(std::string("Dashboard Loading"), [=]() mutable
        {
            it(std::string("should load the dashboard successfully"), [=]() mutable
            {
                cy->url()->should(std::string("include"), std::string("localhost:3000"));
                cy->get(std::string("body"))->should(std::string("be.visible"));
                cy->get(std::string("[data-testid="loading"]"), object{
                    object::pair{std::string("timeout"), 5000}
                })->should(std::string("not.exist"));
            }
            );
            it(std::string("should display the application title or logo"), [=]() mutable
            {
                cy->contains((new RegExp(std::string("eliza|agent"))))->should(std::string("be.visible"));
            }
            );
            it(std::string("should have proper meta tags"), [=]() mutable
            {
                cy->get(std::string("meta[name="viewport"]"))->should(std::string("exist"))->should(std::string("have.attr"), std::string("content"))->and(std::string("include"), std::string("width=device-width"));
            }
            );
        }
        );
        describe(std::string("Navigation"), [=]() mutable
        {
            it(std::string("should navigate to different sections"), [=]() mutable
            {
                cy->get(std::string("nav, [role="navigation"], .navigation"))->should(std::string("exist"));
                auto navItems = array<string>{ std::string("agents"), std::string("chat"), std::string("settings"), std::string("docs") };
                navItems->forEach([=](auto item) mutable
                {
                    cy->get(std::string("a[href*="") + item + std::string(""], button:contains("") + item + std::string("")"), object{
                        object::pair{std::string("timeout"), 2000}
                    })->should(std::string("exist"));
                }
                );
            }
            );
            it(std::string("should handle navigation clicks"), [=]() mutable
            {
                cy->get(std::string("a[href*="agents"], button:contains("agents")"), object{
                    object::pair{std::string("timeout"), 2000}
                })->first()->click(object{
                    object::pair{std::string("force"), true}
                });
                cy->url()->should(std::string("match"), (new RegExp(std::string("agents|agent"))));
            }
            );
        }
        );
        describe(std::string("Responsive Design"), [=]() mutable
        {
            it(std::string("should be responsive on mobile"), [=]() mutable
            {
                cy->viewport(375, 667);
                cy->wait(500);
                cy->get(std::string("body"))->should(std::string("be.visible"));
                cy->get(std::string("nav, [role="navigation"]"))->then([=](auto $nav) mutable
                {
                    if ($nav["is"](std::string(":visible"))) {
                        expect($nav)->to->be->visible;
                    } else {
                        cy->get(std::string("[aria-label*="menu"], button[class*="menu"]"))->should(std::string("be.visible"));
                    }
                }
                );
            }
            );
            it(std::string("should be responsive on tablet"), [=]() mutable
            {
                cy->viewport(768, 1024);
                cy->wait(500);
                cy->get(std::string("body"))->should(std::string("be.visible"));
            }
            );
        }
        );
        describe(std::string("Error Handling"), [=]() mutable
        {
            it(std::string("should handle 404 pages gracefully"), [=]() mutable
            {
                cy->visit(std::string("/non-existent-page"), object{
                    object::pair{std::string("failOnStatusCode"), false}
                });
                cy->contains((new RegExp(std::string("404|not found|error"))), object{
                    object::pair{std::string("timeout"), 5000}
                })->should(std::string("be.visible"));
            }
            );
            it(std::string("should handle network errors"), [=]() mutable
            {
                cy->intercept(std::string("GET"), std::string("/api/**"), object{
                    object::pair{std::string("forceNetworkError"), true}
                })->as(std::string("networkError"));
                cy->visit(std::string("/"));
                cy->get(std::string("body"))->should(std::string("be.visible"));
            }
            );
        }
        );
    }
    );
}

MAIN
