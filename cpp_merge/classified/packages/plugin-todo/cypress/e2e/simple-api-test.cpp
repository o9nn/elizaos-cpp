#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/cypress/e2e/simple-api-test.cy.h"

void Main(void)
{
    describe(std:("Todo Plugin API Tests"), [=]() mutable
    {
        shared baseUrl = OR((Cypress->config(std:("baseUrl"))), (std:("http://localhost:7777")));
        describe(std:("Basic Server Tests"), [=]() mutable
        {
            it(std:("should have the server running"), [=]() mutable
            {
                cy->request(object{
                    object::pair{std:("url"), string_empty + baseUrl + std:("/")}, 
                    object::pair{std:("failOnStatusCode"), false}
                })->then([=](auto response) mutable
                {
                    expect(response["status"])->to->be->oneOf(array<double>{ 200, 404 });
                }
                );
            }
            );
            it(std:("should check if plugin routes are available"), [=]() mutable
            {
                cy->request(object{
                    object::pair{std:("method"), std:("GET")}, 
                    object::pair{std:("url"), string_empty + baseUrl + std:("/api/todos")}, 
                    object::pair{std:("failOnStatusCode"), false}
                })->then([=](auto response) mutable
                {
                    if (response["status"] == 200) {
                        expect(response["body"])->to->be->an(std:("array"));
                        if (response["body"]["length"] > 0) {
                            auto world = const_(response["body"])[0];
                            expect(world)->to->have->property(std:("worldId"));
                            expect(world)->to->have->property(std:("worldName"));
                            expect(world)->to->have->property(std:("rooms"));
                            expect(world["rooms"])->to->be->an(std:("array"));
                        }
                    } else {
                        cy->log(std:("Todo API endpoint not available in dev mode"));
                    }
                }
                );
            }
            );
            it(std:("should check if tags endpoint is available"), [=]() mutable
            {
                cy->request(object{
                    object::pair{std:("method"), std:("GET")}, 
                    object::pair{std:("url"), string_empty + baseUrl + std:("/api/tags")}, 
                    object::pair{std:("failOnStatusCode"), false}
                })->then([=](auto response) mutable
                {
                    if (response["status"] == 200) {
                        expect(response["body"])->to->be->an(std:("array"));
                    } else {
                        cy->log(std:("Tags API endpoint not available in dev mode"));
                    }
                }
                );
            }
            );
        }
        );
        describe(std:("Frontend Routes"), [=]() mutable
        {
            it(std:("should serve a page at /"), [=]() mutable
            {
                cy->visit(std:("/"), object{
                    object::pair{std:("failOnStatusCode"), false}
                });
                cy->get(std:("body"))->should(std:("exist"));
            }
            );
            it(std:("should check todos page availability"), [=]() mutable
            {
                cy->request(object{
                    object::pair{std:("url"), std:("/todos")}, 
                    object::pair{std:("failOnStatusCode"), false}
                })->then([=](auto response) mutable
                {
                    cy->log(std:("Todos page status: ") + response["status"] + string_empty);
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
