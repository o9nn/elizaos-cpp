#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/cypress/e2e/working-test.cy.h"

void Main(void)
{
    describe(std::string("Plugin Todo Working Tests"), [=]() mutable
    {
        it(std::string("should load the test page successfully"), [=]() mutable
        {
            cy->visit(std::string("/"));
            cy->get(std::string("body"))->should(std::string("exist"));
            cy->get(std::string("h1"))->should(std::string("contain"), std::string("Plugin Todo Test Server"));
        }
        );
        it(std::string("should test basic interactions"), [=]() mutable
        {
            cy->visit(std::string("/"));
            cy->get(std::string("#test-button"))->should(std::string("be.visible"));
            cy->get(std::string("#test-button"))->click();
            cy->get(std::string("#result"))->should(std::string("contain"), std::string("Button clicked!"));
        }
        );
        it(std::string("should test API endpoints"), [=]() mutable
        {
            cy->request(std::string("/health"))->then([=](auto response) mutable
            {
                expect(response["status"])->to->eq(200);
                expect(response["body"])->to->have->property(std::string("status"), std::string("ok"));
                expect(response["body"])->to->have->property(std::string("service"), std::string("plugin-todo-test"));
            }
            );
        }
        );
        it(std::string("should test todos API"), [=]() mutable
        {
            cy->request(std::string("/api/todos"))->then([=](auto response) mutable
            {
                expect(response["status"])->to->eq(200);
                expect(response["body"])->to->have->property(std::string("success"), true);
                expect(response["body"]["data"])->to->have->property(std::string("todos"));
                expect(response["body"]["data"]["todos"])->to->have->length(2);
                expect(const_(response["body"]["data"]["todos"])[0])->to->have->property(std::string("title"), std::string("Test Todo 1"));
            }
            );
        }
        );
        it(std::string("should handle 404 errors gracefully"), [=]() mutable
        {
            cy->request(object{
                object::pair{std::string("url"), std::string("/nonexistent")}, 
                object::pair{std::string("failOnStatusCode"), false}
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
