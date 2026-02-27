#include "base-client.test.h"

TestClient::TestClient(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}


void Main(void)
{
    describe(std:("BaseApiClient"), [=]() mutable
    {
        shared<std::shared_ptr<TestClient>> client;
        shared mockConfig = object{
            object::pair{std:("baseUrl"), std:("http://localhost:3000")}, 
            object::pair{std:("apiKey"), std:("test-key")}, 
            object::pair{std:("timeout"), 5000}
        };
        shared<any> fetchMock;
        beforeEach([=]() mutable
        {
            client = std::make_shared<TestClient>(mockConfig);
            fetchMock = global->fetch;
        }
        );
        it(std:("should initialize with correct config"), [=]() mutable
        {
            expect(const_(client)[std:("baseUrl")])->toBe(std:("http://localhost:3000"));
            expect(const_(client)[std:("apiKey")])->toBe(std:("test-key"));
            expect(const_(client)[std:("timeout")])->toBe(5000);
            expect(const_(const_(client)[std:("defaultHeaders")])[std:("X-API-KEY")])->toBe(std:("test-key"));
        }
        );
        it(std:("should remove trailing slash from baseUrl"), [=]() mutable
        {
            auto clientWithSlash = std::make_shared<TestClient>(utils::assign(object{
                , 
                object::pair{std:("baseUrl"), std:("http://localhost:3000/")}
            }, mockConfig));
            expect((*const_(clientWithSlash))[std:("baseUrl")])->toBe(std:("http://localhost:3000"));
        }
        );
        it(std:("should make successful GET request"), [=]() mutable
        {
            shared mockResponse = object{
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), object{
                    object::pair{std:("message"), std:("Hello World")}
                }}
            };
            global->fetch = [=](auto url, auto options) mutable
            {
                expect(url)->toBe(std:("http://localhost:3000/api/test"));
                expect(options["method"])->toBe(std:("GET"));
                expect(const_(options["headers"])[std:("X-API-KEY")])->toBe(std:("test-key"));
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("headers"), object{
                        object::pair{std:("get"), [=](auto name) mutable
                        {
                            return ((name == std:("content-length")) ? any(std:("100")) (nullptr));
                        }
                        }
                    }}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        return mockResponse;
                    }
                    }
                });
            };
            auto result = std::async([=]() { client->testGet(std:("/api/test")); });
            expect(result)->toEqual(mockResponse["data"]);
        }
        );
        it(std:("should make successful POST request"), [=]() mutable
        {
            shared body = object{
                object::pair{std:("name"), std:("Test Item")}
            };
            shared mockResponse = object{
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), utils::assign(object{
                    object::pair{std:("id"), std:("123")}
                }, body)}
            };
            global->fetch = [=](auto url, auto options) mutable
            {
                expect(url)->toBe(std:("http://localhost:3000/api/test"));
                expect(options["method"])->toBe(std:("POST"));
                expect(options["body"])->toBe(JSON->stringify(body));
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("headers"), object{
                        object::pair{std:("get"), [=](auto name) mutable
                        {
                            return ((name == std:("content-length")) ? any(std:("100")) (nullptr));
                        }
                        }
                    }}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        return mockResponse;
                    }
                    }
                });
            };
            auto result = std::async([=]() { client->testPost(std:("/api/test"), body); });
            expect(result)->toEqual(mockResponse["data"]);
        }
        );
        it(std:("should handle FormData without Content-Type header"), [=]() mutable
        {
            shared formData = std::make_shared<FormData>();
            formData->append(std:("file"), std:("test"));
            global->fetch = [=](auto url, auto options) mutable
            {
                expect(const_(options["headers"])[std:("Content-Type")])->toBeUndefined();
                expect(options["body"])->toBe(formData);
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("headers"), object{
                        object::pair{std:("get"), [=](auto name) mutable
                        {
                            return ((name == std:("content-length")) ? any(std:("100")) (nullptr));
                        }
                        }
                    }}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        return (object{
                            object::pair{std:("success"), true}, 
                            object::pair{std:("data"), object{
                                object::pair{std:("uploaded"), true}
                            }}
                        });
                    }
                    }
                });
            };
            auto result = std::async([=]() { client->testPost(std:("/api/upload"), formData); });
            expect(result)->toEqual(object{
                object::pair{std:("uploaded"), true}
            });
        }
        );
        it(std:("should add query parameters"), [=]() mutable
        {
            global->fetch = [=](auto url) mutable
            {
                expect(url)->toBe(std:("http://localhost:3000/api/test?page=1&limit=10&filter=active"));
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("headers"), object{
                        object::pair{std:("get"), [=](auto name) mutable
                        {
                            return ((name == std:("content-length")) ? any(std:("100")) (nullptr));
                        }
                        }
                    }}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        return (object{
                            object::pair{std:("success"), true}, 
                            object::pair{std:("data"), array<any>()}
                        });
                    }
                    }
                });
            };
            std::async([=]() { client->testRequest(std:("GET"), std:("/api/test"), object{
                object::pair{std:("params"), object{
                    object::pair{std:("page"), 1}, 
                    object::pair{std:("limit"), 10}, 
                    object::pair{std:("filter"), std:("active")}
                }}
            }); });
        }
        );
        it(std:("should handle API error response"), [=]() mutable
        {
            shared errorResponse = object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), object{
                    object::pair{std:("code"), std:("NOT_FOUND")}, 
                    object::pair{std:("message"), std:("Resource not found")}, 
                    object::pair{std:("details"), std:("The requested resource does not exist")}
                }}
            };
            global->fetch = [=]() mutable
            {
                return as<std::shared_ptr<Response>>((object{
                    object::pair{std:("ok"), false}, 
                    object::pair{std:("status"), 404}, 
                    object::pair{std:("headers"), object{
                        object::pair{std:("get"), [=](auto name) mutable
                        {
                            return ((name == std:("content-length")) ? any(std:("100")) (nullptr));
                        }
                        }
                    }}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        return errorResponse;
                    }
                    }
                }));
            };
            try
            {
                std::async([=]() { client->testGet(std:("/api/test")); });
                expect(true)->toBe(false);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(ApiError);
                expect((as<std::shared_ptr<ApiError>>(error))->code)->toBe(std:("NOT_FOUND"));
                expect((as<std::shared_ptr<ApiError>>(error))->message)->toBe(std:("Resource not found"));
                expect((as<std::shared_ptr<ApiError>>(error))->details)->toBe(std:("The requested resource does not exist"));
                expect((as<std::shared_ptr<ApiError>>(error))->status)->toBe(404);
            }
        }
        );
        it(std:("should handle network errors"), [=]() mutable
        {
            global->fetch = [=]() mutable
            {
                throw any(std::make_shared<Error>(std:("Network error")));
            };
            try
            {
                std::async([=]() { client->testGet(std:("/api/test")); });
                expect(true)->toBe(false);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(ApiError);
                expect((as<std::shared_ptr<ApiError>>(error))->code)->toBe(std:("NETWORK_ERROR"));
            }
        }
        );
        it(std:("should handle 204 No Content responses safely"), [=]() mutable
        {
            global->fetch = [=](auto url, auto options) mutable
            {
                expect(url)->toBe(std:("http://localhost:3000/api/delete"));
                expect(options["method"])->toBe(std:("DELETE"));
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 204}, 
                    object::pair{std:("headers"), object{
                        object::pair{std:("get"), [=](auto name) mutable
                        {
                            return ((name == std:("content-length")) ? nullptr : nullptr);
                        }
                        }
                    }}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std:("No content to parse")));
                    }
                    }
                });
            };
            auto result = std::async([=]() { client->testRequest<object>(std:("DELETE"), std:("/api/delete")); });
            expect(result)->toEqual(object{
                object::pair{std:("success"), true}
            });
        }
        );
        it(std:("should handle empty content-length responses safely"), [=]() mutable
        {
            global->fetch = [=](auto url, auto options) mutable
            {
                expect(url)->toBe(std:("http://localhost:3000/api/clear"));
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("headers"), object{
                        object::pair{std:("get"), [=](auto name) mutable
                        {
                            return ((name == std:("content-length")) ? any(std:("0")) (nullptr));
                        }
                        }
                    }}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std:("No content to parse")));
                    }
                    }
                });
            };
            auto result = std::async([=]() { client->testRequest<object>(std:("POST"), std:("/api/clear")); });
            expect(result)->toEqual(object{
                object::pair{std:("success"), true}
            });
        }
        );
        it(std:("should handle JSON parse failures for 2xx responses safely"), [=]() mutable
        {
            global->fetch = [=](auto url, auto options) mutable
            {
                expect(url)->toBe(std:("http://localhost:3000/api/process"));
                return as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("headers"), object{
                        object::pair{std:("get"), [=](auto name) mutable
                        {
                            return ((name == std:("content-length")) ? any(std:("10")) (nullptr));
                        }
                        }
                    }}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std:("Invalid JSON")));
                    }
                    }
                });
            };
            auto result = std::async([=]() { client->testRequest<object>(std:("POST"), std:("/api/process")); });
            expect(result)->toEqual(object{
                object::pair{std:("success"), true}
            });
        }
        );
        it(std:("should handle different expected return types for 204 responses"), [=]() mutable
        {
            global->fetch = [=]() mutable
            {
                return as<std::shared_ptr<Response>>((object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 204}, 
                    object::pair{std:("headers"), object{
                        object::pair{std:("get"), [=]() mutable
                        {
                            return nullptr;
                        }
                        }
                    }}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std:("No content")));
                    }
                    }
                }));
            };
            auto simpleResult = std::async([=]() { client->testRequest<object>(std:("DELETE"), std:("/api/test")); });
            expect(simpleResult)->toEqual(object{
                object::pair{std:("success"), true}
            });
            auto complexResult = std::async([=]() { client->testRequest<object>(std:("DELETE"), std:("/api/test")); });
            expect(complexResult)->toEqual(object{
                object::pair{std:("success"), true}
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
