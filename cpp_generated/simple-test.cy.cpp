#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/cypress/e2e/simple-test.cy.h"

void Main(void)
{
    describe(std::string("Simple Test"), [=]() mutable
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
    }
    );
}

MAIN
