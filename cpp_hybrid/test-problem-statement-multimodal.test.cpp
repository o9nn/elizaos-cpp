#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-problem-statement-multimodal.test.h"

any mockedChildProcess = as<std::shared_ptr<jest::Mocked<child_process>>>(child_process);

void Main(void)
{
    jest->mock(std::string("child_process"));
    describe(std::string("SWEBenchMultimodalProblemStatement"), [=]() mutable
    {
        shared exampleImageUrl = std::string("https://upload.wikimedia.org/wikipedia/commons/thumb/4/4c/Candide1759.jpg/330px-Candide1759.jpg");
        beforeEach([=]() mutable
        {
            jest->clearAllMocks();
        }
        );
        describe(std::string("Initialization"), [=]() mutable
        {
            it(std::string("should initialize with basic properties"), [=]() mutable
            {
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<string>{ exampleImageUrl }}, 
                    object::pair{std::string("id"), std::string("test_id")}
                });
                expect(problemStatement->text)->toBe(std::string("Test problem statement"));
                expect(problemStatement->issueImages)->toEqual(array<string>{ exampleImageUrl });
                expect(problemStatement->id)->toBe(std::string("test_id"));
                expect(problemStatement->type)->toBe(std::string("swe_bench_multimodal"));
            }
            );
            it(std::string("should work with empty images array"), [=]() mutable
            {
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<any>()}
                });
                expect(problemStatement->issueImages)->toEqual(array<any>());
            }
            );
        }
        );
        describe(std::string("Getting problem statement"), [=]() mutable
        {
            it(std::string("should return text when no images present"), [=]() mutable
            {
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<any>()}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toBe(std::string("Test problem statement"));
            }
            );
            it(std::string("should handle valid image with successful download"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std::string("string")), (command->includes(std::string("-I"))))) {
                        return std::string("HTTP/1.1 200 OK
\
Content-Type: image/png
\
Content-Length: 1234
\

\
");
                    }
                    return string_empty;
                }
                )));
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std::string("string")), (command->includes(std::string("base64"))))) {
                        return std::string("ZmFrZV9pbWFnZV9kYXRh");
                    }
                    return string_empty;
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<string>{ exampleImageUrl }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toContain(std::string("Test problem statement"));
                expect(result)->toContain(std::string("![") + exampleImageUrl + std::string("](data:image/png;base64,"));
                expect(mockedChildProcess->execSync)->toHaveBeenCalledTimes(2);
            }
            );
            it(std::string("should handle network errors gracefully"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=]() mutable
                {
                    throw any(std::make_shared<Error>(std::string("Network error")));
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<string>{ exampleImageUrl }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toBe(std::string("Test problem statement"));
                expect(mockedChildProcess->execSync)->toHaveBeenCalled();
            }
            );
            it(std::string("should reject invalid MIME types"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std::string("string")), (command->includes(std::string("-I"))))) {
                        return std::string("HTTP/1.1 200 OK
\
Content-Type: text/html
\
Content-Length: 1234
\

\
");
                    }
                    return string_empty;
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<string>{ std::string("http://example.com/document.html") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toBe(std::string("Test problem statement"));
            }
            );
            it(std::string("should cache results and not re-download images"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std::string("string")), (command->includes(std::string("-I"))))) {
                        return std::string("HTTP/1.1 200 OK
\
Content-Type: image/png
\
Content-Length: 1234
\

\
");
                    }
                    return string_empty;
                }
                )));
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std::string("string")), (command->includes(std::string("base64"))))) {
                        return std::string("ZmFrZV9pbWFnZV9kYXRh");
                    }
                    return string_empty;
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<string>{ exampleImageUrl }}
                });
                auto result1 = problemStatement->getProblemStatement();
                auto result2 = problemStatement->getProblemStatement();
                expect(mockedChildProcess->execSync)->toHaveBeenCalledTimes(2);
                expect(result1)->toBe(result2);
                expect(result1)->toContain(std::string("Test problem statement"));
                expect(result1)->toContain(std::string("![") + exampleImageUrl + std::string("](data:image/png;base64,"));
            }
            );
            it(std::string("should handle invalid URLs gracefully"), [=]() mutable
            {
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<string>{ std::string("not_a_url"), std::string("ftp://invalid_scheme.com/image.png") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toBe(std::string("Test problem statement"));
                expect(mockedChildProcess->execSync)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should reject large images"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std::string("string")), (command->includes(std::string("-I"))))) {
                        return std::string("HTTP/1.1 200 OK
\
Content-Type: image/png
\
Content-Length: 20971520
\

\
");
                    }
                    return string_empty;
                }
                )))->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std::string("string")), (command->includes(std::string("base64"))))) {
                        throw any(std::make_shared<Error>(std::string("curl: (63) Maximum file size exceeded")));
                    }
                    return string_empty;
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<string>{ std::string("http://example.com/huge_image.png") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toBe(std::string("Test problem statement"));
                expect(mockedChildProcess->execSync)->toHaveBeenCalledTimes(2);
            }
            );
            it(std::string("should handle multiple images"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((AND((type_of(command) == std::string("string")), (command->includes(std::string("-I"))))), (command->includes(std::string("image1.png"))))) {
                        return std::string("HTTP/1.1 200 OK
\
Content-Type: image/png
\
Content-Length: 1234
\

\
");
                    }
                    return string_empty;
                }
                )))->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((AND((type_of(command) == std::string("string")), (command->includes(std::string("base64"))))), (command->includes(std::string("image1.png"))))) {
                        return std::string("aW1hZ2UxX2RhdGE=");
                    }
                    return string_empty;
                }
                )))->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((AND((type_of(command) == std::string("string")), (command->includes(std::string("-I"))))), (command->includes(std::string("image2.jpg"))))) {
                        return std::string("HTTP/1.1 200 OK
\
Content-Type: image/jpeg
\
Content-Length: 1234
\

\
");
                    }
                    return string_empty;
                }
                )))->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((AND((type_of(command) == std::string("string")), (command->includes(std::string("base64"))))), (command->includes(std::string("image2.jpg"))))) {
                        return std::string("aW1hZ2UyX2RhdGE=");
                    }
                    return string_empty;
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<string>{ std::string("http://example.com/image1.png"), std::string("http://example.com/image2.jpg") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toContain(std::string("Test problem statement"));
                expect(result)->toContain(std::string("![http://example.com/image1.png](data:image/png;base64,"));
                expect(result)->toContain(std::string("![http://example.com/image2.jpg](data:image/jpeg;base64,"));
                expect(mockedChildProcess->execSync)->toHaveBeenCalledTimes(4);
            }
            );
            it(std::string("should handle mixed valid and invalid images"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((AND((type_of(command) == std::string("string")), (command->includes(std::string("-I"))))), (command->includes(std::string("valid.png"))))) {
                        return std::string("HTTP/1.1 200 OK
\
Content-Type: image/png
\
Content-Length: 1234
\

\
");
                    }
                    return string_empty;
                }
                )))->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((AND((type_of(command) == std::string("string")), (command->includes(std::string("base64"))))), (command->includes(std::string("valid.png"))))) {
                        return std::string("dmFsaWRfaW1hZ2U=");
                    }
                    return string_empty;
                }
                )))->mockImplementationOnce(as<any>(([=](auto _command, auto _options = undefined) mutable
                {
                    throw any(std::make_shared<Error>(std::string("Failed to load")));
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<string>{ std::string("http://example.com/valid.png"), std::string("http://example.com/invalid.png") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toContain(std::string("Test problem statement"));
                expect(result)->toContain(std::string("![http://example.com/valid.png](data:image/png;base64,"));
                expect(result)->not->toContain(std::string("http://example.com/invalid.png"));
                expect(mockedChildProcess->execSync)->toHaveBeenCalledTimes(3);
            }
            );
            it(std::string("should handle HTTP error status codes"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std::string("string")), (command->includes(std::string("-I"))))) {
                        throw any(std::make_shared<Error>(std::string("curl: (22) The requested URL returned error: 404 Not Found")));
                    }
                    return string_empty;
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<string>{ std::string("http://example.com/missing.png") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toBe(std::string("Test problem statement"));
            }
            );
            it(std::string("should validate image URLs before attempting download"), [=]() mutable
            {
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std::string("text"), std::string("Test problem statement")}, 
                    object::pair{std::string("issueImages"), array<string>{ string_empty, std::string("javascript:alert(1)"), std::string("data:image/png;base64,abc"), std::string("file:///etc/passwd") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(mockedChildProcess->execSync)->not->toHaveBeenCalled();
                expect(result)->toBe(std::string("Test problem statement"));
            }
            );
        }
        );
    }
    );
}

MAIN
