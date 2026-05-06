#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/__tests__/actionExample.test.h"

void Main(void)
{
    describe(std::string("ActionExample Module"), [=]() mutable
    {
        shared v1Example = object{
            object::pair{std::string("user"), std::string("TestUser")}, 
            object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
                object::pair{std::string("text"), std::string("Hello world")}, 
                object::pair{std::string("action"), std::string("ACTION1")}
            })}
        };
        shared v2Example = object{
            object::pair{std::string("name"), std::string("TestUser")}, 
            object::pair{std::string("content"), as<std::shared_ptr<ContentV2>>(object{
                object::pair{std::string("text"), std::string("Hello world")}, 
                object::pair{std::string("actions"), array<string>{ std::string("ACTION1") }}
            })}
        };
        describe(std::string("convertContentToV1"), [=]() mutable
        {
            it(std::string("should convert V2 content to V1 content"), [=]() mutable
            {
                auto v2Content = as<std::shared_ptr<ContentV2>>(object{
                    object::pair{std::string("text"), std::string("Test content")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("ACTION1") }}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("timestamp"), 12345}
                    }}
                });
                auto result = convertContentToV1(v2Content);
                expect(result->text)->toBe(std::string("Test content"));
                expect(result->action)->toBe(std::string("ACTION1"));
                expect((as<any>(result))["metadata"]["timestamp"])->toBe(12345);
            }
            );
            it(std::string("should handle empty actions array"), [=]() mutable
            {
                auto v2Content = as<std::shared_ptr<ContentV2>>(object{
                    object::pair{std::string("text"), std::string("Test content")}, 
                    object::pair{std::string("actions"), array<any>()}
                });
                auto result = convertContentToV1(v2Content);
                expect(result->text)->toBe(std::string("Test content"));
                expect(result->action)->toBeUndefined();
            }
            );
            it(std::string("should handle null or undefined content"), [=]() mutable
            {
                auto result1 = convertContentToV1(nullptr);
                auto result2 = convertContentToV1(undefined);
                expect(result1->text)->toBe(string_empty);
                expect(result2->text)->toBe(string_empty);
            }
            );
        }
        );
        describe(std::string("convertContentToV2"), [=]() mutable
        {
            it(std::string("should convert V1 content to V2 content"), [=]() mutable
            {
                auto v1Content = as<std::shared_ptr<Content>>(object{
                    object::pair{std::string("text"), std::string("Test content")}, 
                    object::pair{std::string("action"), std::string("ACTION1")}, 
                    object::pair{std::string("thought"), std::string("Private thought")}
                });
                auto result = convertContentToV2(v1Content);
                expect(result->text)->toBe(std::string("Test content"));
                expect(result->actions)->toEqual(array<string>{ std::string("ACTION1") });
                expect((as<any>(result))["thought"])->toBe(std::string("Private thought"));
            }
            );
            it(std::string("should handle undefined action field"), [=]() mutable
            {
                auto v1Content = as<std::shared_ptr<Content>>(object{
                    object::pair{std::string("text"), std::string("Test content")}, 
                    object::pair{std::string("action"), undefined}
                });
                auto result = convertContentToV2(v1Content);
                expect(result->text)->toBe(std::string("Test content"));
                expect(result->actions)->toEqual(array<any>());
            }
            );
            it(std::string("should handle null or undefined content"), [=]() mutable
            {
                auto result1 = convertContentToV2(nullptr);
                auto result2 = convertContentToV2(undefined);
                expect(result1->text)->toBe(string_empty);
                expect(result2->text)->toBe(string_empty);
            }
            );
        }
        );
        describe(std::string("fromV2ActionExample"), [=]() mutable
        {
            it(std::string("should convert v2 ActionExample to v1 ActionExample"), [=]() mutable
            {
                auto result = fromV2ActionExample(v2Example);
                expect(result->user)->toBe(v2Example->name);
                expect(result->content->text)->toBe(v2Example->content->text);
                expect(result->content->action)->toBe(const_(v2Example->content->actions)[0]);
            }
            );
            it(std::string("should handle v2 example with minimal content"), [=]() mutable
            {
                auto minimalV2Example = object{
                    object::pair{std::string("name"), std::string("TestUser")}, 
                    object::pair{std::string("content"), as<std::shared_ptr<ContentV2>>(object{
                        object::pair{std::string("text"), std::string("Minimal example")}
                    })}
                };
                auto result = fromV2ActionExample(minimalV2Example);
                expect(result->user)->toBe(std::string("TestUser"));
                expect(result->content->text)->toBe(std::string("Minimal example"));
                expect(result->content->action)->toBeUndefined();
            }
            );
            it(std::string("should handle empty strings and arrays"), [=]() mutable
            {
                auto emptyV2Example = object{
                    object::pair{std::string("name"), string_empty}, 
                    object::pair{std::string("content"), as<std::shared_ptr<ContentV2>>(object{
                        object::pair{std::string("text"), string_empty}, 
                        object::pair{std::string("actions"), array<any>()}
                    })}
                };
                auto result = fromV2ActionExample(emptyV2Example);
                expect(result->user)->toBe(string_empty);
                expect(result->content->text)->toBe(string_empty);
                expect(result->content->action)->toBeUndefined();
            }
            );
            it(std::string("should handle null or undefined example"), [=]() mutable
            {
                auto result1 = fromV2ActionExample(nullptr);
                auto result2 = fromV2ActionExample(undefined);
                expect(result1->user)->toBe(string_empty);
                expect(result1->content->text)->toBe(string_empty);
                expect(result2->user)->toBe(string_empty);
                expect(result2->content->text)->toBe(string_empty);
            }
            );
        }
        );
        describe(std::string("toV2ActionExample"), [=]() mutable
        {
            it(std::string("should convert v1 ActionExample to v2 ActionExample"), [=]() mutable
            {
                auto result = toV2ActionExample(v1Example);
                expect(result->name)->toBe(v1Example->user);
                expect(result->content->text)->toBe(v1Example->content->text);
                expect(result->content->actions)->toEqual(array<string>{ v1Example->content->action });
            }
            );
            it(std::string("should handle v1 example with minimal content"), [=]() mutable
            {
                auto minimalV1Example = object{
                    object::pair{std::string("user"), std::string("TestUser")}, 
                    object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
                        object::pair{std::string("text"), std::string("Minimal example")}
                    })}
                };
                auto result = toV2ActionExample(minimalV1Example);
                expect(result->name)->toBe(std::string("TestUser"));
                expect(result->content->text)->toBe(std::string("Minimal example"));
                expect(result->content->actions)->toEqual(array<any>());
            }
            );
            it(std::string("should handle complex content structures"), [=]() mutable
            {
                auto complexV1Example = object{
                    object::pair{std::string("user"), std::string("ComplexUser")}, 
                    object::pair{std::string("content"), as<std::shared_ptr<Content>>(as<any>(object{
                        object::pair{std::string("text"), std::string("Complex example")}, 
                        object::pair{std::string("action"), std::string("ACTION1")}, 
                        object::pair{std::string("metadata"), object{
                            object::pair{std::string("timestamp"), 123456789}, 
                            object::pair{std::string("source"), std::string("test")}, 
                            object::pair{std::string("nested"), object{
                                object::pair{std::string("field1"), std::string("value1")}, 
                                object::pair{std::string("field2"), std::string("value2")}
                            }}
                        }}
                    }))}
                };
                auto result = toV2ActionExample(complexV1Example);
                expect(result->name)->toBe(std::string("ComplexUser"));
                expect(result->content->text)->toBe(std::string("Complex example"));
                expect(result->content->actions)->toEqual(array<string>{ std::string("ACTION1") });
                auto metadata = (as<any>(result->content))["metadata"];
                expect(metadata["timestamp"])->toBe(123456789);
                expect(metadata["nested"]["field1"])->toBe(std::string("value1"));
            }
            );
            it(std::string("should handle null or undefined example"), [=]() mutable
            {
                auto result1 = toV2ActionExample(nullptr);
                auto result2 = toV2ActionExample(undefined);
                expect(result1->name)->toBe(string_empty);
                expect(result1->content->text)->toBe(string_empty);
                expect(result2->name)->toBe(string_empty);
                expect(result2->content->text)->toBe(string_empty);
            }
            );
        }
        );
        describe(std::string("ActionExample type"), [=]() mutable
        {
            it(std::string("should match the expected structure"), [=]() mutable
            {
                auto example = object{
                    object::pair{std::string("user"), std::string("User1")}, 
                    object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
                        object::pair{std::string("text"), std::string("Sample text")}
                    })}
                };
                expect(example)->toHaveProperty(std::string("user"));
                expect(example)->toHaveProperty(std::string("content"));
                expect(type_of(example->user))->toBe(std::string("string"));
            }
            );
        }
        );
    }
    );
}

MAIN
