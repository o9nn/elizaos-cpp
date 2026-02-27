#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/cypress/e2e/simple-test.cy.h"

void Main(void)
{
    describe(std:("Simple Test"), [=]() mutable
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
    }
    );
}

MAIN
