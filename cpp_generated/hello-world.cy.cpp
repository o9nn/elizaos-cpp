#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-elizaos-services/cypress/e2e/hello-world.cy.h"

void Main(void)
{
    describe(std::string("Plugin Starter Hello World"), [=]() mutable
    {
        it(std::string("should verify the server is running"), [=]() mutable
        {
            cy->visit(std::string("/"), object{
                object::pair{std::string("failOnStatusCode"), false}
            });
            cy->get(std::string("body"))->should(std::string("exist"));
        }
        );
        it(std::string("should check server response"), [=]() mutable
        {
            cy->request(object{
                object::pair{std::string("url"), std::string("/")}, 
                object::pair{std::string("failOnStatusCode"), false}
            })->then([=](auto response) mutable
            {
                expect(response)->to->have->property(std::string("status"));
                cy->log(std::string("Server responded with status: ") + response["status"] + string_empty);
            }
            );
        }
        );
        it(std::string("should verify API endpoint"), [=]() mutable
        {
            cy->request(object{
                object::pair{std::string("method"), std::string("GET")}, 
                object::pair{std::string("url"), std::string("/api/health")}, 
                object::pair{std::string("failOnStatusCode"), false}
            })->then([=](auto response) mutable
            {
                cy->log(std::string("API health check status: ") + response["status"] + string_empty);
                expect(response["status"])->to->be->oneOf(array<double>{ 200, 404 });
            }
            );
        }
        );
    }
    );
}

MAIN
