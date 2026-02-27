#include "hello-world.cy.h"

void Main(void)
{
    describe(std:("Plugin Starter Hello World"), [=]() mutable
    {
        it(std:("should verify the server is running"), [=]() mutable
        {
            cy->visit(std:("/"), object{
                object::pair{std:("failOnStatusCode"), false}
            });
            cy->get(std:("body"))->should(std:("exist"));
        }
        );
        it(std:("should check server response"), [=]() mutable
        {
            cy->request(object{
                object::pair{std:("url"), std:("/")}, 
                object::pair{std:("failOnStatusCode"), false}
            })->then([=](auto response) mutable
            {
                expect(response)->to->have->property(std:("status"));
                cy->log(std:("Server responded with status: ") + response["status"] + string_empty);
            }
            );
        }
        );
        it(std:("should verify API endpoint"), [=]() mutable
        {
            cy->request(object{
                object::pair{std:("method"), std:("GET")}, 
                object::pair{std:("url"), std:("/api/health")}, 
                object::pair{std:("failOnStatusCode"), false}
            })->then([=](auto response) mutable
            {
                cy->log(std:("API health check status: ") + response["status"] + string_empty);
                expect(response["status"])->to->be->oneOf(array<double>{ 200, 404 });
            }
            );
        }
        );
    }
    );
}

MAIN
