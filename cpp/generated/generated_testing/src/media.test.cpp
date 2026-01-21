#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/__tests__/services/media.test.h"

any TEST_AGENT_ID = as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440001"));
any TEST_CHANNEL_ID = as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440002"));

void Main(void)
{
    describe(std::string("MediaService"), [=]() mutable
    {
        shared<std::shared_ptr<MediaService>> mediaService;
        shared mockConfig = object{
            object::pair{std::string("baseUrl"), std::string("http://localhost:3000")}, 
            object::pair{std::string("apiKey"), std::string("test-key")}
        };
        beforeEach([=]() mutable
        {
            mediaService = std::make_shared<MediaService>(mockConfig);
            (as<any>(mediaService))["request"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
        }
        );
        afterEach([=]() mutable
        {
            auto requestMock = (as<any>(mediaService))["request"];
            if (requestMock["mockClear"]) requestMock["mockClear"]();
        }
        );
        describe(std::string("constructor"), [=]() mutable
        {
            it(std::string("should create an instance with valid configuration"), [=]() mutable
            {
                expect(mediaService)->toBeInstanceOf(MediaService);
            }
            );
            it(std::string("should throw error when initialized with invalid configuration"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return std::make_shared<MediaService>(as<any>(nullptr));
                }
                )->toThrow();
            }
            );
        }
        );
        describe(std::string("uploadAgentMedia"), [=]() mutable
        {
            shared mockFile = std::make_shared<Blob>(array<string>{ std::string("test content") }, object{
                object::pair{std::string("type"), std::string("image/png")}
            });
            shared params = object{
                object::pair{std::string("file"), mockFile}, 
                object::pair{std::string("filename"), std::string("test.png")}, 
                object::pair{std::string("contentType"), std::string("image/png")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("description"), std::string("Test image")}
                }}
            };
            it(std::string("should upload agent media successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440010"))}, 
                    object::pair{std::string("url"), std::string("http://example.com/media/test.png")}, 
                    object::pair{std::string("filename"), std::string("test.png")}, 
                    object::pair{std::string("contentType"), std::string("image/png")}, 
                    object::pair{std::string("size"), 1024}, 
                    object::pair{std::string("uploadedAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("description"), std::string("Test image")}
                    }}
                };
                (as<any>(mediaService))["request"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { mediaService->uploadAgentMedia(TEST_AGENT_ID, params); });
                expect((as<any>(mediaService))["request"])->toHaveBeenCalledWith(std::string("POST"), std::string("/api/media/agents/") + TEST_AGENT_ID + std::string("/upload-media"), expect->objectContaining(object{
                    object::pair{std::string("body"), expect->any(FormData)}
                }));
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std::string("should handle file upload without optional parameters"), [=]() mutable
            {
                auto paramsMinimal = object{
                    object::pair{std::string("file"), mockFile}
                };
                auto mockResponse = object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440011"))}, 
                    object::pair{std::string("url"), std::string("http://example.com/media/test.png")}, 
                    object::pair{std::string("filename"), std::string("test.png")}, 
                    object::pair{std::string("contentType"), std::string("image/png")}, 
                    object::pair{std::string("size"), 512}, 
                    object::pair{std::string("uploadedAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}
                };
                (as<any>(mediaService))["request"]["mockResolvedValue"](mockResponse);
                std::async([=]() { mediaService->uploadAgentMedia(TEST_AGENT_ID, paramsMinimal); });
                expect((as<any>(mediaService))["request"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std::string("uploadChannelMedia"), [=]() mutable
        {
            shared mockFile = std::make_shared<File>(array<string>{ std::string("test content") }, std::string("test.png"), object{
                object::pair{std::string("type"), std::string("image/png")}
            });
            it(std::string("should upload channel media successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440020"))}, 
                    object::pair{std::string("url"), std::string("http://example.com/media/test.png")}, 
                    object::pair{std::string("filename"), std::string("test.png")}, 
                    object::pair{std::string("size"), 1024}, 
                    object::pair{std::string("contentType"), std::string("image/png")}, 
                    object::pair{std::string("uploadedAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}
                };
                (as<any>(mediaService))["request"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { mediaService->uploadChannelMedia(TEST_CHANNEL_ID, mockFile); });
                expect((as<any>(mediaService))["request"])->toHaveBeenCalledWith(std::string("POST"), std::string("/api/messaging/central-channels/") + TEST_CHANNEL_ID + std::string("/upload-media"), expect->objectContaining(object{
                    object::pair{std::string("body"), expect->any(FormData)}
                }));
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std::string("should handle file upload without errors"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440020"))}, 
                    object::pair{std::string("url"), std::string("http://example.com/media/test.png")}, 
                    object::pair{std::string("filename"), std::string("test.png")}, 
                    object::pair{std::string("size"), 1024}, 
                    object::pair{std::string("contentType"), std::string("image/png")}, 
                    object::pair{std::string("uploadedAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}
                };
                (as<any>(mediaService))["request"]["mockResolvedValue"](mockResponse);
                std::async([=]() { mediaService->uploadChannelMedia(TEST_CHANNEL_ID, mockFile); });
                expect((as<any>(mediaService))["request"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std::string("error handling"), [=]() mutable
        {
            shared mockFile = std::make_shared<Blob>(array<string>{ std::string("test") }, object{
                object::pair{std::string("type"), std::string("image/png")}
            });
            it(std::string("should handle network errors"), [=]() mutable
            {
                (as<any>(mediaService))["request"]["mockRejectedValue"](std::make_shared<Error>(std::string("Network error")));
                std::async([=]() { expect(mediaService->uploadAgentMedia(TEST_AGENT_ID, object{
                    object::pair{std::string("file"), mockFile}, 
                    object::pair{std::string("filename"), std::string("test.png")}
                }))->rejects->toThrow(std::string("Network error")); });
            }
            );
            it(std::string("should handle file upload errors"), [=]() mutable
            {
                (as<any>(mediaService))["request"]["mockRejectedValue"](std::make_shared<Error>(std::string("Upload failed")));
                std::async([=]() { expect(mediaService->uploadChannelMedia(TEST_CHANNEL_ID, mockFile))->rejects->toThrow(std::string("Upload failed")); });
            }
            );
            it(std::string("should handle API errors"), [=]() mutable
            {
                (as<any>(mediaService))["request"]["mockRejectedValue"](std::make_shared<Error>(std::string("API error")));
                std::async([=]() { expect(mediaService->uploadAgentMedia(TEST_AGENT_ID, object{
                    object::pair{std::string("file"), mockFile}, 
                    object::pair{std::string("filename"), std::string("test.png")}
                }))->rejects->toThrow(std::string("API error")); });
            }
            );
        }
        );
    }
    );
}

MAIN
