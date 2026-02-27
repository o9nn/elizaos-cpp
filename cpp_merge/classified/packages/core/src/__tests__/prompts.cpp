#include "prompts.test.h"

void Main(void)
{
    describe(std:("Prompts"), [=]() mutable
    {
        describe(std:("Template Structure"), [=]() mutable
        {
            it(std:("shouldRespondTemplate should contain required placeholders and XML structure"), [=]() mutable
            {
                expect(shouldRespondTemplate)->toContain(std:("{{agentName}}"));
                expect(shouldRespondTemplate)->toContain(std:("{{providers}}"));
                expect(shouldRespondTemplate)->toContain(std:("<response>"));
                expect(shouldRespondTemplate)->toContain(std:("</response>"));
                expect(shouldRespondTemplate)->toContain(std:("<name>"));
                expect(shouldRespondTemplate)->toContain(std:("<reasoning>"));
                expect(shouldRespondTemplate)->toContain(std:("<action>"));
                expect(shouldRespondTemplate)->toMatch((new RegExp(std:("RESPOND \| IGNORE \| STO"))));
            }
            );
            it(std:("messageHandlerTemplate should contain required placeholders and structure"), [=]() mutable
            {
                expect(messageHandlerTemplate)->toContain(std:("{{agentName}}"));
                expect(messageHandlerTemplate)->toContain(std:("{{providers}}"));
                expect(messageHandlerTemplate)->toContain(std:("{{actionNames}}"));
                expect(messageHandlerTemplate)->toContain(std:("<response>"));
                expect(messageHandlerTemplate)->toContain(std:("</response>"));
                expect(messageHandlerTemplate)->toContain(std:("<thought>"));
                expect(messageHandlerTemplate)->toContain(std:("<actions>"));
                expect(messageHandlerTemplate)->toContain(std:("<providers>"));
                expect(messageHandlerTemplate)->toContain(std:("<text>"));
                expect(messageHandlerTemplate)->toContain(std:("IMPORTANT ACTION ORDERING RULES"));
                expect(messageHandlerTemplate)->toContain(std:("Actions are executed in the ORDER you list them"));
                expect(messageHandlerTemplate)->toContain(std:("IMPORTANT CODE BLOCK FORMATTING RULES"));
                expect(messageHandlerTemplate)->toContain(std:("fenced code blocks"));
                expect(messageHandlerTemplate)->toContain(std:("single backticks"));
                expect(messageHandlerTemplate)->toContain(std:("IMPORTANT PROVIDER SELECTION RULES"));
                expect(messageHandlerTemplate)->toContain(std:("ATTACHMENTS"));
                expect(messageHandlerTemplate)->toContain(std:("ENTITIES"));
            }
            );
            it(std:("postCreationTemplate should contain required placeholders and examples"), [=]() mutable
            {
                expect(postCreationTemplate)->toContain(std:("{{agentName}}"));
                expect(postCreationTemplate)->toContain(std:("{{twitterUserName}}"));
                expect(postCreationTemplate)->toContain(std:("{{providers}}"));
                expect(postCreationTemplate)->toContain(std:("{{adjective}}"));
                expect(postCreationTemplate)->toContain(std:("{{topic}}"));
                expect(postCreationTemplate)->toContain(std:("<response>"));
                expect(postCreationTemplate)->toContain(std:("</response>"));
                expect(postCreationTemplate)->toContain(std:("<thought>"));
                expect(postCreationTemplate)->toContain(std:("<post>"));
                expect(postCreationTemplate)->toContain(std:("<imagePrompt>"));
                expect(postCreationTemplate)->toMatch((new RegExp(std:("Example task outputs"))));
                expect(postCreationTemplate)->toContain(std:("A post about"));
            }
            );
            it(std:("booleanFooter should be a simple instruction"), [=]() mutable
            {
                expect(booleanFooter)->toBe(std:("Respond with only a YES or a NO."));
                expect(booleanFooter)->toMatch((new RegExp(std:("^Respond with only a YES or a NO\."))));
            }
            );
            it(std:("imageDescriptionTemplate should contain proper XML structure"), [=]() mutable
            {
                expect(imageDescriptionTemplate)->toContain(std:("<task>"));
                expect(imageDescriptionTemplate)->toContain(std:("<instructions>"));
                expect(imageDescriptionTemplate)->toContain(std:("<output>"));
                expect(imageDescriptionTemplate)->toContain(std:("<response>"));
                expect(imageDescriptionTemplate)->toContain(std:("</response>"));
                expect(imageDescriptionTemplate)->toContain(std:("<title>"));
                expect(imageDescriptionTemplate)->toContain(std:("<description>"));
                expect(imageDescriptionTemplate)->toContain(std:("<text>"));
                expect(imageDescriptionTemplate)->toContain(std:("Analyze the provided image"));
                expect(imageDescriptionTemplate)->toContain(std:("Be objective and descriptive"));
            }
            );
        }
        );
        describe(std:("Template Consistency"), [=]() mutable
        {
            shared templates = array<string>{ shouldRespondTemplate, messageHandlerTemplate, postCreationTemplate, imageDescriptionTemplate };
            it(std:("all templates should have consistent XML output format instructions"), [=]() mutable
            {
                templates->forEach([=](auto template) mutable
                {
                    expect(template)->toContain(std:("Do NOT include any thinking, reasoning, or <think> sections"));
                    expect(template)->toContain(std:("IMPORTANT: Your response must ONLY contain the <response></response> XML block"));
                }
                );
            }
            );
            it(std:("all templates should use proper XML closing tags"), [=]() mutable
            {
                templates->forEach([=](auto template) mutable
                {
                    auto responseBlocks = OR((template->match((new RegExp(std:("<response>[\s\S]*?<\/response>"))))), (array<any>()));
                    responseBlocks->forEach([=](auto block) mutable
                    {
                        auto openTags = (OR((block->match((new RegExp(std:("<[^/][^>]+>"))))), (array<any>())))->filter([=](auto tag) mutable
                        {
                            return !tag->includes(std:("/>"));
                        }
                        )->filter([=](auto tag) mutable
                        {
                            return !tag->includes(std:("think"));
                        }
                        );
                        shared closeTags = OR((block->match((new RegExp(std:("<\/[^>]+>"))))), (array<any>()));
                        openTags->forEach([=](auto openTag) mutable
                        {
                            shared tagName = const_(openTag->match((new RegExp(std:("<([^\s>]+")))))[1];
                            if (AND((tagName), (!(array<string>{ std:("br"), std:("hr"), std:("img"), std:("input"), std:("meta"), std:("link") })->includes(tagName)))) {
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
                    auto mainTags = array<string>{ std:("task"), std:("providers"), std:("instructions"), std:("output"), std:("keys"), std:("actionNames") };
                    mainTags->forEach([=](auto tag) mutable
                    {
                        if (template->includes(std:("<") + tag + std:(">"))) {
                            expect(template)->toContain(std:("</") + tag + std:(">"));
                        }
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std:("Template Placeholders"), [=]() mutable
        {
            it(std:("should use consistent placeholder format"), [=]() mutable
            {
                auto placeholderPattern = (new RegExp(std:("\{\{[^}]+\}\}")));
                auto shouldRespondPlaceholders = OR((shouldRespondTemplate->match(placeholderPattern)), (array<any>()));
                auto messageHandlerPlaceholders = OR((messageHandlerTemplate->match(placeholderPattern)), (array<any>()));
                auto postCreationPlaceholders = OR((postCreationTemplate->match(placeholderPattern)), (array<any>()));
                (array<string>{ shouldRespondPlaceholders, messageHandlerPlaceholders, postCreationPlaceholders })->forEach([=](auto placeholder) mutable
                {
                    expect(placeholder)->toMatch((new RegExp(std:("^\{\{[^}]+\}\}"))));
                }
                );
                expect(shouldRespondPlaceholders)->toContain(std:("{{agentName}}"));
                expect(messageHandlerPlaceholders)->toContain(std:("{{agentName}}"));
                expect(postCreationPlaceholders)->toContain(std:("{{agentName}}"));
            }
            );
        }
        );
    }
    );
}

MAIN