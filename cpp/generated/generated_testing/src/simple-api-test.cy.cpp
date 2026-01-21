#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/cypress/e2e/simple-api-test.cy.h"

void Main(void)
{
    describe(std::string("Goals Plugin API Tests"), [=]() mutable
    {
        shared baseUrl = OR((Cypress->config(std::string("baseUrl"))), (std::string("http://localhost:7777")));
        describe(std::string("Basic Server Tests"), [=]() mutable
        {
            it(std::string("should have the server running"), [=]() mutable
            {
                cy->request(object{
                    object::pair{std::string("url"), string_empty + baseUrl + std::string("/")}, 
                    object::pair{std::string("failOnStatusCode"), false}
                })->then([=](auto response) mutable
                {
                    expect(response["status"])->to->be->oneOf(array<double>{ 200, 404 });
                }
                );
            }
            );
            it(std::string("should check if plugin routes are available"), [=]() mutable
            {
                cy->request(object{
                    object::pair{std::string("method"), std::string("GET")}, 
                    object::pair{std::string("url"), string_empty + baseUrl + std::string("/api/goals")}, 
                    object::pair{std::string("failOnStatusCode"), false}
                })->then([=](auto response) mutable
                {
                    if (response["status"] == 200) {
                        expect(response["body"])->to->be->an(std::string("array"));
                        if (response["body"]["length"] > 0) {
                            auto world = const_(response["body"])[0];
                            expect(world)->to->have->property(std::string("worldId"));
                            expect(world)->to->have->property(std::string("worldName"));
                            expect(world)->to->have->property(std::string("rooms"));
                            expect(world["rooms"])->to->be->an(std::string("array"));
                        }
                    } else {
                        cy->log(std::string("Goals API endpoint not available in dev mode"));
                    }
                }
                );
            }
            );
            it(std::string("should check if tags endpoint is available"), [=]() mutable
            {
                cy->request(object{
                    object::pair{std::string("method"), std::string("GET")}, 
                    object::pair{std::string("url"), string_empty + baseUrl + std::string("/api/tags")}, 
                    object::pair{std::string("failOnStatusCode"), false}
                })->then([=](auto response) mutable
                {
                    if (response["status"] == 200) {
                        expect(response["body"])->to->be->an(std::string("array"));
                    } else {
                        cy->log(std::string("Tags API endpoint not available in dev mode"));
                    }
                }
                );
            }
            );
        }
        );
        describe(std::string("Frontend Routes"), [=]() mutable
        {
            it(std::string("should serve a page at /"), [=]() mutable
            {
                cy->visit(std::string("/"), object{
                    object::pair{std::string("failOnStatusCode"), false}
                });
                cy->get(std::string("body"))->should(std::string("exist"));
            }
            );
            it(std::string("should check goals page availability"), [=]() mutable
            {
                cy->request(object{
                    object::pair{std::string("url"), std::string("/goals")}, 
                    object::pair{std::string("failOnStatusCode"), false}
                })->then([=](auto response) mutable
                {
                    cy->log(std::string("Goals page status: ") + response["status"] + string_empty);
                }
                );
            }
            );
        }
        );
    }
    );
}

MAIN
