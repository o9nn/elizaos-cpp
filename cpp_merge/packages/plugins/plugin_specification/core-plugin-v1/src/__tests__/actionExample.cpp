#include "actionExample.test.h"

void Main(void)
{
    describe(std:("ActionExample Module"), [=]() mutable
    {
        shared v1Example = object{
            object::pair{std:("user"), std:("TestUser")}, 
            object::pair{std:("content"), as<std::shared_ptr<Content>>(object{
                object::pair{std:("text"), std:("Hello world")}, 
                object::pair{std:("action"), std:("ACTION1")}
            })}
        };
        shared v2Example = object{
            object::pair{std:("name"), std:("TestUser")}, 
            object::pair{std:("content"), as<std::shared_ptr<ContentV2>>(object{
                object::pair{std:("text"), std:("Hello world")}, 
                object::pair{std:("actions"), array<string>{ std:("ACTION1") }}
            })}
        };
        describe(std:("convertContentToV1"), [=]() mutable
        {
            it(std:("should convert V2 content to V1 content"), [=]() mutable
            {
                auto v2Content = as<std::shared_ptr<ContentV2>>(object{
                    object::pair{std:("text"), std:("Test content")}, 
                    object::pair{std:("actions"), array<string>{ std:("ACTION1") }}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("timestamp"), 12345}
                    }}
                });
                auto result = convertContentToV1(v2Content);
                expect(result->text)->toBe(std:("Test content"));
                expect(result->action)->toBe(std:("ACTION1"));
                expect((as<any>(result))["metadata"]["timestamp"])->toBe(12345);
            }
            );
            it(std:("should handle empty actions array"), [=]() mutable
            {
                auto v2Content = as<std::shared_ptr<ContentV2>>(object{
                    object::pair{std:("text"), std:("Test content")}, 
                    object::pair{std:("actions"), array<any>()}
                });
                auto result = convertContentToV1(v2Content);
                expect(result->text)->toBe(std:("Test content"));
                expect(result->action)->toBeUndefined();
            }
            );
            it(std:("should handle null or undefined content"), [=]() mutable
            {
                auto result1 = convertContentToV1(nullptr);
                auto result2 = convertContentToV1(undefined);
                expect(result1->text)->toBe(string_empty);
                expect(result2->text)->toBe(string_empty);
            }
            );
        }
        );
        describe(std:("convertContentToV2"), [=]() mutable
        {
            it(std:("should convert V1 content to V2 content"), [=]() mutable
            {
                auto v1Content = as<std::shared_ptr<Content>>(object{
                    object::pair{std:("text"), std:("Test content")}, 
                    object::pair{std:("action"), std:("ACTION1")}, 
                    object::pair{std:("thought"), std:("Private thought")}
                });
                auto result = convertContentToV2(v1Content);
                expect(result->text)->toBe(std:("Test content"));
                expect(result->actions)->toEqual(array<string>{ std:("ACTION1") });
                expect((as<any>(result))["thought"])->toBe(std:("Private thought"));
            }
            );
            it(std:("should handle undefined action field"), [=]() mutable
            {
                auto v1Content = as<std::shared_ptr<Content>>(object{
                    object::pair{std:("text"), std:("Test content")}, 
                    object::pair{std:("action"), undefined}
                });
                auto result = convertContentToV2(v1Content);
                expect(result->text)->toBe(std:("Test content"));
                expect(result->actions)->toEqual(array<any>());
            }
            );
            it(std:("should handle null or undefined content"), [=]() mutable
            {
                auto result1 = convertContentToV2(nullptr);
                auto result2 = convertContentToV2(undefined);
                expect(result1->text)->toBe(string_empty);
                expect(result2->text)->toBe(string_empty);
            }
            );
        }
        );
        describe(std:("fromV2ActionExample"), [=]() mutable
        {
            it(std:("should convert v2 ActionExample to v1 ActionExample"), [=]() mutable
            {
                auto result = fromV2ActionExample(v2Example);
                expect(result->user)->toBe(v2Example->name);
                expect(result->content->text)->toBe(v2Example->content->text);
                expect(result->content->action)->toBe(const_(v2Example->content->actions)[0]);
            }
            );
            it(std:("should handle v2 example with minimal content"), [=]() mutable
            {
                auto minimalV2Example = object{
                    object::pair{std:("name"), std:("TestUser")}, 
                    object::pair{std:("content"), as<std::shared_ptr<ContentV2>>(object{
                        object::pair{std:("text"), std:("Minimal example")}
                    })}
                };
                auto result = fromV2ActionExample(minimalV2Example);
                expect(result->user)->toBe(std:("TestUser"));
                expect(result->content->text)->toBe(std:("Minimal example"));
                expect(result->content->action)->toBeUndefined();
            }
            );
            it(std:("should handle empty strings and arrays"), [=]() mutable
            {
                auto emptyV2Example = object{
                    object::pair{std:("name"), string_empty}, 
                    object::pair{std:("content"), as<std::shared_ptr<ContentV2>>(object{
                        object::pair{std:("text"), string_empty}, 
                        object::pair{std:("actions"), array<any>()}
                    })}
                };
                auto result = fromV2ActionExample(emptyV2Example);
                expect(result->user)->toBe(string_empty);
                expect(result->content->text)->toBe(string_empty);
                expect(result->content->action)->toBeUndefined();
            }
            );
            it(std:("should handle null or undefined example"), [=]() mutable
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
        describe(std:("toV2ActionExample"), [=]() mutable
        {
            it(std:("should convert v1 ActionExample to v2 ActionExample"), [=]() mutable
            {
                auto result = toV2ActionExample(v1Example);
                expect(result->name)->toBe(v1Example->user);
                expect(result->content->text)->toBe(v1Example->content->text);
                expect(result->content->actions)->toEqual(array<string>{ v1Example->content->action });
            }
            );
            it(std:("should handle v1 example with minimal content"), [=]() mutable
            {
                auto minimalV1Example = object{
                    object::pair{std:("user"), std:("TestUser")}, 
                    object::pair{std:("content"), as<std::shared_ptr<Content>>(object{
                        object::pair{std:("text"), std:("Minimal example")}
                    })}
                };
                auto result = toV2ActionExample(minimalV1Example);
                expect(result->name)->toBe(std:("TestUser"));
                expect(result->content->text)->toBe(std:("Minimal example"));
                expect(result->content->actions)->toEqual(array<any>());
            }
            );
            it(std:("should handle complex content structures"), [=]() mutable
            {
                auto complexV1Example = object{
                    object::pair{std:("user"), std:("ComplexUser")}, 
                    object::pair{std:("content"), as<std::shared_ptr<Content>>(as<any>(object{
                        object::pair{std:("text"), std:("Complex example")}, 
                        object::pair{std:("action"), std:("ACTION1")}, 
                        object::pair{std:("metadata"), object{
                            object::pair{std:("timestamp"), 123456789}, 
                            object::pair{std:("source"), std:("test")}, 
                            object::pair{std:("nested"), object{
                                object::pair{std:("field1"), std:("value1")}, 
                                object::pair{std:("field2"), std:("value2")}
                            }}
                        }}
                    }))}
                };
                auto result = toV2ActionExample(complexV1Example);
                expect(result->name)->toBe(std:("ComplexUser"));
                expect(result->content->text)->toBe(std:("Complex example"));
                expect(result->content->actions)->toEqual(array<string>{ std:("ACTION1") });
                auto metadata = (as<any>(result->content))["metadata"];
                expect(metadata["timestamp"])->toBe(123456789);
                expect(metadata["nested"]["field1"])->toBe(std:("value1"));
            }
            );
            it(std:("should handle null or undefined example"), [=]() mutable
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
        describe(std:("ActionExample type"), [=]() mutable
        {
            it(std:("should match the expected structure"), [=]() mutable
            {
                auto example = object{
                    object::pair{std:("user"), std:("User1")}, 
                    object::pair{std:("content"), as<std::shared_ptr<Content>>(object{
                        object::pair{std:("text"), std:("Sample text")}
                    })}
                };
                expect(example)->toHaveProperty(std:("user"));
                expect(example)->toHaveProperty(std:("content"));
                expect(type_of(example->user))->toBe(std:("string"));
            }
            );
        }
        );
    }
    );
}

MAIN
