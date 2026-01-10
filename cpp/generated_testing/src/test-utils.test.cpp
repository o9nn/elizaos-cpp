#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-utils.test.h"

array<any> flattenArray(array<any> array)
{
    return array->reduce<array<any>>([=](auto flat, auto item) mutable
    {
        return flat->concat((Array->isArray(item)) ? any(flattenArray(as<array<any>>(item))) : any(item));
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
    if (type_of(obj1) == std::string("object")) {
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


string REPO_ROOT = path->resolve(__dirname, std::string(".."));

void Main(void)
{
    describe(std::string("Utils"), [=]() mutable
    {
        describe(std::string("Path conversion utilities"), [=]() mutable
        {
            it(std::string("should convert relative path to absolute"), [=]() mutable
            {
                auto result = convertPathToAbspath(std::string("subdir/file.txt"));
                expect(path->format(result))->toBe(path->join(REPO_ROOT, std::string("subdir/file.txt")));
            }
            );
            it(std::string("should keep absolute paths unchanged"), [=]() mutable
            {
                auto absolutePath = std::string("/absolute/path/file.txt");
                auto result = convertPathToAbspath(absolutePath);
                expect(path->format(result))->toBe(absolutePath);
            }
            );
            it(std::string("should handle Windows paths correctly"), [=]() mutable
            {
                if (process->platform == std::string("win32")) {
                    auto windowsPath = std::string("C:\Users\test\file.txt");
                    auto result = convertPathToAbspath(windowsPath);
                    expect(path->format(result))->toBe(windowsPath);
                }
            }
            );
            it(std::string("should convert array of paths"), [=]() mutable
            {
                auto paths = array<string>{ std::string("relative/path.txt"), std::string("/absolute/path.txt"), std::string("./current/dir.txt") };
                auto results = convertPathsToAbspath(paths);
                expect(path->format(const_(results)[0]))->toBe(path->join(REPO_ROOT, std::string("relative/path.txt")));
                expect(path->format(const_(results)[1]))->toBe(std::string("/absolute/path.txt"));
                expect(path->format(const_(results)[2]))->toBe(path->join(REPO_ROOT, std::string("./current/dir.txt")));
            }
            );
            it(std::string("should handle empty array"), [=]() mutable
            {
                auto results = convertPathsToAbspath(array<any>());
                expect(results)->toEqual(array<any>());
            }
            );
        }
        );
        describe(std::string("Config utilities"), [=]() mutable
        {
            it(std::string("should parse YAML config file"), [=]() mutable
            {
                auto yamlContent = std::string("\
agent:\
  model:\
    name: test-model\
  temperature: 0.7\
");
                auto config = parseConfigFile(yamlContent, std::string("yaml"));
                expect(config["agent"]["model"]["name"])->toBe(std::string("test-model"));
                expect(config["agent"]["temperature"])->toBe(0.7);
            }
            );
            it(std::string("should parse JSON config file"), [=]() mutable
            {
                auto jsonContent = std::string("{\
        "agent": {\
          "model": {\
            "name": "test-model"\
          },\
          "temperature": 0.7\
        }\
      }");
                auto config = parseConfigFile(jsonContent, std::string("json"));
                expect(config["agent"]["model"]["name"])->toBe(std::string("test-model"));
                expect(config["agent"]["temperature"])->toBe(0.7);
            }
            );
            it(std::string("should merge configs correctly"), [=]() mutable
            {
                auto baseConfig = object{
                    object::pair{std::string("agent"), object{
                        object::pair{std::string("model"), object{
                            object::pair{std::string("name"), std::string("base-model")}
                        }}, 
                        object::pair{std::string("temperature"), 0.5}
                    }}, 
                    object::pair{std::string("output_dir"), std::string("/base/dir")}
                };
                auto overrideConfig = object{
                    object::pair{std::string("agent"), object{
                        object::pair{std::string("model"), object{
                            object::pair{std::string("name"), std::string("override-model")}
                        }}
                    }}, 
                    object::pair{std::string("verbose"), true}
                };
                auto merged = mergeConfigs(baseConfig, overrideConfig);
                expect(merged["agent"]["model"]["name"])->toBe(std::string("override-model"));
                expect(merged["agent"]["temperature"])->toBe(0.5);
                expect(merged["output_dir"])->toBe(std::string("/base/dir"));
                expect(merged["verbose"])->toBe(true);
            }
            );
            it(std::string("should validate config structure"), [=]() mutable
            {
                shared validConfig = object{
                    object::pair{std::string("agent"), object{
                        object::pair{std::string("model"), object{
                            object::pair{std::string("name"), std::string("test")}
                        }}
                    }}
                };
                expect([=]() mutable
                {
                    return validateConfig(validConfig);
                }
                )->not->toThrow();
                shared invalidConfig = object{
                    object::pair{std::string("agent"), std::string("not an object")}
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
        describe(std::string("String utilities"), [=]() mutable
        {
            it(std::string("should truncate long strings"), [=]() mutable
            {
                auto longString = std::string("a")->repeat(100);
                auto truncated = truncateString(longString, 10);
                expect(truncated)->toBe(std::string("aaaaaaa..."));
                expect(truncated->get_length())->toBeLessThanOrEqual(13);
            }
            );
            it(std::string("should not truncate short strings"), [=]() mutable
            {
                auto shortString = std::string("short");
                auto result = truncateString(shortString, 10);
                expect(result)->toBe(std::string("short"));
            }
            );
            it(std::string("should handle empty strings"), [=]() mutable
            {
                expect(truncateString(string_empty, 10))->toBe(string_empty);
            }
            );
        }
        );
        describe(std::string("Time utilities"), [=]() mutable
        {
            it(std::string("should format duration correctly"), [=]() mutable
            {
                expect(formatDuration(0))->toBe(std::string("0s"));
                expect(formatDuration(45))->toBe(std::string("45s"));
                expect(formatDuration(90))->toBe(std::string("1m 30s"));
                expect(formatDuration(3661))->toBe(std::string("1h 1m 1s"));
                expect(formatDuration(86400))->toBe(std::string("1d 0h 0m 0s"));
            }
            );
            it(std::string("should handle negative durations"), [=]() mutable
            {
                expect(formatDuration(-60))->toBe(std::string("-1m 0s"));
            }
            );
        }
        );
        describe(std::string("Hash utilities"), [=]() mutable
        {
            it(std::string("should calculate consistent hashes"), [=]() mutable
            {
                auto data = std::string("test data");
                auto hash1 = calculateHash(data);
                auto hash2 = calculateHash(data);
                expect(hash1)->toBe(hash2);
                expect(hash1)->toMatch((new RegExp(std::string("^[a-f0-9]+"))));
            }
            );
            it(std::string("should produce different hashes for different data"), [=]() mutable
            {
                auto hash1 = calculateHash(std::string("data1"));
                auto hash2 = calculateHash(std::string("data2"));
                expect(hash1)->not->toBe(hash2);
            }
            );
            it(std::string("should handle objects"), [=]() mutable
            {
                auto obj = object{
                    object::pair{std::string("key"), std::string("value")}, 
                    object::pair{std::string("nested"), object{
                        object::pair{std::string("prop"), 1}
                    }}
                };
                auto hash = calculateHash(obj);
                expect(hash)->toBeDefined();
                expect(hash)->toMatch((new RegExp(std::string("^[a-f0-9]+"))));
            }
            );
        }
        );
        describe(std::string("Async utilities"), [=]() mutable
        {
            describe(std::string("retry"), [=]() mutable
            {
                it(std::string("should retry failed operations"), [=]() mutable
                {
                    shared attempts = 0;
                    auto operation = [=]() mutable
                    {
                        attempts++;
                        if (attempts < 3) {
                            throw any(std::make_shared<Error>(std::string("Failed")));
                        }
                        return std::string("success");
                    };
                    auto result = std::async([=]() { retry(operation, object{
                        object::pair{std::string("retries"), 3}, 
                        object::pair{std::string("delay"), 10}
                    }); });
                    expect(result)->toBe(std::string("success"));
                    expect(attempts)->toBe(3);
                }
                );
                it(std::string("should fail after max retries"), [=]() mutable
                {
                    auto operation = [=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("Always fails")));
                    };
                    std::async([=]() { expect(retry(operation, object{
                        object::pair{std::string("retries"), 2}, 
                        object::pair{std::string("delay"), 10}
                    }))->rejects->toThrow(std::string("Always fails")); });
                }
                );
                it(std::string("should not retry on success"), [=]() mutable
                {
                    shared attempts = 0;
                    auto operation = [=]() mutable
                    {
                        attempts++;
                        return std::string("immediate success");
                    };
                    auto result = std::async([=]() { retry(operation); });
                    expect(result)->toBe(std::string("immediate success"));
                    expect(attempts)->toBe(1);
                }
                );
            }
            );
            describe(std::string("debounce"), [=]() mutable
            {
                it(std::string("should debounce function calls"), [=]() mutable
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
            describe(std::string("throttle"), [=]() mutable
            {
                it(std::string("should throttle function calls"), [=]() mutable
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
        describe(std::string("Environment utilities"), [=]() mutable
        {
            it(std::string("should parse environment variables"), [=]() mutable
            {
                process->env->TEST_VAR = std::string("test_value");
                process->env->TEST_NUMBER = std::string("42");
                process->env->TEST_BOOL = std::string("true");
                expect(process->env->TEST_VAR)->toBe(std::string("test_value"));
                expect(parseInt(process->env->TEST_NUMBER))->toBe(42);
                expect(process->env->TEST_BOOL == std::string("true"))->toBe(true);
                process->env.Delete("TEST_VAR");
                process->env.Delete("TEST_NUMBER");
                process->env.Delete("TEST_BOOL");
            }
            );
            it(std::string("should handle missing environment variables"), [=]() mutable
            {
                expect(process->env->NONEXISTENT_VAR)->toBeUndefined();
            }
            );
        }
        );
        describe(std::string("Array utilities"), [=]() mutable
        {
            it(std::string("should chunk arrays"), [=]() mutable
            {
                auto array = array<double>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
                auto chunks = chunkArray(array, 3);
                expect(chunks)->toEqual(array<array<double>>{ array<double>{ 1, 2, 3 }, array<double>{ 4, 5, 6 }, array<double>{ 7, 8, 9 }, array<double>{ 10 } });
            }
            );
            it(std::string("should flatten nested arrays"), [=]() mutable
            {
                auto nested = array<array<double>>{ array<double>{ 1, 2 }, array<double>{ 3, array<double>{ 4, 5 } }, 6 };
                auto flattened = flattenArray(nested);
                expect(flattened)->toEqual(array<double>{ 1, 2, 3, 4, 5, 6 });
            }
            );
            it(std::string("should remove duplicates"), [=]() mutable
            {
                auto array = array<double>{ 1, 2, 2, 3, 3, 3, 4 };
                auto unique = removeDuplicates(array);
                expect(unique)->toEqual(array<double>{ 1, 2, 3, 4 });
            }
            );
        }
        );
        describe(std::string("Object utilities"), [=]() mutable
        {
            it(std::string("should deep clone objects"), [=]() mutable
            {
                auto original = object{
                    object::pair{std::string("a"), 1}, 
                    object::pair{std::string("b"), object{
                        object::pair{std::string("c"), 2}, 
                        object::pair{std::string("d"), array<double>{ 3, 4 }}
                    }}, 
                    object::pair{std::string("e"), std::make_shared<Date>()}
                };
                auto cloned = deepClone(original);
                expect(cloned)->toEqual(original);
                expect(cloned)->not->toBe(original);
                expect(cloned["b"])->not->toBe(original["b"]);
                expect(cloned["b"]["d"])->not->toBe(original["b"]["d"]);
            }
            );
            it(std::string("should check object equality"), [=]() mutable
            {
                auto obj1 = object{
                    object::pair{std::string("a"), 1}, 
                    object::pair{std::string("b"), object{
                        object::pair{std::string("c"), 2}
                    }}
                };
                auto obj2 = object{
                    object::pair{std::string("a"), 1}, 
                    object::pair{std::string("b"), object{
                        object::pair{std::string("c"), 2}
                    }}
                };
                auto obj3 = object{
                    object::pair{std::string("a"), 1}, 
                    object::pair{std::string("b"), object{
                        object::pair{std::string("c"), 3}
                    }}
                };
                expect(isEqual(obj1, obj2))->toBe(true);
                expect(isEqual(obj1, obj3))->toBe(false);
            }
            );
            it(std::string("should pick properties from object"), [=]() mutable
            {
                auto obj = object{
                    object::pair{std::string("a"), 1}, 
                    object::pair{std::string("b"), 2}, 
                    object::pair{std::string("c"), 3}, 
                    object::pair{std::string("d"), 4}
                };
                auto picked = pick(obj, array<string>{ std::string("a"), std::string("c") });
                expect(picked)->toEqual(object{
                    object::pair{std::string("a"), 1}, 
                    object::pair{std::string("c"), 3}
                });
            }
            );
            it(std::string("should omit properties from object"), [=]() mutable
            {
                auto obj = object{
                    object::pair{std::string("a"), 1}, 
                    object::pair{std::string("b"), 2}, 
                    object::pair{std::string("c"), 3}, 
                    object::pair{std::string("d"), 4}
                };
                auto omitted = omit(obj, array<string>{ std::string("b"), std::string("d") });
                expect(omitted)->toEqual(object{
                    object::pair{std::string("a"), 1}, 
                    object::pair{std::string("c"), 3}
                });
            }
            );
        }
        );
    }
    );
}

MAIN
