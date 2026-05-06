#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/services/__tests__/env-file.service.test.h"

object mockFs = object{
    object::pair{std::string("promises"), object{
        object::pair{std::string("readFile"), mock([=]() mutable
        {
            return Promise->resolve(string_empty);
        }
        )}, 
        object::pair{std::string("writeFile"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("mkdir"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("copyFile"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}
    }}, 
    object::pair{std::string("existsSync"), mock([=]() mutable
    {
        return true;
    }
    )}
};
object mockLogger = object{
    object::pair{std::string("info"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std::string("error"), mock([=]() mutable
    {
    }
    )}
};

void Main(void)
{
    mock->module(std::string("node:fs"), [=]() mutable
    {
        return mockFs;
    }
    );
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), mockLogger}
        });
    }
    );
    mock->module(std::string("@/src/utils"), [=]() mutable
    {
        return (object{
            object::pair{std::string("UserEnvironment"), object{
                object::pair{std::string("getInstanceInfo"), mock([=]() mutable
                {
                    return Promise->resolve(object{
                        object::pair{std::string("paths"), object{
                            object::pair{std::string("envFilePath"), std::string("/test/.env")}
                        }}
                    });
                }
                )}
            }}
        });
    }
    );
    describe(std::string("EnvFileService"), [=]() mutable
    {
        shared<std::shared_ptr<EnvFileService>> service;
        beforeEach([=]() mutable
        {
            service = std::make_shared<EnvFileService>(std::string("/test/.env"));
            Object->values(mockFs["promises"])->forEach([=](auto fn) mutable
            {
                return fn["mockClear"]();
            }
            );
            mockFs["existsSync"]->mockClear();
            mockLogger["info"]->mockClear();
            mockLogger["error"]->mockClear();
        }
        );
        describe(std::string("write method"), [=]() mutable
        {
            it(std::string("should not mutate the input vars object"), [=]() mutable
            {
                auto mockExistingContent = std::string("EXISTING_KEY=existing_value\
");
                mockFs["existsSync"]->mockReturnValue(true);
                mockFs["promises"]["readFile"]->mockResolvedValue(mockExistingContent);
                mockFs["promises"]["writeFile"]->mockResolvedValue(undefined);
                auto originalVars = object{
                    object::pair{std::string("EXISTING_KEY"), std::string("new_value")}, 
                    object::pair{std::string("NEW_KEY"), std::string("new_key_value")}
                };
                auto varsCopy = utils::assign(object{
                }, originalVars);
                std::async([=]() { service->write(originalVars, object{
                    object::pair{std::string("preserveComments"), true}
                }); });
                expect(originalVars)->toEqual(varsCopy);
                expect(Object->keys(originalVars))->toHaveLength(2);
                expect(originalVars["EXISTING_KEY"])->toBe(std::string("new_value"));
                expect(originalVars["NEW_KEY"])->toBe(std::string("new_key_value"));
            }
            );
            it(std::string("should handle objects with overridden hasOwnProperty"), [=]() mutable
            {
                mockFs["existsSync"]->mockReturnValue(true);
                mockFs["promises"]["readFile"]->mockResolvedValue(string_empty);
                mockFs["promises"]["writeFile"]->mockResolvedValue(undefined);
                auto maliciousVars = Object->create(nullptr);
                maliciousVars["TEST_KEY"] = std::string("test_value");
                maliciousVars["hasOwnProperty"] = [=]() mutable
                {
                    throw any(std::make_shared<Error>(std::string("hasOwnProperty was called directly!")));
                };
                auto error = nullptr;
                try
                {
                    std::async([=]() { service->write(maliciousVars); });
                }
                catch (const any& e)
                {
                    error = as<std::shared_ptr<Error>>(e);
                }
                expect(error)->toBeNull();
                expect(mockFs["promises"]["writeFile"])->toHaveBeenCalled();
                auto calls = as<array<any>>(mockFs["promises"]["writeFile"]->mock->calls);
                expect(calls->get_length())->toBe(1);
                expect(const_(calls)[0])->toEqual(array<string>{ std::string("/test/.env"), std::string("TEST_KEY=test_value\
"), std::string("utf-8") });
            }
            );
        }
        );
        describe(std::string("exists method"), [=]() mutable
        {
            it(std::string("should handle objects with overridden hasOwnProperty"), [=]() mutable
            {
                auto mockContent = std::string("TEST_KEY=test_value\
hasOwnProperty=malicious\
");
                mockFs["existsSync"]->mockReturnValue(true);
                mockFs["promises"]["readFile"]->mockResolvedValue(mockContent);
                auto existsTestKey = std::async([=]() { service->exists(std::string("TEST_KEY")); });
                auto existsNonExistent = std::async([=]() { service->exists(std::string("NON_EXISTENT")); });
                auto existsHasOwnProperty = std::async([=]() { service->exists(std::string("hasOwnProperty")); });
                expect(existsTestKey)->toBe(true);
                expect(existsNonExistent)->toBe(false);
                expect(existsHasOwnProperty)->toBe(true);
            }
            );
            it(std::string("should handle null prototype objects"), [=]() mutable
            {
                auto originalRead = std::bind(&EnvFileService::read, service);
                std::bind(&EnvFileService::read, service) = mock([=]() mutable
                {
                    return Promise->resolve(Object->assign(Object->create(nullptr), object{
                        object::pair{std::string("TEST_KEY"), std::string("test_value")}
                    }));
                }
                );
                auto exists = std::async([=]() { service->exists(std::string("TEST_KEY")); });
                expect(exists)->toBe(true);
                auto notExists = std::async([=]() { service->exists(std::string("NON_EXISTENT")); });
                expect(notExists)->toBe(false);
                std::bind(&EnvFileService::read, service) = originalRead;
            }
            );
        }
        );
    }
    );
}

MAIN
