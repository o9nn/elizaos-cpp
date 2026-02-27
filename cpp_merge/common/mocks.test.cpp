#include "mocks.test.h"

void Main(void)
{
    describe(std:("Mock Factory Functions"), [=]() mutable
    {
        describe(std:("createMockAgentRuntime"), [=]() mutable
        {
            it(std:("should create a mock runtime with all required properties"), [=]() mutable
            {
                auto runtime = createMockAgentRuntime();
                expect(runtime->agentId)->toBeDefined();
                expect(runtime->character)->toBeDefined();
                expect(runtime->providers)->toEqual(array<any>());
                expect(runtime->actions)->toEqual(array<any>());
                expect(runtime->evaluators)->toEqual(array<any>());
                expect(runtime->plugins)->toEqual(array<any>());
                expect(runtime->services)->toBeInstanceOf(Map);
                expect(runtime->events)->toBeInstanceOf(Map);
                expect(runtime->routes)->toEqual(array<any>());
                expect(type_of(runtime->getSetting))->toBe(std:("function"));
                expect(type_of(runtime->registerPlugin))->toBe(std:("function"));
                expect(type_of(runtime->initialize))->toBe(std:("function"));
                expect(type_of(runtime->composeState))->toBe(std:("function"));
            }
            );
            it(std:("should allow overriding properties"), [=]() mutable
            {
                auto customCharacter = object{
                    object::pair{std:("id"), std:("custom-id")}, 
                    object::pair{std:("name"), std:("Custom Character")}, 
                    object::pair{std:("description"), std:("Custom description")}, 
                    object::pair{std:("bio"), array<string>{ std:("Custom bio") }}, 
                    object::pair{std:("system"), std:("Custom system")}, 
                    object::pair{std:("modelProvider"), std:("custom")}, 
                    object::pair{std:("settings"), object{
                        object::pair{std:("model"), std:("custom-model")}, 
                        object::pair{std:("customSetting"), std:("value")}
                    }}
                };
                auto runtime = createMockAgentRuntime(object{
                    object::pair{std:("character"), as<any>(customCharacter)}
                });
                expect(runtime->character->name)->toBe(std:("Custom Character"));
                expect(runtime->getSetting(std:("customSetting")))->toBe(std:("value"));
            }
            );
        }
        );
        describe(std:("createMockDatabaseAdapter"), [=]() mutable
        {
            it(std:("should create a mock database adapter with all required methods"), [=]() mutable
            {
                auto adapter = createMockDatabaseAdapter();
                expect(type_of(adapter["init"]))->toBe(std:("function"));
                expect(type_of(adapter["close"]))->toBe(std:("function"));
                expect(type_of(adapter["getAgent"]))->toBe(std:("function"));
                expect(type_of(adapter["createMemory"]))->toBe(std:("function"));
                expect(type_of(adapter["searchMemories"]))->toBe(std:("function"));
                expect(type_of(adapter["createMessageServer"]))->toBe(std:("function"));
                expect(type_of(adapter["getMessageServers"]))->toBe(std:("function"));
                expect(type_of(adapter["createChannel"]))->toBe(std:("function"));
                expect(type_of(adapter["findOrCreateDmChannel"]))->toBe(std:("function"));
            }
            );
        }
        );
        describe(std:("createMockRequest"), [=]() mutable
        {
            it(std:("should create a mock Express request"), [=]() mutable
            {
                auto req = createMockRequest();
                expect(req->params)->toEqual(object{});
                expect(req->query)->toEqual(object{});
                expect(req->body)->toEqual(object{});
                expect(req->method)->toBe(std:("GET"));
                expect(type_of(req->get))->toBe(std:("function"));
            }
            );
            it(std:("should allow overriding request properties"), [=]() mutable
            {
                auto req = createMockRequest(object{
                    object::pair{std:("method"), std:("POST")}, 
                    object::pair{std:("body"), object{
                        object::pair{std:("test"), std:("data")}
                    }}, 
                    object::pair{std:("params"), object{
                        object::pair{std:("id"), std:("123")}
                    }}
                });
                expect(req->method)->toBe(std:("POST"));
                expect(req->body)->toEqual(object{
                    object::pair{std:("test"), std:("data")}
                });
                expect(req->params)->toEqual(object{
                    object::pair{std:("id"), std:("123")}
                });
            }
            );
        }
        );
        describe(std:("createMockResponse"), [=]() mutable
        {
            it(std:("should create a mock Express response with chainable methods"), [=]() mutable
            {
                auto res = createMockResponse();
                expect(type_of(res->status))->toBe(std:("function"));
                expect(type_of(res->json))->toBe(std:("function"));
                expect(type_of(res->send))->toBe(std:("function"));
                auto result = res->status(200)->json(object{
                    object::pair{std:("success"), true}
                });
                expect(result)->toBe(res);
            }
            );
        }
        );
        describe(std:("createMockSocketIO"), [=]() mutable
        {
            it(std:("should create a mock Socket.IO server"), [=]() mutable
            {
                auto io = createMockSocketIO();
                expect(type_of(io["on"]))->toBe(std:("function"));
                expect(type_of(io["emit"]))->toBe(std:("function"));
                expect(type_of(io["to"]))->toBe(std:("function"));
                expect(io["sockets"]["sockets"])->toBeInstanceOf(Map);
            }
            );
        }
        );
        describe(std:("createMockHttpServer"), [=]() mutable
        {
            it(std:("should create a mock HTTP server"), [=]() mutable
            {
                auto server = createMockHttpServer();
                expect(type_of(server["listen"]))->toBe(std:("function"));
                expect(type_of(server["close"]))->toBe(std:("function"));
                expect(type_of(server["address"]))->toBe(std:("function"));
                auto address = server["address"]();
                expect(address)->toEqual(object{
                    object::pair{std:("port"), 3000}
                });
            }
            );
        }
        );
        describe(std:("createMockService"), [=]() mutable
        {
            it(std:("should create a mock service"), [=]() mutable
            {
                auto service = createMockService();
                expect(service)->toBeDefined();
                expect(service)->toHaveProperty(std:("name"), std:("MockService"));
                expect(service)->toHaveProperty(std:("serviceType"), ServiceType->WEB_SEARCH);
                expect(service)->toHaveProperty(std:("start"));
                expect(service)->toHaveProperty(std:("stop"));
            }
            );
        }
        );
        describe(std:("createMockUploadedFile"), [=]() mutable
        {
            it(std:("should create a mock multer file"), [=]() mutable
            {
                auto file = createMockUploadedFile();
                expect(file->originalname)->toBe(std:("test.jpg"));
                expect(file->mimetype)->toBe(std:("image/jpeg"));
                expect(file->size)->toBe(12345);
                expect(file->buffer)->toBeInstanceOf(Buffer);
                expect(file->fieldname)->toBe(std:("file"));
            }
            );
            it(std:("should allow overriding file properties"), [=]() mutable
            {
                auto file = createMockUploadedFile(object{
                    object::pair{std:("originalname"), std:("custom.png")}, 
                    object::pair{std:("mimetype"), std:("image/png")}, 
                    object::pair{std:("size"), 54321}
                });
                expect(file->originalname)->toBe(std:("custom.png"));
                expect(file->mimetype)->toBe(std:("image/png"));
                expect(file->size)->toBe(54321);
            }
            );
        }
        );
    }
    );
}

MAIN
