#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/actions/generate-code.h"

std::shared_ptr<Promise<string>> extractProjectType(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<State> _state)
{
    auto text = OR((message->content->text->toLowerCase()), (string_empty));
    if (text["includes"](std::string("plugin"))) {
        return std::string("plugin");
    }
    if (OR((text["includes"](std::string("agent"))), (text["includes"](std::string("bot"))))) {
        return std::string("agent");
    }
    if (text["includes"](std::string("workflow"))) {
        return std::string("workflow");
    }
    if (text["includes"](std::string("mcp"))) {
        return std::string("mcp");
    }
    if (OR((text["includes"](std::string("app"))), (text["includes"](std::string("application"))))) {
        return std::string("full-stack");
    }
    auto data = as<object>(message->content->data);
    if (data["projectType"]) {
        return data["projectType"];
    }
    auto recentMemories = std::async([=]() { runtime->getMemories(object{
        object::pair{std::string("roomId"), message->roomId}, 
        object::pair{std::string("count"), 10}, 
        object::pair{std::string("unique"), false}, 
        object::pair{std::string("tableName"), std::string("messages")}
    }); });
    for (auto& memory : recentMemories)
    {
        auto memText = OR((memory->content->text->toLowerCase()), (string_empty));
        if (memText["includes"](std::string("plugin"))) {
            return std::string("plugin");
        }
        if (memText["includes"](std::string("agent"))) {
            return std::string("agent");
        }
        if (memText["includes"](std::string("workflow"))) {
            return std::string("workflow");
        }
    }
    return std::string("plugin");
};


array<string> extractRequirements(string text)
{
    shared requirements = array<string>();
    auto lines = text->split(std::string("\
"));
    lines->forEach([=](auto line) mutable
    {
        auto trimmed = line->trim();
        if (OR((trimmed->match((new RegExp(std::string("^[-*•"))))), (trimmed->match((new RegExp(std::string("^\d+[.)"))))))) {
            requirements->push(trimmed->replace((new RegExp(std::string("^[-*•]\s"))), string_empty)->replace((new RegExp(std::string("^\d+[.)]\s"))), string_empty));
        }
    }
    );
    if (requirements->get_length() == 0) {
        auto sentences = OR((text->match((new RegExp(std::string("[^.!?]+[.!?]+"))))), (array<any>()));
        sentences->forEach([=](auto sentence) mutable
        {
            if (OR((OR((sentence->includes(std::string("should"))), (sentence->includes(std::string("must"))))), (sentence->includes(std::string("need"))))) {
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
    auto apiPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("(\w+)\s+API/"))), (new RegExp(std::string("API[:\s]+(\w+)/"))), (new RegExp(std::string("uses?\s+(\w+)/"))), (new RegExp(std::string("integrates?\s+with\s+(\w+)/"))) };
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
    auto quotedMatch = text->match((new RegExp(std::string("["']([^"']+)["'"))));
    if (quotedMatch) {
        return (*const_(quotedMatch))[1];
    }
    auto namedMatch = text->match((new RegExp(std::string("(?:called|named)\s+(\S+)"))));
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
        return std::string("Great! Now let's ") + nextStep["title"] + std::string(". ") + (OR((nextStep["description"]), (string_empty))) + string_empty;
    }
    return std::string("Perfect! I have all the information I need. Let me start generating your code...");
};


any extractProjectData(any formData)
{
    auto data = object{
        object::pair{std::string("projectName"), OR((OR((formData["projectName"]), (formData["project_name"]))), (std::string("unnamed-project")))}, 
        object::pair{std::string("description"), OR((formData["description"]), (string_empty))}, 
        object::pair{std::string("requirements"), array<any>()}, 
        object::pair{std::string("apis"), array<any>()}, 
        object::pair{std::string("targetType"), OR((formData["projectType"]), (std::string("plugin")))}
    };
    if (formData["requirements"]) {
        data["requirements"] = formData["requirements"]["split"](std::string("\
"))["filter"]([=](auto r) mutable
        {
            return r->trim();
        }
        );
    }
    if (formData["apis"]) {
        data["apis"] = formData["apis"]["split"]((new RegExp(std::string("[,\n"))))["map"]([=](auto a) mutable
        {
            return a->trim();
        }
        )["filter"](Boolean);
    }
    if (formData["test_scenarios"]) {
        data["testScenarios"] = formData["test_scenarios"]["split"](std::string("\
"))["filter"]([=](auto s) mutable
        {
            return s->trim();
        }
        );
    }
    data["createGithubRepo"] = OR((formData["create_github"] == true), (formData["create_github"] == std::string("true")));
    data["privateRepo"] = OR((formData["private_repo"] == true), (formData["private_repo"] == std::string("true")));
    if (formData["agent_name"]) {
        data["projectName"] = formData["agent_name"];
        data["targetType"] = std::string("agent");
    }
    if (formData["personality"]) {
        data["requirements"]["push"](std::string("Agent personality: ") + formData["personality"] + string_empty);
    }
    if (formData["capabilities"]) {
        data["requirements"]["push"](std::string("Agent capabilities: ") + formData["capabilities"] + string_empty);
    }
    if (formData["plugins"]) {
        auto plugins = formData["plugins"]["split"]((new RegExp(std::string("[,\n"))))["map"]([=](auto p) mutable
        {
            return p->trim();
        }
        )["filter"](Boolean);
        data["requirements"]["push"](std::string("Include plugins: ") + plugins["join"](std::string(", ")) + string_empty);
    }
    return data;
};


std::shared_ptr<Action> generateCodeAction = object{
    object::pair{std::string("name"), std::string("GENERATE_CODE")}, 
    object::pair{std::string("similes"), array<string>{ std::string("BUILD_CODE"), std::string("CREATE_CODE"), std::string("WRITE_CODE"), std::string("DEVELOP_CODE"), std::string("IMPLEMENT_CODE"), std::string("CODE_GENERATION"), std::string("GENERATE_PROJECT"), std::string("BUILD_PROJECT") }}, 
    object::pair{std::string("description"), std::string("Generate code for ElizaOS projects using AI")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        shared text = OR((message->content->text->toLowerCase()), (string_empty));
        auto data = as<object>(message->content->data);
        if (data["action"] == std::string("GENERATE_CODE")) {
            return true;
        }
        auto genKeywords = array<string>{ std::string("generate"), std::string("create"), std::string("build"), std::string("write"), std::string("implement"), std::string("code") };
        auto contextKeywords = array<string>{ std::string("plugin"), std::string("agent"), std::string("action"), std::string("provider"), std::string("service") };
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
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto _options = undefined, auto callback = undefined, auto _responses = undefined) mutable
    {
        elizaLogger->info(std::string("GENERATE_CODE action triggered"));
        auto codeGenService = as<std::shared_ptr<CodeGenerationService>>(runtime->getService(std::string("code-generation")));
        if (!codeGenService) {
            elizaLogger->error(std::string("Code generation service not available"));
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I can't generate code right now - the code generation service isn't available. Please ensure all required API keys are configured.")}, 
                    object::pair{std::string("error"), true}
                }); });
            }
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), std::string("Code generation service not available")}
            };
        }
        auto formsService = as<any>(runtime->getService(std::string("forms")));
        auto projectType = std::async([=]() { extractProjectType(runtime, message, state); });
        elizaLogger->info(std::string("Project type: ") + projectType + string_empty);
        if (formsService) {
            auto activeForms = std::async([=]() { formsService["listForms"](std::string("active")); });
            auto projectForm = activeForms["find"]([=](auto f) mutable
            {
                return f->id->startsWith(std::string("project-"));
            }
            );
            if (AND((projectForm), (projectForm["status"] != std::string("completed")))) {
                auto nextStepPrompt = getNextStepPrompt(projectForm, OR((message->content->text), (string_empty)));
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), nextStepPrompt}, 
                        object::pair{std::string("form"), projectForm}
                    }); });
                }
                return object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("status"), std::string("collecting_requirements")}, 
                        object::pair{std::string("formId"), projectForm["id"]}
                    }}
                };
            }
            auto messageData = as<object>(message->content->data);
            if (AND((messageData["formId"]), (messageData["formData"]))) {
                auto formData = messageData["formData"];
                auto projectData = extractProjectData(formData);
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("🚀 Starting code generation for your ") + projectData["targetType"] + std::string("...\
\
This may take a few minutes. I'll:\
1. Research best practices\
2. Generate the code structure\
3. Implement all features\
4. Run quality checks")}
                    }); });
                }
                auto result = std::async([=]() { codeGenService->generateCode(object{
                    object::pair{std::string("projectName"), projectData["projectName"]}, 
                    object::pair{std::string("description"), projectData["description"]}, 
                    object::pair{std::string("requirements"), projectData["requirements"]}, 
                    object::pair{std::string("apis"), OR((projectData["apis"]), (array<any>()))}, 
                    object::pair{std::string("targetType"), projectData["targetType"]}, 
                    object::pair{std::string("testScenarios"), OR((projectData["testScenarios"]), (array<any>()))}
                }); });
                if (!result->success) {
                    if (callback) {
                        std::async([=]() { callback(object{
                            object::pair{std::string("text"), std::string("❌ Code generation failed: ") + (OR((result->errors->join(std::string("\
"))), (std::string("Unknown error")))) + string_empty}, 
                            object::pair{std::string("error"), true}
                        }); });
                    }
                    return object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), OR((result->errors->join(std::string("\
"))), (std::string("Code generation failed")))}
                    };
                }
                auto successMessage = std::string("✅ **Code generation complete!**\
\
");
                successMessage += std::string("📁 **Project:** ") + projectData["projectName"] + std::string("\
");
                successMessage += std::string("📋 **Type:** ") + projectData["targetType"] + std::string("\
");
                if (result->files) {
                    successMessage += std::string("📄 **Files generated:** ") + result->files->get_length() + std::string("\
");
                }
                if (result->projectPath) {
                    successMessage += std::string("\
📍 **Project location:** ") + result->projectPath + string_empty;
                }
                if (result->executionResults) {
                    successMessage += std::string("\
\
**Quality Checks:**\
");
                    successMessage += string_empty + (result->executionResults->lintPass) ? std::string("✅") : std::string("❌") + std::string(" Linting\
");
                    successMessage += string_empty + (result->executionResults->typesPass) ? std::string("✅") : std::string("❌") + std::string(" Type checking\
");
                    successMessage += string_empty + (result->executionResults->testsPass) ? std::string("✅") : std::string("❌") + std::string(" Tests\
");
                    successMessage += string_empty + (result->executionResults->buildPass) ? std::string("✅") : std::string("❌") + std::string(" Build\
");
                }
                successMessage += std::string("\
\
🎉 Your ") + projectData["targetType"] + std::string(" is ready! Let me know if you need any modifications.");
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), successMessage}, 
                        object::pair{std::string("data"), object{
                            object::pair{std::string("projectPath"), result->projectPath}, 
                            object::pair{std::string("files"), result->files}
                        }}
                    }); });
                }
                return object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("projectPath"), result->projectPath}, 
                        object::pair{std::string("filesGenerated"), OR((result->files->get_length()), (0))}, 
                        object::pair{std::string("executionResults"), result->executionResults}
                    }}
                };
            }
        }
        auto requirements = extractRequirements(OR((message->content->text), (string_empty)));
        if (OR((!requirements), (requirements->get_length() == 0))) {
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I need more details about what you want to build. Could you describe:\
- What type of project (plugin, agent, etc.)?\
- What features or functionality it should have?\
- Any specific APIs or services it should use?")}, 
                    object::pair{std::string("error"), false}
                }); });
            }
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), std::string("Insufficient requirements provided")}
            };
        }
        auto projectName = OR((extractProjectName(OR((message->content->text), (string_empty)))), (std::string("project-") + Date->now() + string_empty));
        if (callback) {
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("🚀 Starting code generation based on your requirements...\
\
Generating: ") + projectName + std::string("\
Type: ") + projectType + string_empty}
            }); });
        }
        auto result = std::async([=]() { codeGenService->generateCode(object{
            object::pair{std::string("projectName"), std::string("projectName")}, 
            object::pair{std::string("description"), OR((message->content->text), (string_empty))}, 
            object::pair{std::string("requirements"), std::string("requirements")}, 
            object::pair{std::string("apis"), extractAPIs(OR((message->content->text), (string_empty)))}, 
            object::pair{std::string("targetType"), as<ProjectType>(projectType)}
        }); });
        if (!result->success) {
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("❌ Code generation failed: ") + (OR((result->errors->join(std::string("\
"))), (std::string("Unknown error")))) + string_empty}, 
                    object::pair{std::string("error"), true}
                }); });
            }
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), OR((result->errors->join(std::string("\
"))), (std::string("Code generation failed")))}
            };
        }
        auto responseText = std::string("✅ Code generation complete!\
\
");
        responseText += std::string("📁 Project: ") + projectName + std::string("\
");
        if (result->files) {
            responseText += std::string("📄 Files generated: ") + result->files->get_length() + std::string("\
");
        }
        if (result->projectPath) {
            responseText += std::string("📍 Location: ") + result->projectPath + std::string("\
");
        }
        responseText += std::string("\
🎉 Your code is ready!");
        if (callback) {
            std::async([=]() { callback(object{
                object::pair{std::string("text"), responseText}, 
                object::pair{std::string("data"), result}
            }); });
        }
        return object{
            object::pair{std::string("success"), true}, 
            object::pair{std::string("data"), result}
        };
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Generate a plugin that monitors crypto prices and sends alerts")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll generate a crypto price monitoring plugin for you. Let me create the code structure with price tracking and alert functionality.")}, 
            object::pair{std::string("action"), std::string("GENERATE_CODE")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
