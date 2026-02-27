#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-bootstrap/src/__tests__/attachments.test.h"

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), object{
                object::pair{std:("debug"), mock()}, 
                object::pair{std:("warn"), mock()}, 
                object::pair{std:("error"), mock()}, 
                object::pair{std:("info"), mock()}
            }}
        });
    }
    );
    describe(std:("processAttachments"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        beforeEach([=]() mutable
        {
            mockRuntime = createMockRuntime();
            mock->restore();
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should return empty array for no attachments"), [=]() mutable
        {
            auto result = std::async([=]() { processAttachments(array<any>(), as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toEqual(array<any>());
        }
        );
        it(std:("should return empty array for null/undefined attachments"), [=]() mutable
        {
            auto result = std::async([=]() { processAttachments(as<any>(nullptr), as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toEqual(array<any>());
        }
        );
        it(std:("should process image attachments and generate descriptions"), [=]() mutable
        {
            auto imageAttachment = object{
                object::pair{std:("id"), std:("image-1")}, 
                object::pair{std:("url"), std:("https://example.com/image.jpg")}, 
                object::pair{std:("contentType"), ContentType->IMAGE}, 
                object::pair{std:("source"), std:("image/jpeg")}
            };
            mockRuntime->useModel = mock()->mockResolvedValue(std:("<response>\
  <title>Beautiful Sunset</title>\
  <description>A stunning sunset over the ocean with vibrant colors</description>\
  <text>This image captures a breathtaking sunset scene over a calm ocean. The sky is painted with brilliant hues of orange, pink, and purple as the sun dips below the horizon. Gentle waves lap at the shore, creating a peaceful and serene atmosphere.</text>\
</response>"));
            auto result = std::async([=]() { processAttachments(array<any>{ imageAttachment }, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(1);
            expect(const_(result)[0]->id)->toBe(std:("image-1"));
            expect(const_(result)[0]->title)->toBe(std:("Beautiful Sunset"));
            expect(const_(result)[0]->description)->toBe(std:("A stunning sunset over the ocean with vibrant colors"));
            expect(const_(result)[0]->text)->toBe(std:("This image captures a breathtaking sunset scene over a calm ocean. The sky is painted with brilliant hues of orange, pink, and purple as the sun dips below the horizon. Gentle waves lap at the shore, creating a peaceful and serene atmosphere."));
            expect(mockRuntime->useModel)->toHaveBeenCalledWith(ModelType->IMAGE_DESCRIPTION, object{
                object::pair{std:("prompt"), expect->stringContaining(std:("Analyze the provided image"))}, 
                object::pair{std:("imageUrl"), std:("https://example.com/image.jpg")}
            });
        }
        );
        it(std:("should skip processing for images that already have descriptions"), [=]() mutable
        {
            auto imageWithDescription = object{
                object::pair{std:("id"), std:("image-2")}, 
                object::pair{std:("url"), std:("https://example.com/described.jpg")}, 
                object::pair{std:("contentType"), ContentType->IMAGE}, 
                object::pair{std:("source"), std:("image/jpeg")}, 
                object::pair{std:("description"), std:("Already has a description")}, 
                object::pair{std:("title"), std:("Existing Title")}, 
                object::pair{std:("text"), std:("Existing text")}
            };
            auto result = std::async([=]() { processAttachments(array<any>{ imageWithDescription }, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(1);
            expect(const_(result)[0])->toEqual(imageWithDescription);
            expect(mockRuntime->useModel)->not->toHaveBeenCalled();
        }
        );
        it(std:("should handle non-image attachments without processing"), [=]() mutable
        {
            auto pdfAttachment = object{
                object::pair{std:("id"), std:("pdf-1")}, 
                object::pair{std:("url"), std:("https://example.com/document.pdf")}, 
                object::pair{std:("source"), std:("application/pdf")}, 
                object::pair{std:("title"), std:("PDF Document")}
            };
            auto result = std::async([=]() { processAttachments(array<any>{ pdfAttachment }, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(1);
            expect(const_(result)[0])->toEqual(pdfAttachment);
            expect(mockRuntime->useModel)->not->toHaveBeenCalled();
        }
        );
        it(std:("should handle mixed attachment types"), [=]() mutable
        {
            auto attachments = array<std::shared_ptr<Media>>{ object{
                object::pair{std:("id"), std:("image-1")}, 
                object::pair{std:("url"), std:("https://example.com/image1.jpg")}, 
                object::pair{std:("contentType"), ContentType->IMAGE}, 
                object::pair{std:("source"), std:("image/jpeg")}
            }, object{
                object::pair{std:("id"), std:("pdf-1")}, 
                object::pair{std:("url"), std:("https://example.com/doc.pdf")}, 
                object::pair{std:("source"), std:("application/pdf")}
            }, object{
                object::pair{std:("id"), std:("image-2")}, 
                object::pair{std:("url"), std:("https://example.com/image2.png")}, 
                object::pair{std:("contentType"), ContentType->IMAGE}, 
                object::pair{std:("source"), std:("image/png")}, 
                object::pair{std:("description"), std:("Already described")}
            } };
            mockRuntime->useModel = mock()->mockResolvedValue(std:("<response>\
  <title>Test Image</title>\
  <description>A test image description</description>\
  <text>Detailed description of the test image</text>\
</response>"));
            auto result = std::async([=]() { processAttachments(attachments, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(3);
            expect(mockRuntime->useModel)->toHaveBeenCalledTimes(1);
            expect(const_(result)[0]->description)->toBe(std:("A test image description"));
            expect(const_(result)[1])->toEqual(const_(attachments)[1]);
            expect(const_(result)[2])->toEqual(const_(attachments)[2]);
        }
        );
        it(std:("should handle object response format for backwards compatibility"), [=]() mutable
        {
            auto imageAttachment = object{
                object::pair{std:("id"), std:("image-1")}, 
                object::pair{std:("url"), std:("https://example.com/image.jpg")}, 
                object::pair{std:("contentType"), ContentType->IMAGE}, 
                object::pair{std:("source"), std:("image/jpeg")}
            };
            mockRuntime->useModel = mock()->mockResolvedValue(object{
                object::pair{std:("title"), std:("Object Response Title")}, 
                object::pair{std:("description"), std:("Object response description")}, 
                object::pair{std:("text"), std:("Object response text")}
            });
            auto result = std::async([=]() { processAttachments(array<any>{ imageAttachment }, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(1);
            expect(const_(result)[0]->title)->toBe(std:("Object Response Title"));
            expect(const_(result)[0]->description)->toBe(std:("Object response description"));
            expect(const_(result)[0]->text)->toBe(std:("Object response description"));
        }
        );
        it(std:("should handle malformed XML responses gracefully"), [=]() mutable
        {
            auto imageAttachment = object{
                object::pair{std:("id"), std:("image-1")}, 
                object::pair{std:("url"), std:("https://example.com/image.jpg")}, 
                object::pair{std:("contentType"), ContentType->IMAGE}, 
                object::pair{std:("source"), std:("image/jpeg")}
            };
            mockRuntime->useModel = mock()->mockResolvedValue(std:("This is not valid XML"));
            auto result = std::async([=]() { processAttachments(array<any>{ imageAttachment }, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(1);
            expect(const_(result)[0])->toEqual(imageAttachment);
            expect(logger->warn)->toHaveBeenCalledWith(std:("[Bootstrap] Failed to parse XML response for image description"));
        }
        );
        it(std:("should handle errors during processing gracefully"), [=]() mutable
        {
            auto attachments = array<std::shared_ptr<Media>>{ object{
                object::pair{std:("id"), std:("image-1")}, 
                object::pair{std:("url"), std:("https://example.com/image1.jpg")}, 
                object::pair{std:("contentType"), ContentType->IMAGE}, 
                object::pair{std:("source"), std:("image/jpeg")}
            }, object{
                object::pair{std:("id"), std:("image-2")}, 
                object::pair{std:("url"), std:("https://example.com/image2.jpg")}, 
                object::pair{std:("contentType"), ContentType->IMAGE}, 
                object::pair{std:("source"), std:("image/jpeg")}
            } };
            mockRuntime->useModel = mock()->mockRejectedValueOnce(std::make_shared<Error>(std:("Model API error")))->mockResolvedValueOnce(std:("<response>\
  <title>Second Image</title>\
  <description>Description of second image</description>\
  <text>Detailed description of the second image</text>\
</response>"));
            auto result = std::async([=]() { processAttachments(attachments, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(2);
            expect(const_(result)[0])->toEqual(const_(attachments)[0]);
            expect(const_(result)[1]->description)->toBe(std:("Description of second image"));
            expect(logger->error)->toHaveBeenCalledWith(std:("[Bootstrap] Error generating image description:"), expect->any(Error));
        }
        );
        it(std:("should handle various image content types"), [=]() mutable
        {
            auto attachments = array<std::shared_ptr<Media>>{ object{
                object::pair{std:("id"), std:("jpeg-image")}, 
                object::pair{std:("url"), std:("https://example.com/photo.jpg")}, 
                object::pair{std:("contentType"), ContentType->IMAGE}, 
                object::pair{std:("source"), std:("image/jpeg")}
            }, object{
                object::pair{std:("id"), std:("png-image")}, 
                object::pair{std:("url"), std:("https://example.com/graphic.png")}, 
                object::pair{std:("contentType"), ContentType->IMAGE}, 
                object::pair{std:("source"), std:("image/png")}
            }, object{
                object::pair{std:("id"), std:("webp-image")}, 
                object::pair{std:("url"), std:("https://example.com/modern.webp")}, 
                object::pair{std:("contentType"), ContentType->IMAGE}, 
                object::pair{std:("source"), std:("image/webp")}
            } };
            shared callCount = 0;
            mockRuntime->useModel = mock()->mockImplementation([=]() mutable
            {
                callCount++;
                return Promise->resolve(std:("<response>\
  <title>Image ") + callCount + std:("</title>\
  <description>Description ") + callCount + std:("</description>\
  <text>Text ") + callCount + std:("</text>\
</response>"));
            }
            );
            auto result = std::async([=]() { processAttachments(attachments, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(3);
            expect(mockRuntime->useModel)->toHaveBeenCalledTimes(3);
            result->forEach([=](auto attachment, auto index) mutable
            {
                expect(attachment->title)->toBe(std:("Image ") + (index + 1) + string_empty);
                expect(attachment->description)->toBe(std:("Description ") + (index + 1) + string_empty);
            }
            );
        }
        );
        it(std:("should set default title when not provided in response"), [=]() mutable
        {
            auto imageAttachment = object{
                object::pair{std:("id"), std:("image-1")}, 
                object::pair{std:("url"), std:("https://example.com/image.jpg")}, 
                object::pair{std:("contentType"), ContentType->IMAGE}, 
                object::pair{std:("source"), std:("image/jpeg")}
            };
            mockRuntime->useModel = mock()->mockResolvedValue(std:("<response>\
  <description>A description without title</description>\
  <text>Detailed text without title</text>\
</response>"));
            auto result = std::async([=]() { processAttachments(array<any>{ imageAttachment }, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(1);
            expect(const_(result)[0]->title)->toBe(std:("Image"));
            expect(const_(result)[0]->description)->toBe(std:("A description without title"));
        }
        );
    }
    );
}

MAIN
