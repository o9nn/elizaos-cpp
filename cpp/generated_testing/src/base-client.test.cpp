#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/__tests__/base-client.test.h"

TestClient::TestClient(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}


void Main(void)
{
    describe(std::string("BaseApiClient"), [=]() mutable
    {
        shared<std::shared_ptr<TestClient>> client;
        shared mockConfig = object{
            object::pair{std::string("baseUrl"), std::string("http://localhost:3000")}, 
            object::pair{std::string("apiKey"), std::string("test-key")}, 
            object::pair{std::string("timeout"), 5000}
        };
        shared<any> fetchMock;
        beforeEach([=]() mutable
        {
            client = std::make_shared<TestClient>(mockConfig);
            fetchMock = global->fetch;
        }
        );
        it(std::string("should initialize with correct config"), [=]() mutable
        {
            expect(const_(client)[std::string("baseUrl")])->toBe(std::string("http://localhost:3000"));
            expect(const_(client)[std::string("apiKey")])->toBe(std::string("test-key"));
            expect(const_(client)[std::string("timeout")])->toBe(5000);
            expect(const_(const_(client)[std::string("defaultHeaders")])[std::string("X-API-KEY")])->toBe(std::string("test-key"));
        }
        );
        it(std::string("should remove trailing slash from baseUrl"), [=]() mutable
        {
            auto clientWithSlash = std::make_shared<TestClient>(utils::assign(object{
                , 
                object::pair{std::string("baseUrl"), std::string("http://localhost:3000/")}
            }, mockConfig));
            expect((*const_(clientWithSlash))[std::string("baseUrl")])->toBe(std::string("http://localhost:3000"));
        }
        );
        it(std::string("should make successful GET request"), [=]() mutable
        {
            shared mockResponse = object{
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("message"), std::string("Hello World")}
                }}
            };
            global->fetch = [=](auto url, auto options) mutable
            {
                expect(url)->toBe(std::string("http://localhost:3000/api/test"));
                expect(options["method"])->toBe(std::string("GET"));
                expect(const_(options["headers"])[std::string("X-API-KEY")])->toBe(std::string("test-key"));
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("get"), [=](auto name) mutable
                        {
                            return ((name == std::string("content-length")) ? any(std::string("100")) : any(nullptr));
                        }
                        }
                    }}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        return mockResponse;
                    }
                    }
                });
            };
            auto result = std::async([=]() { client->testGet(std::string("/api/test")); });
            expect(result)->toEqual(mockResponse["data"]);
        }
        );
        it(std::string("should make successful POST request"), [=]() mutable
        {
            shared body = object{
                object::pair{std::string("name"), std::string("Test Item")}
            };
            shared mockResponse = object{
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), utils::assign(object{
                    object::pair{std::string("id"), std::string("123")}
                }, body)}
            };
            global->fetch = [=](auto url, auto options) mutable
            {
                expect(url)->toBe(std::string("http://localhost:3000/api/test"));
                expect(options["method"])->toBe(std::string("POST"));
                expect(options["body"])->toBe(JSON->stringify(body));
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("get"), [=](auto name) mutable
                        {
                            return ((name == std::string("content-length")) ? any(std::string("100")) : any(nullptr));
                        }
                        }
                    }}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        return mockResponse;
                    }
                    }
                });
            };
            auto result = std::async([=]() { client->testPost(std::string("/api/test"), body); });
            expect(result)->toEqual(mockResponse["data"]);
        }
        );
        it(std::string("should handle FormData without Content-Type header"), [=]() mutable
        {
            shared formData = std::make_shared<FormData>();
            formData->append(std::string("file"), std::string("test"));
            global->fetch = [=](auto url, auto options) mutable
            {
                expect(const_(options["headers"])[std::string("Content-Type")])->toBeUndefined();
                expect(options["body"])->toBe(formData);
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("get"), [=](auto name) mutable
                        {
                            return ((name == std::string("content-length")) ? any(std::string("100")) : any(nullptr));
                        }
                        }
                    }}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        return (object{
                            object::pair{std::string("success"), true}, 
                            object::pair{std::string("data"), object{
                                object::pair{std::string("uploaded"), true}
                            }}
                        });
                    }
                    }
                });
            };
            auto result = std::async([=]() { client->testPost(std::string("/api/upload"), formData); });
            expect(result)->toEqual(object{
                object::pair{std::string("uploaded"), true}
            });
        }
        );
        it(std::string("should add query parameters"), [=]() mutable
        {
            global->fetch = [=](auto url) mutable
            {
                expect(url)->toBe(std::string("http://localhost:3000/api/test?page=1&limit=10&filter=active"));
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("get"), [=](auto name) mutable
                        {
                            return ((name == std::string("content-length")) ? any(std::string("100")) : any(nullptr));
                        }
                        }
                    }}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        return (object{
                            object::pair{std::string("success"), true}, 
                            object::pair{std::string("data"), array<any>()}
                        });
                    }
                    }
                });
            };
            std::async([=]() { client->testRequest(std::string("GET"), std::string("/api/test"), object{
                object::pair{std::string("params"), object{
                    object::pair{std::string("page"), 1}, 
                    object::pair{std::string("limit"), 10}, 
                    object::pair{std::string("filter"), std::string("active")}
                }}
            }); });
        }
        );
        it(std::string("should handle API error response"), [=]() mutable
        {
            shared errorResponse = object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), object{
                    object::pair{std::string("code"), std::string("NOT_FOUND")}, 
                    object::pair{std::string("message"), std::string("Resource not found")}, 
                    object::pair{std::string("details"), std::string("The requested resource does not exist")}
                }}
            };
            global->fetch = [=]() mutable
            {
                return as<std::shared_ptr<Response>>((object{
                    object::pair{std::string("ok"), false}, 
                    object::pair{std::string("status"), 404}, 
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("get"), [=](auto name) mutable
                        {
                            return ((name == std::string("content-length")) ? any(std::string("100")) : any(nullptr));
                        }
                        }
                    }}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        return errorResponse;
                    }
                    }
                }));
            };
            try
            {
                std::async([=]() { client->testGet(std::string("/api/test")); });
                expect(true)->toBe(false);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(ApiError);
                expect((as<std::shared_ptr<ApiError>>(error))->code)->toBe(std::string("NOT_FOUND"));
                expect((as<std::shared_ptr<ApiError>>(error))->message)->toBe(std::string("Resource not found"));
                expect((as<std::shared_ptr<ApiError>>(error))->details)->toBe(std::string("The requested resource does not exist"));
                expect((as<std::shared_ptr<ApiError>>(error))->status)->toBe(404);
            }
        }
        );
        it(std::string("should handle network errors"), [=]() mutable
        {
            global->fetch = [=]() mutable
            {
                throw any(std::make_shared<Error>(std::string("Network error")));
            };
            try
            {
                std::async([=]() { client->testGet(std::string("/api/test")); });
                expect(true)->toBe(false);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(ApiError);
                expect((as<std::shared_ptr<ApiError>>(error))->code)->toBe(std::string("NETWORK_ERROR"));
            }
        }
        );
        it(std::string("should handle 204 No Content responses safely"), [=]() mutable
        {
            global->fetch = [=](auto url, auto options) mutable
            {
                expect(url)->toBe(std::string("http://localhost:3000/api/delete"));
                expect(options["method"])->toBe(std::string("DELETE"));
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 204}, 
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("get"), [=](auto name) mutable
                        {
                            return ((name == std::string("content-length")) ? nullptr : nullptr);
                        }
                        }
                    }}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("No content to parse")));
                    }
                    }
                });
            };
            auto result = std::async([=]() { client->testRequest<object>(std::string("DELETE"), std::string("/api/delete")); });
            expect(result)->toEqual(object{
                object::pair{std::string("success"), true}
            });
        }
        );
        it(std::string("should handle empty content-length responses safely"), [=]() mutable
        {
            global->fetch = [=](auto url, auto options) mutable
            {
                expect(url)->toBe(std::string("http://localhost:3000/api/clear"));
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("get"), [=](auto name) mutable
                        {
                            return ((name == std::string("content-length")) ? any(std::string("0")) : any(nullptr));
                        }
                        }
                    }}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("No content to parse")));
                    }
                    }
                });
            };
            auto result = std::async([=]() { client->testRequest<object>(std::string("POST"), std::string("/api/clear")); });
            expect(result)->toEqual(object{
                object::pair{std::string("success"), true}
            });
        }
        );
        it(std::string("should handle JSON parse failures for 2xx responses safely"), [=]() mutable
        {
            global->fetch = [=](auto url, auto options) mutable
            {
                expect(url)->toBe(std::string("http://localhost:3000/api/process"));
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("get"), [=](auto name) mutable
                        {
                            return ((name == std::string("content-length")) ? any(std::string("10")) : any(nullptr));
                        }
                        }
                    }}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("Invalid JSON")));
                    }
                    }
                });
            };
            auto result = std::async([=]() { client->testRequest<object>(std::string("POST"), std::string("/api/process")); });
            expect(result)->toEqual(object{
                object::pair{std::string("success"), true}
            });
        }
        );
        it(std::string("should handle different expected return types for 204 responses"), [=]() mutable
        {
            global->fetch = [=]() mutable
            {
                return as<std::shared_ptr<Response>>((object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 204}, 
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("get"), [=]() mutable
                        {
                            return nullptr;
                        }
                        }
                    }}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("No content")));
                    }
                    }
                }));
            };
            auto simpleResult = std::async([=]() { client->testRequest<object>(std::string("DELETE"), std::string("/api/test")); });
            expect(simpleResult)->toEqual(object{
                object::pair{std::string("success"), true}
            });
            auto complexResult = std::async([=]() { client->testRequest<object>(std::string("DELETE"), std::string("/api/test")); });
            expect(complexResult)->toEqual(object{
                object::pair{std::string("success"), true}
            });
        }
        );
        afterEach([=]() mutable
        {
            global->fetch = fetchMock;
        }
        );
    }
    );
}

MAIN
