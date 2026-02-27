#include "generate-code.h"

std::shared_ptr<Promise<string>> extractProjectType(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<State> _state)
{
    auto text = OR((message->content->text->toLowerCase()), (string_empty));
    if (text["includes"](std:("plugin"))) {
        return std:("plugin");
    }
    if (OR((text["includes"](std:("agent"))), (text["includes"](std:("bot"))))) {
        return std:("agent");
    }
    if (text["includes"](std:("workflow"))) {
        return std:("workflow");
    }
    if (text["includes"](std:("mcp"))) {
        return std:("mcp");
    }
    if (OR((text["includes"](std:("app"))), (text["includes"](std:("application"))))) {
        return std:("full-stack");
    }
    auto data = as<object>(message->content->data);
    if (data["projectType"]) {
        return data["projectType"];
    }
    auto recentMemories = std::async([=]() { runtime->getMemories(object{
        object::pair{std:("roomId"), message->roomId}, 
        object::pair{std:("count"), 10}, 
        object::pair{std:("unique"), false}, 
        object::pair{std:("tableName"), std:("messages")}
    }); });
    for (auto& memory : recentMemories)
    {
        auto memText = OR((memory->content->text->toLowerCase()), (string_empty));
        if (memText["includes"](std:("plugin"))) {
            return std:("plugin");
        }
        if (memText["includes"](std:("agent"))) {
            return std:("agent");
        }
        if (memText["includes"](std:("workflow"))) {
            return std:("workflow");
        }
    }
    return std:("plugin");
};


array<string> extractRequirements(string text)
{
    shared requirements = array<string>();
    auto lines = text->split(std:("\
"));
    lines->forEach([=](auto line) mutable
    {
        auto trimmed = line->trim();
        if (OR((trimmed->match((new RegExp(std:("^[-*•"))))), (trimmed->match((new RegExp(std:("^\d+[.)"))))))) {
            requirements->push(trimmed->replace((new RegExp(std:("^[-*•]\s"))), string_empty)->replace((new RegExp(std:("^\d+[.)]\s"))), string_empty));
        }
    }
    );
    if (requirements->get_length() == 0) {
        auto sentences = OR((text->match((new RegExp(std:("[^.!?]+[.!?]+"))))), (array<any>()));
        sentences->forEach([=](auto sentence) mutable
        {
            if (OR((OR((sentence->includes(std:("should"))), (sentence->includes(std:("must"))))), (sentence->includes(std:("need"))))) {
                requirements->push(sentence->trim());
            }
        }
        );
    }
    if (AND((requirements->get_length() == 0), (text->get_length() > 20))) {
        requirements->push(text);
    }
    return requirements;
};


array<string> extractAPIs(string text)
{
    shared apis = array<string>();
    auto apiPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("(\w+)\s+API/"))), (new RegExp(std:("API[:\s]+(\w+)/"))), (new RegExp(std:("uses?\s+(\w+)/"))), (new RegExp(std:("integrates?\s+with\s+(\w+)/"))) };
    apiPatterns->forEach([=](auto pattern) mutable
    {
        auto matches = text->matchAll(pattern);
        for (auto& match : matches)
        {
            if (const_(match)[1]) {
                apis->push(const_(match)[1]);
            }
        }
    }
    );
    return array<any>{ std::make_shared<Set>(apis) };
};


any extractProjectName(string text)
{
    auto quotedMatch = text->match((new RegExp(std:("["']([^"']+)["'"))));
    if (quotedMatch) {
        return (*const_(quotedMatch))[1];
    }
    auto namedMatch = text->match((new RegExp(std:("(?:called|named)\s+(\S+)"))));
    if (namedMatch) {
        return (*const_(namedMatch))[1];
    }
    return nullptr;
};


string getNextStepPrompt(any form, string completedStepId)
{
    auto steps = OR((form["steps"]), (array<any>()));
    auto currentStepIndex = steps["findIndex"]([=](auto s) mutable
    {
        return s["id"] == completedStepId;
    }
    );
    if (AND((currentStepIndex >= 0), (currentStepIndex < steps["length"] - 1))) {
        auto nextStep = const_(steps)[currentStepIndex + 1];
        return std:("Great! Now let's ") + nextStep["title"] + std:(". ") + (OR((nextStep["description"]), (string_empty))) + string_empty;
    }
    return std:("Perfect! I have all the information I need. Let me start generating your code...");
};


any extractProjectData(any formData)
{
    auto data = object{
        object::pair{std:("projectName"), OR((OR((formData["projectName"]), (formData["project_name"]))), (std:("unnamed-project")))}, 
        object::pair{std:("description"), OR((formData["description"]), (string_empty))}, 
        object::pair{std:("requirements"), array<any>()}, 
        object::pair{std:("apis"), array<any>()}, 
        object::pair{std:("targetType"), OR((formData["projectType"]), (std:("plugin")))}
    };
    if (formData["requirements"]) {
        data["requirements"] = formData["requirements"]["split"](std:("\
"))["filter"]([=](auto r) mutable
        {
            return r->trim();
        }
        );
    }
    if (formData["apis"]) {
        data["apis"] = formData["apis"]["split"]((new RegExp(std:("[,\n"))))["map"]([=](auto a) mutable
        {
            return a->trim();
        }
        )["filter"](Boolean);
    }
    if (formData["test_scenarios"]) {
        data["testScenarios"] = formData["test_scenarios"]["split"](std:("\
"))["filter"]([=](auto s) mutable
        {
            return s->trim();
        }
        );
    }
    data["createGithubRepo"] = OR((formData["create_github"] == true), (formData["create_github"] == std:("true")));
    data["privateRepo"] = OR((formData["private_repo"] == true), (formData["private_repo"] == std:("true")));
    if (formData["agent_name"]) {
        data["projectName"] = formData["agent_name"];
        data["targetType"] = std:("agent");
    }
    if (formData["personality"]) {
        data["requirements"]["push"](std:("Agent personality: ") + formData["personality"] + string_empty);
    }
    if (formData["capabilities"]) {
        data["requirements"]["push"](std:("Agent capabilities: ") + formData["capabilities"] + string_empty);
    }
    if (formData["plugins"]) {
        auto plugins = formData["plugins"]["split"]((new RegExp(std:("[,\n"))))["map"]([=](auto p) mutable
        {
            return p->trim();
        }
        )["filter"](Boolean);
        data["requirements"]["push"](std:("Include plugins: ") + plugins["join"](std:(", ")) + string_empty);
    }
    return data;
};


std::shared_ptr<Action> generateCodeAction = object{
    object::pair{std:("name"), std:("GENERATE_CODE")}, 
    object::pair{std:("similes"), array<string>{ std:("BUILD_CODE"), std:("CREATE_CODE"), std:("WRITE_CODE"), std:("DEVELOP_CODE"), std:("IMPLEMENT_CODE"), std:("CODE_GENERATION"), std:("GENERATE_PROJECT"), std:("BUILD_PROJECT") }}, 
    object::pair{std:("description"), std:("Generate code for ElizaOS projects using AI")}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        shared text = OR((message->content->text->toLowerCase()), (string_empty));
        auto data = as<object>(message->content->data);
        if (data["action"] == std:("GENERATE_CODE")) {
            return true;
        }
        auto genKeywords = array<string>{ std:("generate"), std:("create"), std:("build"), std:("write"), std:("implement"), std:("code") };
        auto contextKeywords = array<string>{ std:("plugin"), std:("agent"), std:("action"), std:("provider"), std:("service") };
        auto hasGenKeyword = genKeywords->some([=](auto keyword) mutable
        {
            return text["includes"](keyword);
        }
        );
        auto hasContextKeyword = contextKeywords->some([=](auto keyword) mutable
        {
            return text["includes"](keyword);
        }
        );
        return AND((hasGenKeyword), (hasContextKeyword));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto _options = undefined, auto callback = undefined, auto _responses = undefined) mutable
    {
        elizaLogger->info(std:("GENERATE_CODE action triggered"));
        auto codeGenService = as<std::shared_ptr<CodeGenerationService>>(runtime->getService(std:("code-generation")));
        if (!codeGenService) {
            elizaLogger->error(std:("Code generation service not available"));
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I can't generate code right now - the code generation service isn't available. Please ensure all required API keys are configured.")}, 
                    object::pair{std:("error"), true}
                }); });
            }
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std:("Code generation service not available")}
            };
        }
        auto formsService = as<any>(runtime->getService(std:("forms")));
        auto projectType = std::async([=]() { extractProjectType(runtime, message, state); });
        elizaLogger->info(std:("Project type: ") + projectType + string_empty);
        if (formsService) {
            auto activeForms = std::async([=]() { formsService["listForms"](std:("active")); });
            auto projectForm = activeForms["find"]([=](auto f) mutable
            {
                return f->id->startsWith(std:("project-"));
            }
            );
            if (AND((projectForm), (projectForm["status"] != std:("completed")))) {
                auto nextStepPrompt = getNextStepPrompt(projectForm, OR((message->content->text), (string_empty)));
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), nextStepPrompt}, 
                        object::pair{std:("form"), projectForm}
                    }); });
                }
                return object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("status"), std:("collecting_requirements")}, 
                        object::pair{std:("formId"), projectForm["id"]}
                    }}
                };
            }
            auto messageData = as<object>(message->content->data);
            if (AND((messageData["formId"]), (messageData["formData"]))) {
                auto formData = messageData["formData"];
                auto projectData = extractProjectData(formData);
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("🚀 Starting code generation for your ") + projectData["targetType"] + std:("...\
\
This may take a few minutes. I'll:\
1. Research best practices\
2. Generate the code structure\
3. Implement all features\
4. Run quality checks")}
                    }); });
                }
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std:("projectName"), projectData["projectName"]}, 
                    object::pair{std:("description"), projectData["description"]}, 
                    object::pair{std:("requirements"), projectData["requirements"]}, 
                    object::pair{std:("apis"), OR((projectData["apis"]), (array<any>()))}, 
                    object::pair{std:("targetType"), projectData["targetType"]}, 
                    object::pair{std:("testScenarios"), OR((projectData["testScenarios"]), (array<any>()))}
                }); });
                if (!result->success) {
                    if (callback) {
                        std::async([=]() { callback(object{
                            object::pair{std:("text"), std:("❌ Code generation failed: ") + (OR((result->errors->join(std:("\
"))), (std:("Unknown error")))) + string_empty}, 
                            object::pair{std:("error"), true}
                        }); });
                    }
                    return object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), OR((result->errors->join(std:("\
"))), (std:("Code generation failed")))}
                    };
                }
                auto successMessage = std:("✅ **Code generation complete!**\
\
");
                successMessage += std:("📁 **Project:** ") + projectData["projectName"] + std:("\
");
                successMessage += std:("📋 **Type:** ") + projectData["targetType"] + std:("\
");
                if (result->files) {
                    successMessage += std:("📄 **Files generated:** ") + result->files->get_length() + std:("\
");
                }
                if (result->projectPath) {
                    successMessage += std:("\
📍 **Project location:** ") + result->projectPath + string_empty;
                }
                if (result->executionResults) {
                    successMessage += std:("\
\
**Quality Checks:**\
");
                    successMessage += string_empty + (result->executionResults->lintPass) ? std:("✅") : std:("❌") + std:(" Linting\
");
                    successMessage += string_empty + (result->executionResults->typesPass) ? std:("✅") : std:("❌") + std:(" Type checking\
");
                    successMessage += string_empty + (result->executionResults->testsPass) ? std:("✅") : std:("❌") + std:(" Tests\
");
                    successMessage += string_empty + (result->executionResults->buildPass) ? std:("✅") : std:("❌") + std:(" Build\
");
                }
                successMessage += std:("\
\
🎉 Your ") + projectData["targetType"] + std:(" is ready! Let me know if you need any modifications.");
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), successMessage}, 
                        object::pair{std:("data"), object{
                            object::pair{std:("projectPath"), result->projectPath}, 
                            object::pair{std:("files"), result->files}
                        }}
                    }); });
                }
                return object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("projectPath"), result->projectPath}, 
                        object::pair{std:("filesGenerated"), OR((result->files->get_length()), (0))}, 
                        object::pair{std:("executionResults"), result->executionResults}
                    }}
                };
            }
        }
        auto requirements = extractRequirements(OR((message->content->text), (string_empty)));
        if (OR((!requirements), (requirements->get_length() == 0))) {
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I need more details about what you want to build. Could you describe:\
- What type of project (plugin, agent, etc.)?\
- What features or functionality it should have?\
- Any specific APIs or services it should use?")}, 
                    object::pair{std:("error"), false}
                }); });
            }
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std:("Insufficient requirements provided")}
            };
        }
        auto projectName = OR((extractProjectName(OR((message->content->text), (string_empty)))), (std:("project-") + Date->now() + string_empty));
        if (callback) {
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("🚀 Starting code generation based on your requirements...\
\
Generating: ") + projectName + std:("\
Type: ") + projectType + string_empty}
            }); });
        }
        auto result = std::async([=]() { codeGenService->generateCode(object{
            object::pair{std:("projectName"), std:("projectName")}, 
            object::pair{std:("description"), OR((message->content->text), (string_empty))}, 
            object::pair{std:("requirements"), std:("requirements")}, 
            object::pair{std:("apis"), extractAPIs(OR((message->content->text), (string_empty)))}, 
            object::pair{std:("targetType"), as<ProjectType>(projectType)}
        }); });
        if (!result->success) {
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("❌ Code generation failed: ") + (OR((result->errors->join(std:("\
"))), (std:("Unknown error")))) + string_empty}, 
                    object::pair{std:("error"), true}
                }); });
            }
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), OR((result->errors->join(std:("\
"))), (std:("Code generation failed")))}
            };
        }
        auto responseText = std:("✅ Code generation complete!\
\
");
        responseText += std:("📁 Project: ") + projectName + std:("\
");
        if (result->files) {
            responseText += std:("📄 Files generated: ") + result->files->get_length() + std:("\
");
        }
        if (result->projectPath) {
            responseText += std:("📍 Location: ") + result->projectPath + std:("\
");
        }
        responseText += std:("\
🎉 Your code is ready!");
        if (callback) {
            std::async([=]() { callback(object{
                object::pair{std:("text"), responseText}, 
                object::pair{std:("data"), result}
            }); });
        }
        return object{
            object::pair{std:("success"), true}, 
            object::pair{std:("data"), result}
        };
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Generate a plugin that monitors crypto prices and sends alerts")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll generate a crypto price monitoring plugin for you. Let me create the code structure with price tracking and alert functionality.")}, 
            object::pair{std:("action"), std:("GENERATE_CODE")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
