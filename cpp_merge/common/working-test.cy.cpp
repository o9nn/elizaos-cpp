#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/cypress/e2e/working-test.cy.h"

void Main(void)
{
    describe(std:("Plugin Todo Working Tests"), [=]() mutable
    {
        it(std:("should load the test page successfully"), [=]() mutable
        {
            cy->visit(std:("/"));
            cy->get(std:("body"))->should(std:("exist"));
            cy->get(std:("h1"))->should(std:("contain"), std:("Plugin Todo Test Server"));
        }
        );
        it(std:("should test basic interactions"), [=]() mutable
        {
            cy->visit(std:("/"));
            cy->get(std:("#test-button"))->should(std:("be.visible"));
            cy->get(std:("#test-button"))->click();
            cy->get(std:("#result"))->should(std:("contain"), std:("Button clicked!"));
        }
        );
        it(std:("should test API endpoints"), [=]() mutable
        {
            cy->request(std:("/health"))->then([=](auto response) mutable
            {
                expect(response["status"])->to->eq(200);
                expect(response["body"])->to->have->property(std:("status"), std:("ok"));
                expect(response["body"])->to->have->property(std:("service"), std:("plugin-todo-test"));
            }
            );
        }
        );
        it(std:("should test todos API"), [=]() mutable
        {
            cy->request(std:("/api/todos"))->then([=](auto response) mutable
            {
                expect(response["status"])->to->eq(200);
                expect(response["body"])->to->have->property(std:("success"), true);
                expect(response["body"]["data"])->to->have->property(std:("todos"));
                expect(response["body"]["data"]["todos"])->to->have->length(2);
                expect(const_(response["body"]["data"]["todos"])[0])->to->have->property(std:("title"), std:("Test Todo 1"));
            }
            );
        }
        );
        it(std:("should handle 404 errors gracefully"), [=]() mutable
        {
            cy->request(object{
                object::pair{std:("url"), std:("/nonexistent")}, 
                object::pair{std:("failOnStatusCode"), false}
            })->then([=](auto response) mutable
            {
                expect(response["status"])->to->eq(404);
            }
            );
        }
        );
    }
    );
}

MAIN
