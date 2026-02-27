#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-problem-statement-multimodal.test.h"

any mockedChildProcess = as<std::shared_ptr<jest::Mocked<child_process>>>(child_process);

void Main(void)
{
    jest->mock(std:("child_process"));
    describe(std:("SWEBenchMultimodalProblemStatement"), [=]() mutable
    {
        shared exampleImageUrl = std:("https://upload.wikimedia.org/wikipedia/commons/thumb/4/4c/Candide1759.jpg/330px-Candide1759.jpg");
        beforeEach([=]() mutable
        {
            jest->clearAllMocks();
        }
        );
        describe(std:("Initialization"), [=]() mutable
        {
            it(std:("should initialize with basic properties"), [=]() mutable
            {
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<string>{ exampleImageUrl }}, 
                    object::pair{std:("id"), std:("test_id")}
                });
                expect(problemStatement->text)->toBe(std:("Test problem statement"));
                expect(problemStatement->issueImages)->toEqual(array<string>{ exampleImageUrl });
                expect(problemStatement->id)->toBe(std:("test_id"));
                expect(problemStatement->type)->toBe(std:("swe_bench_multimodal"));
            }
            );
            it(std:("should work with empty images array"), [=]() mutable
            {
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<any>()}
                });
                expect(problemStatement->issueImages)->toEqual(array<any>());
            }
            );
        }
        );
        describe(std:("Getting problem statement"), [=]() mutable
        {
            it(std:("should return text when no images present"), [=]() mutable
            {
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<any>()}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toBe(std:("Test problem statement"));
            }
            );
            it(std:("should handle valid image with successful download"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std:("string")), (command->includes(std:("-I"))))) {
                        return std:("HTTP/1.1 200 OK
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
                    if (AND((type_of(command) == std:("string")), (command->includes(std:("base64"))))) {
                        return std:("ZmFrZV9pbWFnZV9kYXRh");
                    }
                    return string_empty;
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<string>{ exampleImageUrl }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toContain(std:("Test problem statement"));
                expect(result)->toContain(std:("![") + exampleImageUrl + std:("](data:image/png;base64,"));
                expect(mockedChildProcess->execSync)->toHaveBeenCalledTimes(2);
            }
            );
            it(std:("should handle network errors gracefully"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=]() mutable
                {
                    throw any(std::make_shared<Error>(std:("Network error")));
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<string>{ exampleImageUrl }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toBe(std:("Test problem statement"));
                expect(mockedChildProcess->execSync)->toHaveBeenCalled();
            }
            );
            it(std:("should reject invalid MIME types"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std:("string")), (command->includes(std:("-I"))))) {
                        return std:("HTTP/1.1 200 OK
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
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<string>{ std:("http://example.com/document.html") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toBe(std:("Test problem statement"));
            }
            );
            it(std:("should cache results and not re-download images"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std:("string")), (command->includes(std:("-I"))))) {
                        return std:("HTTP/1.1 200 OK
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
                    if (AND((type_of(command) == std:("string")), (command->includes(std:("base64"))))) {
                        return std:("ZmFrZV9pbWFnZV9kYXRh");
                    }
                    return string_empty;
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<string>{ exampleImageUrl }}
                });
                auto result1 = problemStatement->getProblemStatement();
                auto result2 = problemStatement->getProblemStatement();
                expect(mockedChildProcess->execSync)->toHaveBeenCalledTimes(2);
                expect(result1)->toBe(result2);
                expect(result1)->toContain(std:("Test problem statement"));
                expect(result1)->toContain(std:("![") + exampleImageUrl + std:("](data:image/png;base64,"));
            }
            );
            it(std:("should handle invalid URLs gracefully"), [=]() mutable
            {
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<string>{ std:("not_a_url"), std:("ftp://invalid_scheme.com/image.png") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toBe(std:("Test problem statement"));
                expect(mockedChildProcess->execSync)->not->toHaveBeenCalled();
            }
            );
            it(std:("should reject large images"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std:("string")), (command->includes(std:("-I"))))) {
                        return std:("HTTP/1.1 200 OK
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
                    if (AND((type_of(command) == std:("string")), (command->includes(std:("base64"))))) {
                        throw any(std::make_shared<Error>(std:("curl: (63) Maximum file size exceeded")));
                    }
                    return string_empty;
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<string>{ std:("http://example.com/huge_image.png") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toBe(std:("Test problem statement"));
                expect(mockedChildProcess->execSync)->toHaveBeenCalledTimes(2);
            }
            );
            it(std:("should handle multiple images"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((AND((type_of(command) == std:("string")), (command->includes(std:("-I"))))), (command->includes(std:("image1.png"))))) {
                        return std:("HTTP/1.1 200 OK
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
                    if (AND((AND((type_of(command) == std:("string")), (command->includes(std:("base64"))))), (command->includes(std:("image1.png"))))) {
                        return std:("aW1hZ2UxX2RhdGE=");
                    }
                    return string_empty;
                }
                )))->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((AND((type_of(command) == std:("string")), (command->includes(std:("-I"))))), (command->includes(std:("image2.jpg"))))) {
                        return std:("HTTP/1.1 200 OK
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
                    if (AND((AND((type_of(command) == std:("string")), (command->includes(std:("base64"))))), (command->includes(std:("image2.jpg"))))) {
                        return std:("aW1hZ2UyX2RhdGE=");
                    }
                    return string_empty;
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<string>{ std:("http://example.com/image1.png"), std:("http://example.com/image2.jpg") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toContain(std:("Test problem statement"));
                expect(result)->toContain(std:("![http://example.com/image1.png](data:image/png;base64,"));
                expect(result)->toContain(std:("![http://example.com/image2.jpg](data:image/jpeg;base64,"));
                expect(mockedChildProcess->execSync)->toHaveBeenCalledTimes(4);
            }
            );
            it(std:("should handle mixed valid and invalid images"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((AND((type_of(command) == std:("string")), (command->includes(std:("-I"))))), (command->includes(std:("valid.png"))))) {
                        return std:("HTTP/1.1 200 OK
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
                    if (AND((AND((type_of(command) == std:("string")), (command->includes(std:("base64"))))), (command->includes(std:("valid.png"))))) {
                        return std:("dmFsaWRfaW1hZ2U=");
                    }
                    return string_empty;
                }
                )))->mockImplementationOnce(as<any>(([=](auto _command, auto _options = undefined) mutable
                {
                    throw any(std::make_shared<Error>(std:("Failed to load")));
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<string>{ std:("http://example.com/valid.png"), std:("http://example.com/invalid.png") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toContain(std:("Test problem statement"));
                expect(result)->toContain(std:("![http://example.com/valid.png](data:image/png;base64,"));
                expect(result)->not->toContain(std:("http://example.com/invalid.png"));
                expect(mockedChildProcess->execSync)->toHaveBeenCalledTimes(3);
            }
            );
            it(std:("should handle HTTP error status codes"), [=]() mutable
            {
                mockedChildProcess->execSync->mockImplementationOnce(as<any>(([=](auto command, auto _options = undefined) mutable
                {
                    if (AND((type_of(command) == std:("string")), (command->includes(std:("-I"))))) {
                        throw any(std::make_shared<Error>(std:("curl: (22) The requested URL returned error: 404 Not Found")));
                    }
                    return string_empty;
                }
                )));
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<string>{ std:("http://example.com/missing.png") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(result)->toBe(std:("Test problem statement"));
            }
            );
            it(std:("should validate image URLs before attempting download"), [=]() mutable
            {
                auto problemStatement = std::make_shared<SWEBenchMultimodalProblemStatement>(object{
                    object::pair{std:("text"), std:("Test problem statement")}, 
                    object::pair{std:("issueImages"), array<string>{ string_empty, std:("javascript:alert(1)"), std:("data:image/png;base64,abc"), std:("file:///etc/passwd") }}
                });
                auto result = problemStatement->getProblemStatement();
                expect(mockedChildProcess->execSync)->not->toHaveBeenCalled();
                expect(result)->toBe(std:("Test problem statement"));
            }
            );
        }
        );
    }
    );
}

MAIN
