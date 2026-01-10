#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-robot/tests/ocr-integration.test.h"

std::shared_ptr<Buffer> generateTextImage(string text, double width, double height, double fontSize)
{
    auto canvas = createCanvas(width, height);
    auto ctx = canvas->getContext(std::string("2d"));
    ctx->fillStyle = std::string("white");
    ctx->fillRect(0, 0, width, height);
    ctx->fillStyle = std::string("black");
    ctx->font = string_empty + fontSize + std::string("px Arial");
    ctx->textAlign = std::string("center");
    ctx->textBaseline = std::string("middle");
    ctx->fillText(text, width / 2, height / 2);
    return canvas->toBuffer(std::string("image/png"));
};


std::shared_ptr<Buffer> generateMultiLineTextImage(string text, double width, double height, double fontSize)
{
    auto canvas = createCanvas(width, height);
    shared ctx = canvas->getContext(std::string("2d"));
    ctx->fillStyle = std::string("white");
    ctx->fillRect(0, 0, width, height);
    ctx->fillStyle = std::string("black");
    ctx->font = string_empty + fontSize + std::string("px Arial");
    ctx->textAlign = std::string("left");
    ctx->textBaseline = std::string("top");
    auto lines = text->split(std::string("\
"));
    shared lineHeight = fontSize * 1.2;
    shared startY = (height - lines->get_length() * lineHeight) / 2;
    lines->forEach([=](auto line, auto index) mutable
    {
        ctx->fillText(line, 20, startY + index * lineHeight);
    }
    );
    return canvas->toBuffer(std::string("image/png"));
};


std::shared_ptr<Buffer> generateComplexLayoutImage(double width, double height)
{
    auto canvas = createCanvas(width, height);
    auto ctx = canvas->getContext(std::string("2d"));
    ctx->fillStyle = std::string("white");
    ctx->fillRect(0, 0, width, height);
    ctx->fillStyle = std::string("navy");
    ctx->fillRect(0, 0, width, 60);
    ctx->fillStyle = std::string("white");
    ctx->font = std::string("24px Arial");
    ctx->textAlign = std::string("center");
    ctx->fillText(std::string("Application Header"), width / 2, 35);
    ctx->fillStyle = std::string("lightgray");
    ctx->fillRect(0, 60, 200, height - 60);
    ctx->fillStyle = std::string("black");
    ctx->font = std::string("16px Arial");
    ctx->textAlign = std::string("left");
    ctx->fillText(std::string("Menu Item 1"), 20, 100);
    ctx->fillText(std::string("Menu Item 2"), 20, 130);
    ctx->fillText(std::string("Menu Item 3"), 20, 160);
    ctx->fillStyle = std::string("black");
    ctx->font = std::string("18px Arial");
    ctx->fillText(std::string("Main Content Area"), 220, 100);
    ctx->fillText(std::string("Lorem ipsum dolor sit amet"), 220, 130);
    ctx->fillText(std::string("consectetur adipiscing elit"), 220, 160);
    ctx->fillStyle = std::string("blue");
    ctx->fillRect(220, 200, 100, 40);
    ctx->fillStyle = std::string("white");
    ctx->font = std::string("16px Arial");
    ctx->textAlign = std::string("center");
    ctx->fillText(std::string("Submit"), 270, 225);
    return canvas->toBuffer(std::string("image/png"));
};



void Main(void)
{
    vi->mock(std::string("@jitsi/robotjs"), [=]() mutable
    {
        return (object{
            object::pair{std::string("default"), object{
                object::pair{std::string("getScreenSize"), vi->fn([=]() mutable
                {
                    return (object{
                        object::pair{std::string("width"), 1920}, 
                        object::pair{std::string("height"), 1080}
                    });
                }
                )}, 
                object::pair{std::string("screen"), object{
                    object::pair{std::string("capture"), vi->fn([=]() mutable
                    {
                        return (object{
                            object::pair{std::string("image"), Buffer::from(std::string("mock-screenshot-data"))}, 
                            object::pair{std::string("width"), 1920}, 
                            object::pair{std::string("height"), 1080}, 
                            object::pair{std::string("byteWidth"), 7680}, 
                            object::pair{std::string("bitsPerPixel"), 32}, 
                            object::pair{std::string("bytesPerPixel"), 4}
                        });
                    }
                    )}
                }}, 
                object::pair{std::string("moveMouse"), vi->fn()}, 
                object::pair{std::string("mouseClick"), vi->fn()}, 
                object::pair{std::string("typeString"), vi->fn()}
            }}
        });
    }
    );
    describe(std::string("OCR Integration Tests with Real Images"), [=]() mutable
    {
        shared<std::shared_ptr<RobotService>> robotService;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            mockRuntime = as<any>(object{
                object::pair{std::string("useModel"), vi->fn()}, 
                object::pair{std::string("getService"), vi->fn()}, 
                object::pair{std::string("getAllServices"), vi->fn([=]() mutable
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
        describe(std::string("Real Image Generation and OCR"), [=]() mutable
        {
            it(std::string("should generate and process a simple text image"), [=]() mutable
            {
                auto testText = std::string("Hello World");
                auto imageBuffer = generateTextImage(testText, 400, 100);
                expect(imageBuffer)->toBeDefined();
                expect(Buffer::isBuffer(imageBuffer))->toBe(true);
                expect(imageBuffer->length)->toBeGreaterThan(0);
            }
            );
            it(std::string("should generate images with different text sizes"), [=]() mutable
            {
                auto texts = array<string>{ std::string("Small"), std::string("Medium Text"), std::string("Large Text Content") };
                auto sizes = array<double>{ 12, 24, 48 };
                for (auto i = 0; i < texts->get_length(); i++)
                {
                    auto imageBuffer = generateTextImage(const_(texts)[i], 400, 100, const_(sizes)[i]);
                    expect(imageBuffer->length)->toBeGreaterThan(0);
                }
            }
            );
            it(std::string("should generate multi-line text images"), [=]() mutable
            {
                auto multiLineText = std::string("Line 1\
Line 2\
Line 3");
                auto imageBuffer = generateMultiLineTextImage(multiLineText, 400, 200);
                expect(imageBuffer)->toBeDefined();
                expect(imageBuffer->length)->toBeGreaterThan(0);
            }
            );
            it(std::string("should generate images with special characters"), [=]() mutable
            {
                auto specialText = std::string("Email: test@example.com\
Phone: +1-555-123-4567\
Price: $29.99");
                auto imageBuffer = generateMultiLineTextImage(specialText, 500, 150);
                expect(imageBuffer)->toBeDefined();
                expect(imageBuffer->length)->toBeGreaterThan(0);
            }
            );
            it(std::string("should handle different image dimensions"), [=]() mutable
            {
                auto dimensions = array<object>{ object{
                    object::pair{std::string("width"), 200}, 
                    object::pair{std::string("height"), 100}
                }, object{
                    object::pair{std::string("width"), 800}, 
                    object::pair{std::string("height"), 200}
                }, object{
                    object::pair{std::string("width"), 1024}, 
                    object::pair{std::string("height"), 768}
                }, object{
                    object::pair{std::string("width"), 1920}, 
                    object::pair{std::string("height"), 1080}
                } };
                auto& __array2789_3015 = dimensions;
                for (auto __indx2789_3015 = 0_N; __indx2789_3015 < __array2789_3015->get_length(); __indx2789_3015++)
                {
                    auto& dim = const_(__array2789_3015)[__indx2789_3015];
                    {
                        auto imageBuffer = generateTextImage(std::string("Test Text"), dim["width"], dim["height"]);
                        expect(imageBuffer->length)->toBeGreaterThan(0);
                    }
                }
            }
            );
        }
        );
        describe(std::string("Image Downscaling Tests"), [=]() mutable
        {
            it(std::string("should downscale large images correctly"), [=]() mutable
            {
                auto largeImage = generateTextImage(std::string("Large Image Test"), 2048, 1536);
                auto downscaleImage = (as<any>(robotService))["downscaleImage"]["bind"](robotService);
                auto scaledImage = downscaleImage(largeImage, 1024);
                expect(scaledImage)->toBeDefined();
                expect(Buffer::isBuffer(scaledImage))->toBe(true);
                expect(scaledImage["length"])->toBeGreaterThan(0);
            }
            );
            it(std::string("should maintain image quality during downscaling"), [=]() mutable
            {
                auto originalImage = generateTextImage(std::string("Quality Test"), 1600, 1200);
                auto downscaleImage = (as<any>(robotService))["downscaleImage"]["bind"](robotService);
                auto scaledImage = downscaleImage(originalImage, 800);
                expect(originalImage->length)->toBeGreaterThan(0);
                expect(scaledImage["length"])->toBeGreaterThan(0);
            }
            );
        }
        );
        describe(std::string("Performance with Real Images"), [=]() mutable
        {
            it(std::string("should process small images quickly"), [=]() mutable
            {
                auto smallImage = generateTextImage(std::string("Quick Test"), 200, 100);
                auto startTime = Date->now();
                auto downscaleImage = (as<any>(robotService))["downscaleImage"]["bind"](robotService);
                auto result = downscaleImage(smallImage, 1024);
                auto duration = Date->now() - startTime;
                expect(duration)->toBeLessThan(100);
                expect(result)->toBeDefined();
            }
            );
            it(std::string("should handle multiple image processing requests"), [=]() mutable
            {
                auto images = Array(10)->fill(0)->map([=](auto _, auto i) mutable
                {
                    return generateTextImage(std::string("Test ") + i + string_empty, 300, 100);
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
        describe(std::string("Screen Context Integration"), [=]() mutable
        {
            it(std::string("should integrate real image processing with screen context"), [=]() mutable
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
