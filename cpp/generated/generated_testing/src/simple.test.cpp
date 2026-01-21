#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/src/__tests__/simple.test.h"

void Main(void)
{
    describe(std::string("Stagehand Plugin Simple Tests"), [=]() mutable
    {
        describe(std::string("Plugin Structure"), [=]() mutable
        {
            it(std::string("should have correct plugin metadata"), [=]() mutable
            {
                expect(stagehandPlugin->name)->toBe(std::string("plugin-stagehand"));
                expect(stagehandPlugin->description)->toBeDefined();
                expect(stagehandPlugin->actions)->toBeDefined();
                expect(stagehandPlugin->providers)->toBeDefined();
                expect(stagehandPlugin->services)->toBeDefined();
            }
            );
            it(std::string("should have BROWSER_NAVIGATE action"), [=]() mutable
            {
                auto action = stagehandPlugin->actions->find([=](auto a) mutable
                {
                    return a["name"] == std::string("BROWSER_NAVIGATE");
                }
                );
                expect(action)->toBeDefined();
                expect(action->similes)->toContain(std::string("GO_TO_URL"));
                expect(action->similes)->toContain(std::string("OPEN_WEBSITE"));
                expect(action->validate)->toBeDefined();
                expect(action->handler)->toBeDefined();
            }
            );
            it(std::string("should have BROWSER_STATE provider"), [=]() mutable
            {
                auto provider = stagehandPlugin->providers->find([=](auto p) mutable
                {
                    return p["name"] == std::string("BROWSER_STATE");
                }
                );
                expect(provider)->toBeDefined();
                expect(provider->get)->toBeDefined();
                expect(provider->description)->toContain(std::string("browser state"));
            }
            );
            it(std::string("should have examples for BROWSER_NAVIGATE"), [=]() mutable
            {
                auto action = stagehandPlugin->actions->find([=](auto a) mutable
                {
                    return a["name"] == std::string("BROWSER_NAVIGATE");
                }
                );
                expect(action->examples)->toBeDefined();
                expect(Array->isArray(action->examples))->toBe(true);
                expect(action->examples->length)->toBeGreaterThan(0);
                auto example = const_(action->examples)[0];
                expect(Array->isArray(example))->toBe(true);
                expect(const_(example)[0]->name)->toBe(std::string("{{user}}"));
                expect(const_(example)[1]->name)->toBe(std::string("{{agent}}"));
                expect(const_(example)[1]->content->actions)->toContain(std::string("BROWSER_NAVIGATE"));
            }
            );
        }
        );
        describe(std::string("BrowserSession"), [=]() mutable
        {
            it(std::string("should create a session with correct properties"), [=]() mutable
            {
                auto session = std::make_shared<BrowserSession>(std::string("test-123"));
                expect(session->id)->toBe(std::string("test-123"));
                expect(session->createdAt)->toBeInstanceOf(Date);
            }
            );
        }
        );
        describe(std::string("URL Validation"), [=]() mutable
        {
            it(std::string("should validate URLs in action"), [=]() mutable
            {
                auto action = stagehandPlugin->actions->find([=](auto a) mutable
                {
                    return a["name"] == std::string("BROWSER_NAVIGATE");
                }
                );
                auto mockRuntime = object{
                    object::pair{std::string("getService"), [=]() mutable
                    {
                        return (object{
                            object::pair{std::string("getCurrentSession"), [=]() mutable
                            {
                                return (object{
                                    object::pair{std::string("id"), std::string("test")}
                                });
                            }
                            }
                        });
                    }
                    }, 
                    object::pair{std::string("getSetting"), [=]() mutable
                    {
                        return std::string("true");
                    }
                    }
                };
                auto validMessage = object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Navigate to https://google.com")}, 
                        object::pair{std::string("source"), std::string("test")}
                    }}
                };
                auto invalidMessage = object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Just some text without URL")}, 
                        object::pair{std::string("source"), std::string("test")}
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
