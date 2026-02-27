#include "ocr-integration.test.h"

std::shared_ptr<Buffer> generateTextImage(string text, double width, double height, double fontSize)
{
    auto canvas = createCanvas(width, height);
    auto ctx = canvas->getContext(std:("2d"));
    ctx->fillStyle = std:("white");
    ctx->fillRect(0, 0, width, height);
    ctx->fillStyle = std:("black");
    ctx->font = string_empty + fontSize + std:("px Arial");
    ctx->textAlign = std:("center");
    ctx->textBaseline = std:("middle");
    ctx->fillText(text, width / 2, height / 2);
    return canvas->toBuffer(std:("image/png"));
};


std::shared_ptr<Buffer> generateMultiLineTextImage(string text, double width, double height, double fontSize)
{
    auto canvas = createCanvas(width, height);
    shared ctx = canvas->getContext(std:("2d"));
    ctx->fillStyle = std:("white");
    ctx->fillRect(0, 0, width, height);
    ctx->fillStyle = std:("black");
    ctx->font = string_empty + fontSize + std:("px Arial");
    ctx->textAlign = std:("left");
    ctx->textBaseline = std:("top");
    auto lines = text->split(std:("\
"));
    shared lineHeight = fontSize * 1.2;
    shared startY = (height - lines->get_length() * lineHeight) / 2;
    lines->forEach([=](auto line, auto index) mutable
    {
        ctx->fillText(line, 20, startY + index * lineHeight);
    }
    );
    return canvas->toBuffer(std:("image/png"));
};


std::shared_ptr<Buffer> generateComplexLayoutImage(double width, double height)
{
    auto canvas = createCanvas(width, height);
    auto ctx = canvas->getContext(std:("2d"));
    ctx->fillStyle = std:("white");
    ctx->fillRect(0, 0, width, height);
    ctx->fillStyle = std:("navy");
    ctx->fillRect(0, 0, width, 60);
    ctx->fillStyle = std:("white");
    ctx->font = std:("24px Arial");
    ctx->textAlign = std:("center");
    ctx->fillText(std:("Application Header"), width / 2, 35);
    ctx->fillStyle = std:("lightgray");
    ctx->fillRect(0, 60, 200, height - 60);
    ctx->fillStyle = std:("black");
    ctx->font = std:("16px Arial");
    ctx->textAlign = std:("left");
    ctx->fillText(std:("Menu Item 1"), 20, 100);
    ctx->fillText(std:("Menu Item 2"), 20, 130);
    ctx->fillText(std:("Menu Item 3"), 20, 160);
    ctx->fillStyle = std:("black");
    ctx->font = std:("18px Arial");
    ctx->fillText(std:("Main Content Area"), 220, 100);
    ctx->fillText(std:("Lorem ipsum dolor sit amet"), 220, 130);
    ctx->fillText(std:("consectetur adipiscing elit"), 220, 160);
    ctx->fillStyle = std:("blue");
    ctx->fillRect(220, 200, 100, 40);
    ctx->fillStyle = std:("white");
    ctx->font = std:("16px Arial");
    ctx->textAlign = std:("center");
    ctx->fillText(std:("Submit"), 270, 225);
    return canvas->toBuffer(std:("image/png"));
};



void Main(void)
{
    vi->mock(std:("@jitsi/robotjs"), [=]() mutable
    {
        return (object{
            object::pair{std:("default"), object{
                object::pair{std:("getScreenSize"), vi->fn([=]() mutable
                {
                    return (object{
                        object::pair{std:("width"), 1920}, 
                        object::pair{std:("height"), 1080}
                    });
                }
                )}, 
                object::pair{std:("screen"), object{
                    object::pair{std:("capture"), vi->fn([=]() mutable
                    {
                        return (object{
                            object::pair{std:("image"), Buffer::from(std:("mock-screenshot-data"))}, 
                            object::pair{std:("width"), 1920}, 
                            object::pair{std:("height"), 1080}, 
                            object::pair{std:("byteWidth"), 7680}, 
                            object::pair{std:("bitsPerPixel"), 32}, 
                            object::pair{std:("bytesPerPixel"), 4}
                        });
                    }
                    )}
                }}, 
                object::pair{std:("moveMouse"), vi->fn()}, 
                object::pair{std:("mouseClick"), vi->fn()}, 
                object::pair{std:("typeString"), vi->fn()}
            }}
        });
    }
    );
    describe(std:("OCR Integration Tests with Real Images"), [=]() mutable
    {
        shared<std::shared_ptr<RobotService>> robotService;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            mockRuntime = as<any>(object{
                object::pair{std:("useModel"), vi->fn()}, 
                object::pair{std:("getService"), vi->fn()}, 
                object::pair{std:("getAllServices"), vi->fn([=]() mutable
                {
                    return std::make_shared<Map>();
                }
                )}
            });
            robotService = std::make_shared<RobotService>(mockRuntime);
        }
        );
        afterEach([=]() mutable
        {
            std::async([=]() { robotService->stop(); });
            vi->clearAllMocks();
        }
        );
        describe(std:("Real Image Generation and OCR"), [=]() mutable
        {
            it(std:("should generate and process a simple text image"), [=]() mutable
            {
                auto testText = std:("Hello World");
                auto imageBuffer = generateTextImage(testText, 400, 100);
                expect(imageBuffer)->toBeDefined();
                expect(Buffer::isBuffer(imageBuffer))->toBe(true);
                expect(imageBuffer->length)->toBeGreaterThan(0);
            }
            );
            it(std:("should generate images with different text sizes"), [=]() mutable
            {
                auto texts = array<string>{ std:("Small"), std:("Medium Text"), std:("Large Text Content") };
                auto sizes = array<double>{ 12, 24, 48 };
                for (auto i = 0; i < texts->get_length(); i++)
                {
                    auto imageBuffer = generateTextImage(const_(texts)[i], 400, 100, const_(sizes)[i]);
                    expect(imageBuffer->length)->toBeGreaterThan(0);
                }
            }
            );
            it(std:("should generate multi-line text images"), [=]() mutable
            {
                auto multiLineText = std:("Line 1\
Line 2\
Line 3");
                auto imageBuffer = generateMultiLineTextImage(multiLineText, 400, 200);
                expect(imageBuffer)->toBeDefined();
                expect(imageBuffer->length)->toBeGreaterThan(0);
            }
            );
            it(std:("should generate images with special characters"), [=]() mutable
            {
                auto specialText = std:("Email: test@example.com\
Phone: +1-555-123-4567\
Price: $29.99");
                auto imageBuffer = generateMultiLineTextImage(specialText, 500, 150);
                expect(imageBuffer)->toBeDefined();
                expect(imageBuffer->length)->toBeGreaterThan(0);
            }
            );
            it(std:("should handle different image dimensions"), [=]() mutable
            {
                auto dimensions = array<object>{ object{
                    object::pair{std:("width"), 200}, 
                    object::pair{std:("height"), 100}
                }, object{
                    object::pair{std:("width"), 800}, 
                    object::pair{std:("height"), 200}
                }, object{
                    object::pair{std:("width"), 1024}, 
                    object::pair{std:("height"), 768}
                }, object{
                    object::pair{std:("width"), 1920}, 
                    object::pair{std:("height"), 1080}
                } };
                auto& __array2789_3015 = dimensions;
                for (auto __indx2789_3015 = 0_N; __indx2789_3015 < __array2789_3015->get_length(); __indx2789_3015++)
                {
                    auto& dim = const_(__array2789_3015)[__indx2789_3015];
                    {
                        auto imageBuffer = generateTextImage(std:("Test Text"), dim["width"], dim["height"]);
                        expect(imageBuffer->length)->toBeGreaterThan(0);
                    }
                }
            }
            );
        }
        );
        describe(std:("Image Downscaling Tests"), [=]() mutable
        {
            it(std:("should downscale large images correctly"), [=]() mutable
            {
                auto largeImage = generateTextImage(std:("Large Image Test"), 2048, 1536);
                auto downscaleImage = (as<any>(robotService))["downscaleImage"]["bind"](robotService);
                auto scaledImage = downscaleImage(largeImage, 1024);
                expect(scaledImage)->toBeDefined();
                expect(Buffer::isBuffer(scaledImage))->toBe(true);
                expect(scaledImage["length"])->toBeGreaterThan(0);
            }
            );
            it(std:("should maintain image quality during downscaling"), [=]() mutable
            {
                auto originalImage = generateTextImage(std:("Quality Test"), 1600, 1200);
                auto downscaleImage = (as<any>(robotService))["downscaleImage"]["bind"](robotService);
                auto scaledImage = downscaleImage(originalImage, 800);
                expect(originalImage->length)->toBeGreaterThan(0);
                expect(scaledImage["length"])->toBeGreaterThan(0);
            }
            );
        }
        );
        describe(std:("Performance with Real Images"), [=]() mutable
        {
            it(std:("should process small images quickly"), [=]() mutable
            {
                auto smallImage = generateTextImage(std:("Quick Test"), 200, 100);
                auto startTime = Date->now();
                auto downscaleImage = (as<any>(robotService))["downscaleImage"]["bind"](robotService);
                auto result = downscaleImage(smallImage, 1024);
                auto duration = Date->now() - startTime;
                expect(duration)->toBeLessThan(100);
                expect(result)->toBeDefined();
            }
            );
            it(std:("should handle multiple image processing requests"), [=]() mutable
            {
                auto images = Array(10)->fill(0)->map([=](auto _, auto i) mutable
                {
                    return generateTextImage(std:("Test ") + i + string_empty, 300, 100);
                }
                );
                auto startTime = Date->now();
                shared downscaleImage = (as<any>(robotService))["downscaleImage"]["bind"](robotService);
                auto results = images->map([=](auto img) mutable
                {
                    return downscaleImage(img, 1024);
                }
                );
                auto duration = Date->now() - startTime;
                expect(duration)->toBeLessThan(1000);
                expect(results)->toHaveLength(10);
                results->forEach([=](auto result) mutable
                {
                    return expect(result)->toBeDefined();
                }
                );
            }
            );
        }
        );
        describe(std:("Screen Context Integration"), [=]() mutable
        {
            it(std:("should integrate real image processing with screen context"), [=]() mutable
            {
                auto context = std::async([=]() { robotService->getContext(); });
                expect(context)->toBeDefined();
                expect(context->screenshot)->toBeDefined();
                expect(context->currentDescription)->toBe(string_empty);
            }
            );
        }
        );
    }
    );
}

MAIN
