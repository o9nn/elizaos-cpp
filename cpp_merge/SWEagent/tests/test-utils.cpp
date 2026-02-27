#include "test-utils.test.h"

array<any> flattenArray(array<any> array)
{
    return array->reduce<array<any>>([=](auto flat, auto item) mutable
    {
        return flat->concat((Array->isArray(item)) ? any(flattenArray(as<array<any>>(item))) (item));
    }
    , array<any>());
};


boolean isEqual(any obj1, any obj2)
{
    if (obj1 == obj2) {
        return true;
    }
    if (OR((equals(obj1, nullptr)), (equals(obj2, nullptr)))) {
        return false;
    }
    if (type_of(obj1) != type_of(obj2)) {
        return false;
    }
    if (type_of(obj1) == std:("object")) {
        auto keys1 = Object->keys(as<any>(obj1));
        auto keys2 = Object->keys(as<any>(obj2));
        if (keys1->get_length() != keys2->get_length()) {
            return false;
        }
        return keys1->every([=](auto key) mutable
        {
            return isEqual(const_((as<any>(obj1)))[key], const_((as<any>(obj2)))[key]);
        }
        );
    }
    return false;
};


string REPO_ROOT = path->resolve(__dirname, std:(".."));

void Main(void)
{
    describe(std:("Utils"), [=]() mutable
    {
        describe(std:("Path conversion utilities"), [=]() mutable
        {
            it(std:("should convert relative path to absolute"), [=]() mutable
            {
                auto result = convertPathToAbspath(std:("subdir/file.txt"));
                expect(path->format(result))->toBe(path->join(REPO_ROOT, std:("subdir/file.txt")));
            }
            );
            it(std:("should keep absolute paths unchanged"), [=]() mutable
            {
                auto absolutePath = std:("/absolute/path/file.txt");
                auto result = convertPathToAbspath(absolutePath);
                expect(path->format(result))->toBe(absolutePath);
            }
            );
            it(std:("should handle Windows paths correctly"), [=]() mutable
            {
                if (process->platform == std:("win32")) {
                    auto windowsPath = std:("C:\Users\test\file.txt");
                    auto result = convertPathToAbspath(windowsPath);
                    expect(path->format(result))->toBe(windowsPath);
                }
            }
            );
            it(std:("should convert array of paths"), [=]() mutable
            {
                auto paths = array<string>{ std:("relative/path.txt"), std:("/absolute/path.txt"), std:("./current/dir.txt") };
                auto results = convertPathsToAbspath(paths);
                expect(path->format(const_(results)[0]))->toBe(path->join(REPO_ROOT, std:("relative/path.txt")));
                expect(path->format(const_(results)[1]))->toBe(std:("/absolute/path.txt"));
                expect(path->format(const_(results)[2]))->toBe(path->join(REPO_ROOT, std:("./current/dir.txt")));
            }
            );
            it(std:("should handle empty array"), [=]() mutable
            {
                auto results = convertPathsToAbspath(array<any>());
                expect(results)->toEqual(array<any>());
            }
            );
        }
        );
        describe(std:("Config utilities"), [=]() mutable
        {
            it(std:("should parse YAML config file"), [=]() mutable
            {
                auto yamlContent = std:("\
agent:\
  model:\
    name: test-model\
  temperature: 0.7\
");
                auto config = parseConfigFile(yamlContent, std:("yaml"));
                expect(config["agent"]["model"]["name"])->toBe(std:("test-model"));
                expect(config["agent"]["temperature"])->toBe(0.7);
            }
            );
            it(std:("should parse JSON config file"), [=]() mutable
            {
                auto jsonContent = std:("{\
        "agent": {\
          "model": {\
            "name": "test-model"\
          },\
          "temperature": 0.7\
        }\
      }");
                auto config = parseConfigFile(jsonContent, std:("json"));
                expect(config["agent"]["model"]["name"])->toBe(std:("test-model"));
                expect(config["agent"]["temperature"])->toBe(0.7);
            }
            );
            it(std:("should merge configs correctly"), [=]() mutable
            {
                auto baseConfig = object{
                    object::pair{std:("agent"), object{
                        object::pair{std:("model"), object{
                            object::pair{std:("name"), std:("base-model")}
                        }}, 
                        object::pair{std:("temperature"), 0.5}
                    }}, 
                    object::pair{std:("output_dir"), std:("/base/dir")}
                };
                auto overrideConfig = object{
                    object::pair{std:("agent"), object{
                        object::pair{std:("model"), object{
                            object::pair{std:("name"), std:("override-model")}
                        }}
                    }}, 
                    object::pair{std:("verbose"), true}
                };
                auto merged = mergeConfigs(baseConfig, overrideConfig);
                expect(merged["agent"]["model"]["name"])->toBe(std:("override-model"));
                expect(merged["agent"]["temperature"])->toBe(0.5);
                expect(merged["output_dir"])->toBe(std:("/base/dir"));
                expect(merged["verbose"])->toBe(true);
            }
            );
            it(std:("should validate config structure"), [=]() mutable
            {
                shared validConfig = object{
                    object::pair{std:("agent"), object{
                        object::pair{std:("model"), object{
                            object::pair{std:("name"), std:("test")}
                        }}
                    }}
                };
                expect([=]() mutable
                {
                    return validateConfig(validConfig);
                }
                )->not->toThrow();
                shared invalidConfig = object{
                    object::pair{std:("agent"), std:("not an object")}
                };
                expect([=]() mutable
                {
                    return validateConfig(invalidConfig);
                }
                )->toThrow();
            }
            );
        }
        );
        describe(std:("String utilities"), [=]() mutable
        {
            it(std:("should truncate long strings"), [=]() mutable
            {
                auto longString = std:("a")->repeat(100);
                auto truncated = truncateString(longString, 10);
                expect(truncated)->toBe(std:("aaaaaaa..."));
                expect(truncated->get_length())->toBeLessThanOrEqual(13);
            }
            );
            it(std:("should not truncate short strings"), [=]() mutable
            {
                auto shortString = std:("short");
                auto result = truncateString(shortString, 10);
                expect(result)->toBe(std:("short"));
            }
            );
            it(std:("should handle empty strings"), [=]() mutable
            {
                expect(truncateString(string_empty, 10))->toBe(string_empty);
            }
            );
        }
        );
        describe(std:("Time utilities"), [=]() mutable
        {
            it(std:("should format duration correctly"), [=]() mutable
            {
                expect(formatDuration(0))->toBe(std:("0s"));
                expect(formatDuration(45))->toBe(std:("45s"));
                expect(formatDuration(90))->toBe(std:("1m 30s"));
                expect(formatDuration(3661))->toBe(std:("1h 1m 1s"));
                expect(formatDuration(86400))->toBe(std:("1d 0h 0m 0s"));
            }
            );
            it(std:("should handle negative durations"), [=]() mutable
            {
                expect(formatDuration(-60))->toBe(std:("-1m 0s"));
            }
            );
        }
        );
        describe(std:("Hash utilities"), [=]() mutable
        {
            it(std:("should calculate consistent hashes"), [=]() mutable
            {
                auto data = std:("test data");
                auto hash1 = calculateHash(data);
                auto hash2 = calculateHash(data);
                expect(hash1)->toBe(hash2);
                expect(hash1)->toMatch((new RegExp(std:("^[a-f0-9]+"))));
            }
            );
            it(std:("should produce different hashes for different data"), [=]() mutable
            {
                auto hash1 = calculateHash(std:("data1"));
                auto hash2 = calculateHash(std:("data2"));
                expect(hash1)->not->toBe(hash2);
            }
            );
            it(std:("should handle objects"), [=]() mutable
            {
                auto obj = object{
                    object::pair{std:("key"), std:("value")}, 
                    object::pair{std:("nested"), object{
                        object::pair{std:("prop"), 1}
                    }}
                };
                auto hash = calculateHash(obj);
                expect(hash)->toBeDefined();
                expect(hash)->toMatch((new RegExp(std:("^[a-f0-9]+"))));
            }
            );
        }
        );
        describe(std:("Async utilities"), [=]() mutable
        {
            describe(std:("retry"), [=]() mutable
            {
                it(std:("should retry failed operations"), [=]() mutable
                {
                    shared attempts = 0;
                    auto operation = [=]() mutable
                    {
                        attempts++;
                        if (attempts < 3) {
                            throw any(std::make_shared<Error>(std:("Failed")));
                        }
                        return std:("success");
                    };
                    auto result = std::async([=]() { retry(operation, object{
                        object::pair{std:("retries"), 3}, 
                        object::pair{std:("delay"), 10}
                    }); });
                    expect(result)->toBe(std:("success"));
                    expect(attempts)->toBe(3);
                }
                );
                it(std:("should fail after max retries"), [=]() mutable
                {
                    auto operation = [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std:("Always fails")));
                    };
                    std::async([=]() { expect(retry(operation, object{
                        object::pair{std:("retries"), 2}, 
                        object::pair{std:("delay"), 10}
                    }))->rejects->toThrow(std:("Always fails")); });
                }
                );
                it(std:("should not retry on success"), [=]() mutable
                {
                    shared attempts = 0;
                    auto operation = [=]() mutable
                    {
                        attempts++;
                        return std:("immediate success");
                    };
                    auto result = std::async([=]() { retry(operation); });
                    expect(result)->toBe(std:("immediate success"));
                    expect(attempts)->toBe(1);
                }
                );
            }
            );
            describe(std:("debounce"), [=]() mutable
            {
                it(std:("should debounce function calls"), [=]() mutable
                {
                    jest->useFakeTimers();
                    shared callCount = 0;
                    auto fn = [=]() mutable
                    {
                        return callCount++;
                    };
                    auto debounced = debounce(fn, 100);
                    debounced();
                    debounced();
                    debounced();
                    expect(callCount)->toBe(0);
                    jest->advanceTimersByTime(100);
                    expect(callCount)->toBe(1);
                    jest->useRealTimers();
                }
                );
            }
            );
            describe(std:("throttle"), [=]() mutable
            {
                it(std:("should throttle function calls"), [=]() mutable
                {
                    jest->useFakeTimers();
                    shared callCount = 0;
                    auto fn = [=]() mutable
                    {
                        return callCount++;
                    };
                    auto throttled = throttle(fn, 100);
                    throttled();
                    expect(callCount)->toBe(1);
                    throttled();
                    throttled();
                    expect(callCount)->toBe(1);
                    jest->advanceTimersByTime(100);
                    throttled();
                    expect(callCount)->toBe(2);
                    jest->useRealTimers();
                }
                );
            }
            );
        }
        );
        describe(std:("Environment utilities"), [=]() mutable
        {
            it(std:("should parse environment variables"), [=]() mutable
            {
                process->env->TEST_VAR = std:("test_value");
                process->env->TEST_NUMBER = std:("42");
                process->env->TEST_BOOL = std:("true");
                expect(process->env->TEST_VAR)->toBe(std:("test_value"));
                expect(parseInt(process->env->TEST_NUMBER))->toBe(42);
                expect(process->env->TEST_BOOL == std:("true"))->toBe(true);
                process->env.Delete("TEST_VAR");
                process->env.Delete("TEST_NUMBER");
                process->env.Delete("TEST_BOOL");
            }
            );
            it(std:("should handle missing environment variables"), [=]() mutable
            {
                expect(process->env->NONEXISTENT_VAR)->toBeUndefined();
            }
            );
        }
        );
        describe(std:("Array utilities"), [=]() mutable
        {
            it(std:("should chunk arrays"), [=]() mutable
            {
                auto array = array<double>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
                auto chunks = chunkArray(array, 3);
                expect(chunks)->toEqual(array<array<double>>{ array<double>{ 1, 2, 3 }, array<double>{ 4, 5, 6 }, array<double>{ 7, 8, 9 }, array<double>{ 10 } });
            }
            );
            it(std:("should flatten nested arrays"), [=]() mutable
            {
                auto nested = array<array<double>>{ array<double>{ 1, 2 }, array<double>{ 3, array<double>{ 4, 5 } }, 6 };
                auto flattened = flattenArray(nested);
                expect(flattened)->toEqual(array<double>{ 1, 2, 3, 4, 5, 6 });
            }
            );
            it(std:("should remove duplicates"), [=]() mutable
            {
                auto array = array<double>{ 1, 2, 2, 3, 3, 3, 4 };
                auto unique = removeDuplicates(array);
                expect(unique)->toEqual(array<double>{ 1, 2, 3, 4 });
            }
            );
        }
        );
        describe(std:("Object utilities"), [=]() mutable
        {
            it(std:("should deep clone objects"), [=]() mutable
            {
                auto original = object{
                    object::pair{std:("a"), 1}, 
                    object::pair{std:("b"), object{
                        object::pair{std:("c"), 2}, 
                        object::pair{std:("d"), array<double>{ 3, 4 }}
                    }}, 
                    object::pair{std:("e"), std::make_shared<Date>()}
                };
                auto cloned = deepClone(original);
                expect(cloned)->toEqual(original);
                expect(cloned)->not->toBe(original);
                expect(cloned["b"])->not->toBe(original["b"]);
                expect(cloned["b"]["d"])->not->toBe(original["b"]["d"]);
            }
            );
            it(std:("should check object equality"), [=]() mutable
            {
                auto obj1 = object{
                    object::pair{std:("a"), 1}, 
                    object::pair{std:("b"), object{
                        object::pair{std:("c"), 2}
                    }}
                };
                auto obj2 = object{
                    object::pair{std:("a"), 1}, 
                    object::pair{std:("b"), object{
                        object::pair{std:("c"), 2}
                    }}
                };
                auto obj3 = object{
                    object::pair{std:("a"), 1}, 
                    object::pair{std:("b"), object{
                        object::pair{std:("c"), 3}
                    }}
                };
                expect(isEqual(obj1, obj2))->toBe(true);
                expect(isEqual(obj1, obj3))->toBe(false);
            }
            );
            it(std:("should pick properties from object"), [=]() mutable
            {
                auto obj = object{
                    object::pair{std:("a"), 1}, 
                    object::pair{std:("b"), 2}, 
                    object::pair{std:("c"), 3}, 
                    object::pair{std:("d"), 4}
                };
                auto picked = pick(obj, array<string>{ std:("a"), std:("c") });
                expect(picked)->toEqual(object{
                    object::pair{std:("a"), 1}, 
                    object::pair{std:("c"), 3}
                });
            }
            );
            it(std:("should omit properties from object"), [=]() mutable
            {
                auto obj = object{
                    object::pair{std:("a"), 1}, 
                    object::pair{std:("b"), 2}, 
                    object::pair{std:("c"), 3}, 
                    object::pair{std:("d"), 4}
                };
                auto omitted = omit(obj, array<string>{ std:("b"), std:("d") });
                expect(omitted)->toEqual(object{
                    object::pair{std:("a"), 1}, 
                    object::pair{std:("c"), 3}
                });
            }
            );
        }
        );
    }
    );
}

MAIN
