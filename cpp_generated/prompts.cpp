#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/core/src/__tests__/prompts.test.h"

void Main(void)
{
    describe(std::string("Prompts"), [=]() mutable
    {
        describe(std::string("Template Structure"), [=]() mutable
        {
            it(std::string("shouldRespondTemplate should contain required placeholders and XML structure"), [=]() mutable
            {
                expect(shouldRespondTemplate)->toContain(std::string("{{agentName}}"));
                expect(shouldRespondTemplate)->toContain(std::string("{{providers}}"));
                expect(shouldRespondTemplate)->toContain(std::string("<response>"));
                expect(shouldRespondTemplate)->toContain(std::string("</response>"));
                expect(shouldRespondTemplate)->toContain(std::string("<name>"));
                expect(shouldRespondTemplate)->toContain(std::string("<reasoning>"));
                expect(shouldRespondTemplate)->toContain(std::string("<action>"));
                expect(shouldRespondTemplate)->toMatch((new RegExp(std::string("RESPOND \| IGNORE \| STO"))));
            }
            );
            it(std::string("messageHandlerTemplate should contain required placeholders and structure"), [=]() mutable
            {
                expect(messageHandlerTemplate)->toContain(std::string("{{agentName}}"));
                expect(messageHandlerTemplate)->toContain(std::string("{{providers}}"));
                expect(messageHandlerTemplate)->toContain(std::string("{{actionNames}}"));
                expect(messageHandlerTemplate)->toContain(std::string("<response>"));
                expect(messageHandlerTemplate)->toContain(std::string("</response>"));
                expect(messageHandlerTemplate)->toContain(std::string("<thought>"));
                expect(messageHandlerTemplate)->toContain(std::string("<actions>"));
                expect(messageHandlerTemplate)->toContain(std::string("<providers>"));
                expect(messageHandlerTemplate)->toContain(std::string("<text>"));
                expect(messageHandlerTemplate)->toContain(std::string("IMPORTANT ACTION ORDERING RULES"));
                expect(messageHandlerTemplate)->toContain(std::string("Actions are executed in the ORDER you list them"));
                expect(messageHandlerTemplate)->toContain(std::string("IMPORTANT PROVIDER SELECTION RULES"));
                expect(messageHandlerTemplate)->toContain(std::string("ATTACHMENTS"));
                expect(messageHandlerTemplate)->toContain(std::string("ENTITIES"));
                expect(messageHandlerTemplate)->toContain(std::string("KNOWLEDGE"));
            }
            );
            it(std::string("postCreationTemplate should contain required placeholders and examples"), [=]() mutable
            {
                expect(postCreationTemplate)->toContain(std::string("{{agentName}}"));
                expect(postCreationTemplate)->toContain(std::string("{{twitterUserName}}"));
                expect(postCreationTemplate)->toContain(std::string("{{providers}}"));
                expect(postCreationTemplate)->toContain(std::string("{{adjective}}"));
                expect(postCreationTemplate)->toContain(std::string("{{topic}}"));
                expect(postCreationTemplate)->toContain(std::string("<response>"));
                expect(postCreationTemplate)->toContain(std::string("</response>"));
                expect(postCreationTemplate)->toContain(std::string("<thought>"));
                expect(postCreationTemplate)->toContain(std::string("<post>"));
                expect(postCreationTemplate)->toContain(std::string("<imagePrompt>"));
                expect(postCreationTemplate)->toMatch((new RegExp(std::string("Example task outputs"))));
                expect(postCreationTemplate)->toContain(std::string("A post about"));
            }
            );
            it(std::string("booleanFooter should be a simple instruction"), [=]() mutable
            {
                expect(booleanFooter)->toBe(std::string("Respond with only a YES or a NO."));
                expect(booleanFooter)->toMatch((new RegExp(std::string("^Respond with only a YES or a NO\."))));
            }
            );
            it(std::string("imageDescriptionTemplate should contain proper XML structure"), [=]() mutable
            {
                expect(imageDescriptionTemplate)->toContain(std::string("<task>"));
                expect(imageDescriptionTemplate)->toContain(std::string("<instructions>"));
                expect(imageDescriptionTemplate)->toContain(std::string("<output>"));
                expect(imageDescriptionTemplate)->toContain(std::string("<response>"));
                expect(imageDescriptionTemplate)->toContain(std::string("</response>"));
                expect(imageDescriptionTemplate)->toContain(std::string("<title>"));
                expect(imageDescriptionTemplate)->toContain(std::string("<description>"));
                expect(imageDescriptionTemplate)->toContain(std::string("<text>"));
                expect(imageDescriptionTemplate)->toContain(std::string("Analyze the provided image"));
                expect(imageDescriptionTemplate)->toContain(std::string("Be objective and descriptive"));
            }
            );
        }
        );
        describe(std::string("Template Consistency"), [=]() mutable
        {
            shared templates = array<string>{ shouldRespondTemplate, messageHandlerTemplate, postCreationTemplate, imageDescriptionTemplate };
            it(std::string("all templates should have consistent XML output format instructions"), [=]() mutable
            {
                templates->forEach([=](auto template) mutable
                {
                    expect(template)->toContain(std::string("Do NOT include any thinking, reasoning, or <think> sections"));
                    expect(template)->toContain(std::string("IMPORTANT: Your response must ONLY contain the <response></response> XML block"));
                }
                );
            }
            );
            it(std::string("all templates should use proper XML closing tags"), [=]() mutable
            {
                templates->forEach([=](auto template) mutable
                {
                    auto responseBlocks = OR((template->match((new RegExp(std::string("<response>[\s\S]*?<\/response>"))))), (array<any>()));
                    responseBlocks->forEach([=](auto block) mutable
                    {
                        auto openTags = (OR((block->match((new RegExp(std::string("<[^/][^>]+>"))))), (array<any>())))->filter([=](auto tag) mutable
                        {
                            return !tag->includes(std::string("/>"));
                        }
                        )->filter([=](auto tag) mutable
                        {
                            return !tag->includes(std::string("think"));
                        }
                        );
                        shared closeTags = OR((block->match((new RegExp(std::string("<\/[^>]+>"))))), (array<any>()));
                        openTags->forEach([=](auto openTag) mutable
                        {
                            shared tagName = const_(openTag->match((new RegExp(std::string("<([^\s>]+")))))[1];
                            if (AND((tagName), (!(array<string>{ std::string("br"), std::string("hr"), std::string("img"), std::string("input"), std::string("meta"), std::string("link") })->includes(tagName)))) {
                                expect(closeTags->some([=](auto closeTag) mutable
                                {
                                    return closeTag->includes(tagName);
                                }
                                ))->toBe(true);
                            }
                        }
                        );
                    }
                    );
                    auto mainTags = array<string>{ std::string("task"), std::string("providers"), std::string("instructions"), std::string("output"), std::string("keys"), std::string("actionNames") };
                    mainTags->forEach([=](auto tag) mutable
                    {
                        if (template->includes(std::string("<") + tag + std::string(">"))) {
                            expect(template)->toContain(std::string("</") + tag + std::string(">"));
                        }
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std::string("Template Placeholders"), [=]() mutable
        {
            it(std::string("should use consistent placeholder format"), [=]() mutable
            {
                auto placeholderPattern = (new RegExp(std::string("\{\{[^}]+\}\}")));
                auto shouldRespondPlaceholders = OR((shouldRespondTemplate->match(placeholderPattern)), (array<any>()));
                auto messageHandlerPlaceholders = OR((messageHandlerTemplate->match(placeholderPattern)), (array<any>()));
                auto postCreationPlaceholders = OR((postCreationTemplate->match(placeholderPattern)), (array<any>()));
                (array<string>{ shouldRespondPlaceholders, messageHandlerPlaceholders, postCreationPlaceholders })->forEach([=](auto placeholder) mutable
                {
                    expect(placeholder)->toMatch((new RegExp(std::string("^\{\{[^}]+\}\}"))));
                }
                );
                expect(shouldRespondPlaceholders)->toContain(std::string("{{agentName}}"));
                expect(messageHandlerPlaceholders)->toContain(std::string("{{agentName}}"));
                expect(postCreationPlaceholders)->toContain(std::string("{{agentName}}"));
            }
            );
        }
        );
    }
    );
}

MAIN