#include "working-test.cy.h"

void Main(void)
{
    describe(std:("Plugin Goals Working Tests"), [=]() mutable
    {
        shared testAgentId = std:("b438180f-bcb4-0e28-8cb1-ec0264051e59");
        it(std:("should load the goals test page successfully"), [=]() mutable
        {
            cy->visit(std:("/"));
            cy->get(std:("body"))->should(std:("exist"));
            cy->get(std:("h1"))->should(std:("contain"), std:("Plugin Goals Test Server"));
        }
        );
        it(std:("should test goal creation interactions"), [=]() mutable
        {
            cy->visit(std:("/"));
            cy->get(std:("#create-goal"))->should(std:("be.visible"));
            cy->get(std:("#create-goal"))->click();
            cy->get(std:("#goals-list"))->should(std:("contain"), std:("Goal created successfully!"));
        }
        );
        it(std:("should test API health endpoint"), [=]() mutable
        {
            cy->request(std:("/api/health"))->then([=](auto response) mutable
            {
                expect(response["status"])->to->eq(200);
                expect(response["body"])->to->have->property(std:("status"), std:("ok"));
                expect(response["body"])->to->have->property(std:("service"), std:("plugin-goals-test"));
            }
            );
        }
        );
        it(std:("should test goals endpoint"), [=]() mutable
        {
            cy->request(std:("/api/goals?agentId=") + testAgentId + string_empty)->then([=](auto response) mutable
            {
                expect(response["status"])->to->eq(200);
                expect(response["body"])->to->have->property(std:("success"), true);
                expect(response["body"]["data"])->to->have->property(std:("goals"));
                expect(response["body"]["data"]["goals"])->to->have->length(2);
                expect(const_(response["body"]["data"]["goals"])[0])->to->have->property(std:("title"), std:("Complete project"));
                expect(const_(response["body"]["data"]["goals"])[1])->to->have->property(std:("status"), std:("completed"));
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
