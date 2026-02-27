#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/actions/create-project.h"

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
    auto patterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("create\s+(?:a\s+)?(?:new\s+)?(\S+)"))), (new RegExp(std:("(?:build|make)\s+(?:a\s+)?(\S+)"))) };
    for (auto& pattern : patterns)
    {
        auto match = text->match(pattern);
        if (match) {
            return (*const_(match))[1];
        }
    }
    return nullptr;
};


any getFormTemplate(string projectType)
{
    static switch_type __switch1650_6261 = {
        { any(std:("plugin")), 1 },
        { any(std:("agent")), 2 },
        { any(std:("workflow")), 3 }
    };
    switch (__switch1650_6261[projectType])
    {
    case 1:
        return object{
            object::pair{std:("title"), std:("ElizaOS Plugin Project")}, 
            object::pair{std:("description"), std:("Let's gather information to create your plugin")}, 
            object::pair{std:("fields"), array<object>{ object{
                object::pair{std:("id"), std:("project_name")}, 
                object::pair{std:("label"), std:("Project Name")}, 
                object::pair{std:("type"), std:("text")}, 
                object::pair{std:("placeholder"), std:("my-awesome-plugin")}, 
                object::pair{std:("required"), true}, 
                object::pair{std:("helpText"), std:("What would you like to name your plugin?")}
            }, object{
                object::pair{std:("id"), std:("description")}, 
                object::pair{std:("label"), std:("Description")}, 
                object::pair{std:("type"), std:("textarea")}, 
                object::pair{std:("placeholder"), std:("A plugin that does amazing things...")}, 
                object::pair{std:("required"), true}, 
                object::pair{std:("helpText"), std:("Describe what your plugin will do")}
            }, object{
                object::pair{std:("id"), std:("requirements")}, 
                object::pair{std:("label"), std:("Requirements")}, 
                object::pair{std:("type"), std:("textarea")}, 
                object::pair{std:("placeholder"), std:("List the key features and functionality...")}, 
                object::pair{std:("required"), true}, 
                object::pair{std:("helpText"), std:("What specific features or capabilities should this plugin have?")}
            }, object{
                object::pair{std:("id"), std:("apis")}, 
                object::pair{std:("label"), std:("External APIs (optional)")}, 
                object::pair{std:("type"), std:("textarea")}, 
                object::pair{std:("placeholder"), std:("OpenAI API, Discord API, etc.")}, 
                object::pair{std:("required"), false}, 
                object::pair{std:("helpText"), std:("List any external APIs or services this plugin will use")}
            }, object{
                object::pair{std:("id"), std:("test_scenarios")}, 
                object::pair{std:("label"), std:("Test Scenarios (optional)")}, 
                object::pair{std:("type"), std:("textarea")}, 
                object::pair{std:("placeholder"), std:("User asks for weather, plugin responds with...")}, 
                object::pair{std:("required"), false}, 
                object::pair{std:("helpText"), std:("Describe some test scenarios to validate the plugin works correctly")}
            } }}
        };
    case 2:
        return object{
            object::pair{std:("title"), std:("ElizaOS Agent Project")}, 
            object::pair{std:("description"), std:("Let's design your agent's personality and capabilities")}, 
            object::pair{std:("fields"), array<object>{ object{
                object::pair{std:("id"), std:("agent_name")}, 
                object::pair{std:("label"), std:("Agent Name")}, 
                object::pair{std:("type"), std:("text")}, 
                object::pair{std:("placeholder"), std:("My Assistant")}, 
                object::pair{std:("required"), true}, 
                object::pair{std:("helpText"), std:("What's your agent's name?")}
            }, object{
                object::pair{std:("id"), std:("personality")}, 
                object::pair{std:("label"), std:("Personality & Bio")}, 
                object::pair{std:("type"), std:("textarea")}, 
                object::pair{std:("placeholder"), std:("A helpful, friendly assistant who...")}, 
                object::pair{std:("required"), true}, 
                object::pair{std:("helpText"), std:("Describe your agent's personality and background")}
            }, object{
                object::pair{std:("id"), std:("capabilities")}, 
                object::pair{std:("label"), std:("Capabilities")}, 
                object::pair{std:("type"), std:("textarea")}, 
                object::pair{std:("placeholder"), std:("Can help with coding, answer questions...")}, 
                object::pair{std:("required"), true}, 
                object::pair{std:("helpText"), std:("What should your agent be able to do?")}
            }, object{
                object::pair{std:("id"), std:("plugins")}, 
                object::pair{std:("label"), std:("Plugins to Include")}, 
                object::pair{std:("type"), std:("textarea")}, 
                object::pair{std:("placeholder"), std:("twitter, discord, knowledge...")}, 
                object::pair{std:("required"), false}, 
                object::pair{std:("helpText"), std:("Which ElizaOS plugins should this agent use?")}
            } }}
        };
    case 3:
        return object{
            object::pair{std:("title"), std:("Workflow Project")}, 
            object::pair{std:("description"), std:("Define your workflow automation")}, 
            object::pair{std:("fields"), array<object>{ object{
                object::pair{std:("id"), std:("workflow_name")}, 
                object::pair{std:("label"), std:("Workflow Name")}, 
                object::pair{std:("type"), std:("text")}, 
                object::pair{std:("placeholder"), std:("data-processing-workflow")}, 
                object::pair{std:("required"), true}
            }, object{
                object::pair{std:("id"), std:("description")}, 
                object::pair{std:("label"), std:("Description")}, 
                object::pair{std:("type"), std:("textarea")}, 
                object::pair{std:("placeholder"), std:("This workflow processes data and...")}, 
                object::pair{std:("required"), true}
            }, object{
                object::pair{std:("id"), std:("steps")}, 
                object::pair{std:("label"), std:("Workflow Steps")}, 
                object::pair{std:("type"), std:("textarea")}, 
                object::pair{std:("placeholder"), std:("1. Fetch data\
2. Process\
3. Store results")}, 
                object::pair{std:("required"), true}, 
                object::pair{std:("helpText"), std:("List the steps in your workflow")}
            }, object{
                object::pair{std:("id"), std:("triggers")}, 
                object::pair{std:("label"), std:("Triggers")}, 
                object::pair{std:("type"), std:("textarea")}, 
                object::pair{std:("placeholder"), std:("On schedule, webhook, manual...")}, 
                object::pair{std:("required"), false}
            } }}
        };
    default:
        return object{
            object::pair{std:("title"), std:("New Project")}, 
            object::pair{std:("description"), std:("Tell me about your project")}, 
            object::pair{std:("fields"), array<object>{ object{
                object::pair{std:("id"), std:("project_name")}, 
                object::pair{std:("label"), std:("Project Name")}, 
                object::pair{std:("type"), std:("text")}, 
                object::pair{std:("required"), true}
            }, object{
                object::pair{std:("id"), std:("description")}, 
                object::pair{std:("label"), std:("Description")}, 
                object::pair{std:("type"), std:("textarea")}, 
                object::pair{std:("required"), true}
            }, object{
                object::pair{std:("id"), std:("requirements")}, 
                object::pair{std:("label"), std:("Requirements")}, 
                object::pair{std:("type"), std:("textarea")}, 
                object::pair{std:("required"), true}
            } }}
        };
    }
};


std::shared_ptr<Action> createProjectAction = object{
    object::pair{std:("name"), std:("CREATE_PROJECT")}, 
    object::pair{std:("similes"), array<string>{ std:("GENERATE_CODE"), std:("BUILD_PROJECT"), std:("CREATE_PLUGIN"), std:("CREATE_AGENT"), std:("BUILD_PLUGIN"), std:("BUILD_AGENT"), std:("MAKE_PROJECT"), std:("NEW_PROJECT"), std:("DEVELOP_PLUGIN"), std:("CODE_PROJECT") }}, 
    object::pair{std:("description"), std:("Create a new ElizaOS project (plugin, agent, workflow, etc.) with AI-generated code")}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        shared text = OR((message->content->text->toLowerCase()), (string_empty));
        auto createKeywords = array<string>{ std:("create"), std:("build"), std:("make"), std:("generate"), std:("develop"), std:("code") };
        auto projectKeywords = array<string>{ std:("project"), std:("plugin"), std:("agent"), std:("bot"), std:("workflow"), std:("app"), std:("application") };
        auto hasCreateKeyword = createKeywords->some([=](auto keyword) mutable
        {
            return text["includes"](keyword);
        }
        );
        auto hasProjectKeyword = projectKeywords->some([=](auto keyword) mutable
        {
            return text["includes"](keyword);
        }
        );
        auto wantPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("i want to (?:create|build|make)"))), (new RegExp(std:("i need (?:a|an) (?:new )?(?:plugin|agent|bot)"))), (new RegExp(std:("can you (?:create|build|make|help me)"))), (new RegExp(std:("help me (?:create|build|make)"))) };
        auto hasWantPattern = wantPatterns->some([=](auto pattern) mutable
        {
            return pattern->test(text);
        }
        );
        return OR(((AND((hasCreateKeyword), (hasProjectKeyword)))), (hasWantPattern));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto _options = undefined, auto callback = undefined, auto _responses = undefined) mutable
    {
        elizaLogger->info(std:("CREATE_PROJECT action triggered"));
        auto formsService = as<any>(runtime->getService(std:("forms")));
        if (!formsService) {
            elizaLogger->error(std:("Forms service not available"));
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I can't create projects right now - the forms service isn't available. Please try again later.")}, 
                    object::pair{std:("error"), true}
                }); });
            }
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std:("Forms service not available")}
            };
        }
        auto projectType = extractProjectType(OR((message->content->text), (string_empty)));
        auto suggestedName = extractProjectName(OR((message->content->text), (string_empty)));
        elizaLogger->info(std:("Detected project type: ") + projectType + std:(", suggested name: ") + suggestedName + string_empty);
        auto formTemplate = getFormTemplate(projectType);
        auto projectId = uuidv4();
        auto formId = as<std::shared_ptr<UUID>>(uuidv4());
        auto formData = utils::assign(object{
            object::pair{std:("id"), formId}, 
            object::pair{std:("onCancel"), object{
                object::pair{std:("action"), std:("CANCEL_FORM")}, 
                object::pair{std:("data"), object{
                    object::pair{std:("formId"), std:("formId")}
                }}
            }}, 
            object::pair{std:("onSubmit"), object{
                object::pair{std:("action"), std:("GENERATE_CODE")}, 
                object::pair{std:("data"), object{
                    object::pair{std:("projectId"), std:("projectId")}, 
                    object::pair{std:("projectType"), std:("projectType")}
                }}
            }}
        }, formTemplate);
        std::async([=]() { formsService["createForm"](formData); });
        auto projectMetadata = object{
            object::pair{std:("id"), as<std::shared_ptr<UUID>>(projectId)}, 
            object::pair{std:("name"), OR((suggestedName), (std:("Unnamed Project")))}, 
            object::pair{std:("type"), projectType}, 
            object::pair{std:("description"), OR((message->content->text), (string_empty))}, 
            object::pair{std:("createdAt"), Date->now()}, 
            object::pair{std:("updatedAt"), Date->now()}, 
            object::pair{std:("status"), std:("planning")}
        };
        std::async([=]() { runtime->createMemory(object{
            object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
            object::pair{std:("entityId"), runtime->agentId}, 
            object::pair{std:("roomId"), message->roomId}, 
            object::pair{std:("agentId"), runtime->agentId}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("Project metadata: ") + JSON->stringify(projectMetadata) + string_empty}, 
                object::pair{std:("type"), std:("project_metadata")}, 
                object::pair{std:("metadata"), projectMetadata}
            }}, 
            object::pair{std:("createdAt"), Date->now()}
        }, std:("project_metadata")); });
        auto responseText = (projectType == std:("agent")) ? any(std:("Great! Let's create your ElizaOS agent. I'll help you design their personality and capabilities.")) ((projectType == std:("plugin")) ? std:("Excellent! Let's build your ElizaOS plugin. I'll help you define its features and functionality.") : std:("Perfect! Let's create your project. Please fill out the details below."));
        if (callback) {
            std::async([=]() { callback(object{
                object::pair{std:("text"), responseText}, 
                object::pair{std:("form"), formData}
            }); });
        }
        return object{
            object::pair{std:("success"), true}, 
            object::pair{std:("data"), object{
                object::pair{std:("projectId"), std:("projectId")}, 
                object::pair{std:("projectType"), std:("projectType")}, 
                object::pair{std:("formId"), formData->id}
            }}
        };
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I want to create a new ElizaOS plugin called 'weather-bot' that fetches weather data")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Excellent! Let's build your ElizaOS plugin. I'll help you define its features and functionality.")}, 
            object::pair{std:("action"), std:("CREATE_PROJECT")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Can you help me build an agent for customer support?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Great! Let's create your ElizaOS agent. I'll help you design their personality and capabilities.")}, 
            object::pair{std:("action"), std:("CREATE_PROJECT")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
