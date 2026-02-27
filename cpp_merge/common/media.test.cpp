#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/__tests__/services/media.test.h"

any TEST_AGENT_ID = as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440001"));
any TEST_CHANNEL_ID = as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440002"));

void Main(void)
{
    describe(std:("MediaService"), [=]() mutable
    {
        shared<std::shared_ptr<MediaService>> mediaService;
        shared mockConfig = object{
            object::pair{std:("baseUrl"), std:("http://localhost:3000")}, 
            object::pair{std:("apiKey"), std:("test-key")}
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
        describe(std:("constructor"), [=]() mutable
        {
            it(std:("should create an instance with valid configuration"), [=]() mutable
            {
                expect(mediaService)->toBeInstanceOf(MediaService);
            }
            );
            it(std:("should throw error when initialized with invalid configuration"), [=]() mutable
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
        describe(std:("uploadAgentMedia"), [=]() mutable
        {
            shared mockFile = std::make_shared<Blob>(array<string>{ std:("test content") }, object{
                object::pair{std:("type"), std:("image/png")}
            });
            shared params = object{
                object::pair{std:("file"), mockFile}, 
                object::pair{std:("filename"), std:("test.png")}, 
                object::pair{std:("contentType"), std:("image/png")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("description"), std:("Test image")}
                }}
            };
            it(std:("should upload agent media successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440010"))}, 
                    object::pair{std:("url"), std:("http://example.com/media/test.png")}, 
                    object::pair{std:("filename"), std:("test.png")}, 
                    object::pair{std:("contentType"), std:("image/png")}, 
                    object::pair{std:("size"), 1024}, 
                    object::pair{std:("uploadedAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("description"), std:("Test image")}
                    }}
                };
                (as<any>(mediaService))["request"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { mediaService->uploadAgentMedia(TEST_AGENT_ID, params); });
                expect((as<any>(mediaService))["request"])->toHaveBeenCalledWith(std:("POST"), std:("/api/media/agents/") + TEST_AGENT_ID + std:("/upload-media"), expect->objectContaining(object{
                    object::pair{std:("body"), expect->any(FormData)}
                }));
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std:("should handle file upload without optional parameters"), [=]() mutable
            {
                auto paramsMinimal = object{
                    object::pair{std:("file"), mockFile}
                };
                auto mockResponse = object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440011"))}, 
                    object::pair{std:("url"), std:("http://example.com/media/test.png")}, 
                    object::pair{std:("filename"), std:("test.png")}, 
                    object::pair{std:("contentType"), std:("image/png")}, 
                    object::pair{std:("size"), 512}, 
                    object::pair{std:("uploadedAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}
                };
                (as<any>(mediaService))["request"]["mockResolvedValue"](mockResponse);
                std::async([=]() { mediaService->uploadAgentMedia(TEST_AGENT_ID, paramsMinimal); });
                expect((as<any>(mediaService))["request"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std:("uploadChannelMedia"), [=]() mutable
        {
            shared mockFile = std::make_shared<File>(array<string>{ std:("test content") }, std:("test.png"), object{
                object::pair{std:("type"), std:("image/png")}
            });
            it(std:("should upload channel media successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440020"))}, 
                    object::pair{std:("url"), std:("http://example.com/media/test.png")}, 
                    object::pair{std:("filename"), std:("test.png")}, 
                    object::pair{std:("size"), 1024}, 
                    object::pair{std:("contentType"), std:("image/png")}, 
                    object::pair{std:("uploadedAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}
                };
                (as<any>(mediaService))["request"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { mediaService->uploadChannelMedia(TEST_CHANNEL_ID, mockFile); });
                expect((as<any>(mediaService))["request"])->toHaveBeenCalledWith(std:("POST"), std:("/api/messaging/central-channels/") + TEST_CHANNEL_ID + std:("/upload-media"), expect->objectContaining(object{
                    object::pair{std:("body"), expect->any(FormData)}
                }));
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std:("should handle file upload without errors"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440020"))}, 
                    object::pair{std:("url"), std:("http://example.com/media/test.png")}, 
                    object::pair{std:("filename"), std:("test.png")}, 
                    object::pair{std:("size"), 1024}, 
                    object::pair{std:("contentType"), std:("image/png")}, 
                    object::pair{std:("uploadedAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}
                };
                (as<any>(mediaService))["request"]["mockResolvedValue"](mockResponse);
                std::async([=]() { mediaService->uploadChannelMedia(TEST_CHANNEL_ID, mockFile); });
                expect((as<any>(mediaService))["request"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std:("error handling"), [=]() mutable
        {
            shared mockFile = std::make_shared<Blob>(array<string>{ std:("test") }, object{
                object::pair{std:("type"), std:("image/png")}
            });
            it(std:("should handle network errors"), [=]() mutable
            {
                (as<any>(mediaService))["request"]["mockRejectedValue"](std::make_shared<Error>(std:("Network error")));
                std::async([=]() { expect(mediaService->uploadAgentMedia(TEST_AGENT_ID, object{
                    object::pair{std:("file"), mockFile}, 
                    object::pair{std:("filename"), std:("test.png")}
                }))->rejects->toThrow(std:("Network error")); });
            }
            );
            it(std:("should handle file upload errors"), [=]() mutable
            {
                (as<any>(mediaService))["request"]["mockRejectedValue"](std::make_shared<Error>(std:("Upload failed")));
                std::async([=]() { expect(mediaService->uploadChannelMedia(TEST_CHANNEL_ID, mockFile))->rejects->toThrow(std:("Upload failed")); });
            }
            );
            it(std:("should handle API errors"), [=]() mutable
            {
                (as<any>(mediaService))["request"]["mockRejectedValue"](std::make_shared<Error>(std:("API error")));
                std::async([=]() { expect(mediaService->uploadAgentMedia(TEST_AGENT_ID, object{
                    object::pair{std:("file"), mockFile}, 
                    object::pair{std:("filename"), std:("test.png")}
                }))->rejects->toThrow(std:("API error")); });
            }
            );
        }
        );
    }
    );
}

MAIN
