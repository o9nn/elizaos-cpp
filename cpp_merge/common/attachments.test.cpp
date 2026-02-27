#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-bootstrap/src/__tests__/attachments.test.h"

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), object{
                object::pair{std::string("debug"), mock()}, 
                object::pair{std::string("warn"), mock()}, 
                object::pair{std::string("error"), mock()}, 
                object::pair{std::string("info"), mock()}
            }}
        });
    }
    );
    describe(std::string("processAttachments"), [=]() mutable
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
        it(std::string("should return empty array for no attachments"), [=]() mutable
        {
            auto result = std::async([=]() { processAttachments(array<any>(), as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toEqual(array<any>());
        }
        );
        it(std::string("should return empty array for null/undefined attachments"), [=]() mutable
        {
            auto result = std::async([=]() { processAttachments(as<any>(nullptr), as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toEqual(array<any>());
        }
        );
        it(std::string("should process image attachments and generate descriptions"), [=]() mutable
        {
            auto imageAttachment = object{
                object::pair{std::string("id"), std::string("image-1")}, 
                object::pair{std::string("url"), std::string("https://example.com/image.jpg")}, 
                object::pair{std::string("contentType"), ContentType->IMAGE}, 
                object::pair{std::string("source"), std::string("image/jpeg")}
            };
            mockRuntime->useModel = mock()->mockResolvedValue(std::string("<response>\
  <title>Beautiful Sunset</title>\
  <description>A stunning sunset over the ocean with vibrant colors</description>\
  <text>This image captures a breathtaking sunset scene over a calm ocean. The sky is painted with brilliant hues of orange, pink, and purple as the sun dips below the horizon. Gentle waves lap at the shore, creating a peaceful and serene atmosphere.</text>\
</response>"));
            auto result = std::async([=]() { processAttachments(array<any>{ imageAttachment }, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(1);
            expect(const_(result)[0]->id)->toBe(std::string("image-1"));
            expect(const_(result)[0]->title)->toBe(std::string("Beautiful Sunset"));
            expect(const_(result)[0]->description)->toBe(std::string("A stunning sunset over the ocean with vibrant colors"));
            expect(const_(result)[0]->text)->toBe(std::string("This image captures a breathtaking sunset scene over a calm ocean. The sky is painted with brilliant hues of orange, pink, and purple as the sun dips below the horizon. Gentle waves lap at the shore, creating a peaceful and serene atmosphere."));
            expect(mockRuntime->useModel)->toHaveBeenCalledWith(ModelType->IMAGE_DESCRIPTION, object{
                object::pair{std::string("prompt"), expect->stringContaining(std::string("Analyze the provided image"))}, 
                object::pair{std::string("imageUrl"), std::string("https://example.com/image.jpg")}
            });
        }
        );
        it(std::string("should skip processing for images that already have descriptions"), [=]() mutable
        {
            auto imageWithDescription = object{
                object::pair{std::string("id"), std::string("image-2")}, 
                object::pair{std::string("url"), std::string("https://example.com/described.jpg")}, 
                object::pair{std::string("contentType"), ContentType->IMAGE}, 
                object::pair{std::string("source"), std::string("image/jpeg")}, 
                object::pair{std::string("description"), std::string("Already has a description")}, 
                object::pair{std::string("title"), std::string("Existing Title")}, 
                object::pair{std::string("text"), std::string("Existing text")}
            };
            auto result = std::async([=]() { processAttachments(array<any>{ imageWithDescription }, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(1);
            expect(const_(result)[0])->toEqual(imageWithDescription);
            expect(mockRuntime->useModel)->not->toHaveBeenCalled();
        }
        );
        it(std::string("should handle non-image attachments without processing"), [=]() mutable
        {
            auto pdfAttachment = object{
                object::pair{std::string("id"), std::string("pdf-1")}, 
                object::pair{std::string("url"), std::string("https://example.com/document.pdf")}, 
                object::pair{std::string("source"), std::string("application/pdf")}, 
                object::pair{std::string("title"), std::string("PDF Document")}
            };
            auto result = std::async([=]() { processAttachments(array<any>{ pdfAttachment }, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(1);
            expect(const_(result)[0])->toEqual(pdfAttachment);
            expect(mockRuntime->useModel)->not->toHaveBeenCalled();
        }
        );
        it(std::string("should handle mixed attachment types"), [=]() mutable
        {
            auto attachments = array<std::shared_ptr<Media>>{ object{
                object::pair{std::string("id"), std::string("image-1")}, 
                object::pair{std::string("url"), std::string("https://example.com/image1.jpg")}, 
                object::pair{std::string("contentType"), ContentType->IMAGE}, 
                object::pair{std::string("source"), std::string("image/jpeg")}
            }, object{
                object::pair{std::string("id"), std::string("pdf-1")}, 
                object::pair{std::string("url"), std::string("https://example.com/doc.pdf")}, 
                object::pair{std::string("source"), std::string("application/pdf")}
            }, object{
                object::pair{std::string("id"), std::string("image-2")}, 
                object::pair{std::string("url"), std::string("https://example.com/image2.png")}, 
                object::pair{std::string("contentType"), ContentType->IMAGE}, 
                object::pair{std::string("source"), std::string("image/png")}, 
                object::pair{std::string("description"), std::string("Already described")}
            } };
            mockRuntime->useModel = mock()->mockResolvedValue(std::string("<response>\
  <title>Test Image</title>\
  <description>A test image description</description>\
  <text>Detailed description of the test image</text>\
</response>"));
            auto result = std::async([=]() { processAttachments(attachments, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(3);
            expect(mockRuntime->useModel)->toHaveBeenCalledTimes(1);
            expect(const_(result)[0]->description)->toBe(std::string("A test image description"));
            expect(const_(result)[1])->toEqual(const_(attachments)[1]);
            expect(const_(result)[2])->toEqual(const_(attachments)[2]);
        }
        );
        it(std::string("should handle object response format for backwards compatibility"), [=]() mutable
        {
            auto imageAttachment = object{
                object::pair{std::string("id"), std::string("image-1")}, 
                object::pair{std::string("url"), std::string("https://example.com/image.jpg")}, 
                object::pair{std::string("contentType"), ContentType->IMAGE}, 
                object::pair{std::string("source"), std::string("image/jpeg")}
            };
            mockRuntime->useModel = mock()->mockResolvedValue(object{
                object::pair{std::string("title"), std::string("Object Response Title")}, 
                object::pair{std::string("description"), std::string("Object response description")}, 
                object::pair{std::string("text"), std::string("Object response text")}
            });
            auto result = std::async([=]() { processAttachments(array<any>{ imageAttachment }, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(1);
            expect(const_(result)[0]->title)->toBe(std::string("Object Response Title"));
            expect(const_(result)[0]->description)->toBe(std::string("Object response description"));
            expect(const_(result)[0]->text)->toBe(std::string("Object response description"));
        }
        );
        it(std::string("should handle malformed XML responses gracefully"), [=]() mutable
        {
            auto imageAttachment = object{
                object::pair{std::string("id"), std::string("image-1")}, 
                object::pair{std::string("url"), std::string("https://example.com/image.jpg")}, 
                object::pair{std::string("contentType"), ContentType->IMAGE}, 
                object::pair{std::string("source"), std::string("image/jpeg")}
            };
            mockRuntime->useModel = mock()->mockResolvedValue(std::string("This is not valid XML"));
            auto result = std::async([=]() { processAttachments(array<any>{ imageAttachment }, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(1);
            expect(const_(result)[0])->toEqual(imageAttachment);
            expect(logger->warn)->toHaveBeenCalledWith(std::string("[Bootstrap] Failed to parse XML response for image description"));
        }
        );
        it(std::string("should handle errors during processing gracefully"), [=]() mutable
        {
            auto attachments = array<std::shared_ptr<Media>>{ object{
                object::pair{std::string("id"), std::string("image-1")}, 
                object::pair{std::string("url"), std::string("https://example.com/image1.jpg")}, 
                object::pair{std::string("contentType"), ContentType->IMAGE}, 
                object::pair{std::string("source"), std::string("image/jpeg")}
            }, object{
                object::pair{std::string("id"), std::string("image-2")}, 
                object::pair{std::string("url"), std::string("https://example.com/image2.jpg")}, 
                object::pair{std::string("contentType"), ContentType->IMAGE}, 
                object::pair{std::string("source"), std::string("image/jpeg")}
            } };
            mockRuntime->useModel = mock()->mockRejectedValueOnce(std::make_shared<Error>(std::string("Model API error")))->mockResolvedValueOnce(std::string("<response>\
  <title>Second Image</title>\
  <description>Description of second image</description>\
  <text>Detailed description of the second image</text>\
</response>"));
            auto result = std::async([=]() { processAttachments(attachments, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(2);
            expect(const_(result)[0])->toEqual(const_(attachments)[0]);
            expect(const_(result)[1]->description)->toBe(std::string("Description of second image"));
            expect(logger->error)->toHaveBeenCalledWith(std::string("[Bootstrap] Error generating image description:"), expect->any(Error));
        }
        );
        it(std::string("should handle various image content types"), [=]() mutable
        {
            auto attachments = array<std::shared_ptr<Media>>{ object{
                object::pair{std::string("id"), std::string("jpeg-image")}, 
                object::pair{std::string("url"), std::string("https://example.com/photo.jpg")}, 
                object::pair{std::string("contentType"), ContentType->IMAGE}, 
                object::pair{std::string("source"), std::string("image/jpeg")}
            }, object{
                object::pair{std::string("id"), std::string("png-image")}, 
                object::pair{std::string("url"), std::string("https://example.com/graphic.png")}, 
                object::pair{std::string("contentType"), ContentType->IMAGE}, 
                object::pair{std::string("source"), std::string("image/png")}
            }, object{
                object::pair{std::string("id"), std::string("webp-image")}, 
                object::pair{std::string("url"), std::string("https://example.com/modern.webp")}, 
                object::pair{std::string("contentType"), ContentType->IMAGE}, 
                object::pair{std::string("source"), std::string("image/webp")}
            } };
            shared callCount = 0;
            mockRuntime->useModel = mock()->mockImplementation([=]() mutable
            {
                callCount++;
                return Promise->resolve(std::string("<response>\
  <title>Image ") + callCount + std::string("</title>\
  <description>Description ") + callCount + std::string("</description>\
  <text>Text ") + callCount + std::string("</text>\
</response>"));
            }
            );
            auto result = std::async([=]() { processAttachments(attachments, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(3);
            expect(mockRuntime->useModel)->toHaveBeenCalledTimes(3);
            result->forEach([=](auto attachment, auto index) mutable
            {
                expect(attachment->title)->toBe(std::string("Image ") + (index + 1) + string_empty);
                expect(attachment->description)->toBe(std::string("Description ") + (index + 1) + string_empty);
            }
            );
        }
        );
        it(std::string("should set default title when not provided in response"), [=]() mutable
        {
            auto imageAttachment = object{
                object::pair{std::string("id"), std::string("image-1")}, 
                object::pair{std::string("url"), std::string("https://example.com/image.jpg")}, 
                object::pair{std::string("contentType"), ContentType->IMAGE}, 
                object::pair{std::string("source"), std::string("image/jpeg")}
            };
            mockRuntime->useModel = mock()->mockResolvedValue(std::string("<response>\
  <description>A description without title</description>\
  <text>Detailed text without title</text>\
</response>"));
            auto result = std::async([=]() { processAttachments(array<any>{ imageAttachment }, as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
            expect(result)->toHaveLength(1);
            expect(const_(result)[0]->title)->toBe(std::string("Image"));
            expect(const_(result)[0]->description)->toBe(std::string("A description without title"));
        }
        );
    }
    );
}

MAIN
