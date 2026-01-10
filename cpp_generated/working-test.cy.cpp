#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/cypress/e2e/working-test.cy.h"

void Main(void)
{
    describe(std::string("Plugin Goals Working Tests"), [=]() mutable
    {
        shared testAgentId = std::string("b438180f-bcb4-0e28-8cb1-ec0264051e59");
        it(std::string("should load the goals test page successfully"), [=]() mutable
        {
            cy->visit(std::string("/"));
            cy->get(std::string("body"))->should(std::string("exist"));
            cy->get(std::string("h1"))->should(std::string("contain"), std::string("Plugin Goals Test Server"));
        }
        );
        it(std::string("should test goal creation interactions"), [=]() mutable
        {
            cy->visit(std::string("/"));
            cy->get(std::string("#create-goal"))->should(std::string("be.visible"));
            cy->get(std::string("#create-goal"))->click();
            cy->get(std::string("#goals-list"))->should(std::string("contain"), std::string("Goal created successfully!"));
        }
        );
        it(std::string("should test API health endpoint"), [=]() mutable
        {
            cy->request(std::string("/api/health"))->then([=](auto response) mutable
            {
                expect(response["status"])->to->eq(200);
                expect(response["body"])->to->have->property(std::string("status"), std::string("ok"));
                expect(response["body"])->to->have->property(std::string("service"), std::string("plugin-goals-test"));
            }
            );
        }
        );
        it(std::string("should test goals endpoint"), [=]() mutable
        {
            cy->request(std::string("/api/goals?agentId=") + testAgentId + string_empty)->then([=](auto response) mutable
            {
                expect(response["status"])->to->eq(200);
                expect(response["body"])->to->have->property(std::string("success"), true);
                expect(response["body"]["data"])->to->have->property(std::string("goals"));
                expect(response["body"]["data"]["goals"])->to->have->length(2);
                expect(const_(response["body"]["data"]["goals"])[0])->to->have->property(std::string("title"), std::string("Complete project"));
                expect(const_(response["body"]["data"]["goals"])[1])->to->have->property(std::string("status"), std::string("completed"));
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
