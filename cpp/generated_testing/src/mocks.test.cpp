#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/__tests__/test-utils/mocks.test.h"

void Main(void)
{
    describe(std::string("Mock Factory Functions"), [=]() mutable
    {
        describe(std::string("createMockAgentRuntime"), [=]() mutable
        {
            it(std::string("should create a mock runtime with all required properties"), [=]() mutable
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
                expect(type_of(runtime->getSetting))->toBe(std::string("function"));
                expect(type_of(runtime->registerPlugin))->toBe(std::string("function"));
                expect(type_of(runtime->initialize))->toBe(std::string("function"));
                expect(type_of(runtime->composeState))->toBe(std::string("function"));
            }
            );
            it(std::string("should allow overriding properties"), [=]() mutable
            {
                auto customCharacter = object{
                    object::pair{std::string("id"), std::string("custom-id")}, 
                    object::pair{std::string("name"), std::string("Custom Character")}, 
                    object::pair{std::string("description"), std::string("Custom description")}, 
                    object::pair{std::string("bio"), array<string>{ std::string("Custom bio") }}, 
                    object::pair{std::string("system"), std::string("Custom system")}, 
                    object::pair{std::string("modelProvider"), std::string("custom")}, 
                    object::pair{std::string("settings"), object{
                        object::pair{std::string("model"), std::string("custom-model")}, 
                        object::pair{std::string("customSetting"), std::string("value")}
                    }}
                };
                auto runtime = createMockAgentRuntime(object{
                    object::pair{std::string("character"), as<any>(customCharacter)}
                });
                expect(runtime->character->name)->toBe(std::string("Custom Character"));
                expect(runtime->getSetting(std::string("customSetting")))->toBe(std::string("value"));
            }
            );
        }
        );
        describe(std::string("createMockDatabaseAdapter"), [=]() mutable
        {
            it(std::string("should create a mock database adapter with all required methods"), [=]() mutable
            {
                auto adapter = createMockDatabaseAdapter();
                expect(type_of(adapter["init"]))->toBe(std::string("function"));
                expect(type_of(adapter["close"]))->toBe(std::string("function"));
                expect(type_of(adapter["getAgent"]))->toBe(std::string("function"));
                expect(type_of(adapter["createMemory"]))->toBe(std::string("function"));
                expect(type_of(adapter["searchMemories"]))->toBe(std::string("function"));
                expect(type_of(adapter["createMessageServer"]))->toBe(std::string("function"));
                expect(type_of(adapter["getMessageServers"]))->toBe(std::string("function"));
                expect(type_of(adapter["createChannel"]))->toBe(std::string("function"));
                expect(type_of(adapter["findOrCreateDmChannel"]))->toBe(std::string("function"));
            }
            );
        }
        );
        describe(std::string("createMockRequest"), [=]() mutable
        {
            it(std::string("should create a mock Express request"), [=]() mutable
            {
                auto req = createMockRequest();
                expect(req->params)->toEqual(object{});
                expect(req->query)->toEqual(object{});
                expect(req->body)->toEqual(object{});
                expect(req->method)->toBe(std::string("GET"));
                expect(type_of(req->get))->toBe(std::string("function"));
            }
            );
            it(std::string("should allow overriding request properties"), [=]() mutable
            {
                auto req = createMockRequest(object{
                    object::pair{std::string("method"), std::string("POST")}, 
                    object::pair{std::string("body"), object{
                        object::pair{std::string("test"), std::string("data")}
                    }}, 
                    object::pair{std::string("params"), object{
                        object::pair{std::string("id"), std::string("123")}
                    }}
                });
                expect(req->method)->toBe(std::string("POST"));
                expect(req->body)->toEqual(object{
                    object::pair{std::string("test"), std::string("data")}
                });
                expect(req->params)->toEqual(object{
                    object::pair{std::string("id"), std::string("123")}
                });
            }
            );
        }
        );
        describe(std::string("createMockResponse"), [=]() mutable
        {
            it(std::string("should create a mock Express response with chainable methods"), [=]() mutable
            {
                auto res = createMockResponse();
                expect(type_of(res->status))->toBe(std::string("function"));
                expect(type_of(res->json))->toBe(std::string("function"));
                expect(type_of(res->send))->toBe(std::string("function"));
                auto result = res->status(200)->json(object{
                    object::pair{std::string("success"), true}
                });
                expect(result)->toBe(res);
            }
            );
        }
        );
        describe(std::string("createMockSocketIO"), [=]() mutable
        {
            it(std::string("should create a mock Socket.IO server"), [=]() mutable
            {
                auto io = createMockSocketIO();
                expect(type_of(io["on"]))->toBe(std::string("function"));
                expect(type_of(io["emit"]))->toBe(std::string("function"));
                expect(type_of(io["to"]))->toBe(std::string("function"));
                expect(io["sockets"]["sockets"])->toBeInstanceOf(Map);
            }
            );
        }
        );
        describe(std::string("createMockHttpServer"), [=]() mutable
        {
            it(std::string("should create a mock HTTP server"), [=]() mutable
            {
                auto server = createMockHttpServer();
                expect(type_of(server["listen"]))->toBe(std::string("function"));
                expect(type_of(server["close"]))->toBe(std::string("function"));
                expect(type_of(server["address"]))->toBe(std::string("function"));
                auto address = server["address"]();
                expect(address)->toEqual(object{
                    object::pair{std::string("port"), 3000}
                });
            }
            );
        }
        );
        describe(std::string("createMockService"), [=]() mutable
        {
            it(std::string("should create a mock service"), [=]() mutable
            {
                auto service = createMockService();
                expect(service)->toBeDefined();
                expect(service)->toHaveProperty(std::string("name"), std::string("MockService"));
                expect(service)->toHaveProperty(std::string("serviceType"), ServiceType->WEB_SEARCH);
                expect(service)->toHaveProperty(std::string("start"));
                expect(service)->toHaveProperty(std::string("stop"));
            }
            );
        }
        );
        describe(std::string("createMockUploadedFile"), [=]() mutable
        {
            it(std::string("should create a mock multer file"), [=]() mutable
            {
                auto file = createMockUploadedFile();
                expect(file->originalname)->toBe(std::string("test.jpg"));
                expect(file->mimetype)->toBe(std::string("image/jpeg"));
                expect(file->size)->toBe(12345);
                expect(file->buffer)->toBeInstanceOf(Buffer);
                expect(file->fieldname)->toBe(std::string("file"));
            }
            );
            it(std::string("should allow overriding file properties"), [=]() mutable
            {
                auto file = createMockUploadedFile(object{
                    object::pair{std::string("originalname"), std::string("custom.png")}, 
                    object::pair{std::string("mimetype"), std::string("image/png")}, 
                    object::pair{std::string("size"), 54321}
                });
                expect(file->originalname)->toBe(std::string("custom.png"));
                expect(file->mimetype)->toBe(std::string("image/png"));
                expect(file->size)->toBe(54321);
            }
            );
        }
        );
    }
    );
}

MAIN
