#include "simple.test.h"

void Main(void)
{
    describe(std:("Stagehand Plugin Simple Tests"), [=]() mutable
    {
        describe(std:("Plugin Structure"), [=]() mutable
        {
            it(std:("should have correct plugin metadata"), [=]() mutable
            {
                expect(stagehandPlugin->name)->toBe(std:("plugin-stagehand"));
                expect(stagehandPlugin->description)->toBeDefined();
                expect(stagehandPlugin->actions)->toBeDefined();
                expect(stagehandPlugin->providers)->toBeDefined();
                expect(stagehandPlugin->services)->toBeDefined();
            }
            );
            it(std:("should have BROWSER_NAVIGATE action"), [=]() mutable
            {
                auto action = stagehandPlugin->actions->find([=](auto a) mutable
                {
                    return a["name"] == std:("BROWSER_NAVIGATE");
                }
                );
                expect(action)->toBeDefined();
                expect(action->similes)->toContain(std:("GO_TO_URL"));
                expect(action->similes)->toContain(std:("OPEN_WEBSITE"));
                expect(action->validate)->toBeDefined();
                expect(action->handler)->toBeDefined();
            }
            );
            it(std:("should have BROWSER_STATE provider"), [=]() mutable
            {
                auto provider = stagehandPlugin->providers->find([=](auto p) mutable
                {
                    return p["name"] == std:("BROWSER_STATE");
                }
                );
                expect(provider)->toBeDefined();
                expect(provider->get)->toBeDefined();
                expect(provider->description)->toContain(std:("browser state"));
            }
            );
            it(std:("should have examples for BROWSER_NAVIGATE"), [=]() mutable
            {
                auto action = stagehandPlugin->actions->find([=](auto a) mutable
                {
                    return a["name"] == std:("BROWSER_NAVIGATE");
                }
                );
                expect(action->examples)->toBeDefined();
                expect(Array->isArray(action->examples))->toBe(true);
                expect(action->examples->length)->toBeGreaterThan(0);
                auto example = const_(action->examples)[0];
                expect(Array->isArray(example))->toBe(true);
                expect(const_(example)[0]->name)->toBe(std:("{{user}}"));
                expect(const_(example)[1]->name)->toBe(std:("{{agent}}"));
                expect(const_(example)[1]->content->actions)->toContain(std:("BROWSER_NAVIGATE"));
            }
            );
        }
        );
        describe(std:("BrowserSession"), [=]() mutable
        {
            it(std:("should create a session with correct properties"), [=]() mutable
            {
                auto session = std::make_shared<BrowserSession>(std:("test-123"));
                expect(session->id)->toBe(std:("test-123"));
                expect(session->createdAt)->toBeInstanceOf(Date);
            }
            );
        }
        );
        describe(std:("URL Validation"), [=]() mutable
        {
            it(std:("should validate URLs in action"), [=]() mutable
            {
                auto action = stagehandPlugin->actions->find([=](auto a) mutable
                {
                    return a["name"] == std:("BROWSER_NAVIGATE");
                }
                );
                auto mockRuntime = object{
                    object::pair{std:("getService"), [=]() mutable
                    {
                        return (object{
                            object::pair{std:("getCurrentSession"), [=]() mutable
                            {
                                return (object{
                                    object::pair{std:("id"), std:("test")}
                                });
                            }
                            }
                        });
                    }
                    }, 
                    object::pair{std:("getSetting"), [=]() mutable
                    {
                        return std:("true");
                    }
                    }
                };
                auto validMessage = object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Navigate to https://google.com")}, 
                        object::pair{std:("source"), std:("test")}
                    }}
                };
                auto invalidMessage = object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Just some text without URL")}, 
                        object::pair{std:("source"), std:("test")}
                    }}
                };
                expect(std::async([=]() { action->validate(as<any>(mockRuntime), as<any>(validMessage), as<any>(object{})); }))->toBe(true);
                expect(std::async([=]() { action->validate(as<any>(mockRuntime), as<any>(invalidMessage), as<any>(object{})); }))->toBe(false);
            }
            );
        }
        );
    }
    );
}

MAIN
