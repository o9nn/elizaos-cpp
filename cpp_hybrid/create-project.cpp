#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/actions/create-project.h"

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
    auto patterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("create\s+(?:a\s+)?(?:new\s+)?(\S+)"))), (new RegExp(std::string("(?:build|make)\s+(?:a\s+)?(\S+)"))) };
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
        { any(std::string("plugin")), 1 },
        { any(std::string("agent")), 2 },
        { any(std::string("workflow")), 3 }
    };
    switch (__switch1650_6261[projectType])
    {
    case 1:
        return object{
            object::pair{std::string("title"), std::string("ElizaOS Plugin Project")}, 
            object::pair{std::string("description"), std::string("Let's gather information to create your plugin")}, 
            object::pair{std::string("fields"), array<object>{ object{
                object::pair{std::string("id"), std::string("project_name")}, 
                object::pair{std::string("label"), std::string("Project Name")}, 
                object::pair{std::string("type"), std::string("text")}, 
                object::pair{std::string("placeholder"), std::string("my-awesome-plugin")}, 
                object::pair{std::string("required"), true}, 
                object::pair{std::string("helpText"), std::string("What would you like to name your plugin?")}
            }, object{
                object::pair{std::string("id"), std::string("description")}, 
                object::pair{std::string("label"), std::string("Description")}, 
                object::pair{std::string("type"), std::string("textarea")}, 
                object::pair{std::string("placeholder"), std::string("A plugin that does amazing things...")}, 
                object::pair{std::string("required"), true}, 
                object::pair{std::string("helpText"), std::string("Describe what your plugin will do")}
            }, object{
                object::pair{std::string("id"), std::string("requirements")}, 
                object::pair{std::string("label"), std::string("Requirements")}, 
                object::pair{std::string("type"), std::string("textarea")}, 
                object::pair{std::string("placeholder"), std::string("List the key features and functionality...")}, 
                object::pair{std::string("required"), true}, 
                object::pair{std::string("helpText"), std::string("What specific features or capabilities should this plugin have?")}
            }, object{
                object::pair{std::string("id"), std::string("apis")}, 
                object::pair{std::string("label"), std::string("External APIs (optional)")}, 
                object::pair{std::string("type"), std::string("textarea")}, 
                object::pair{std::string("placeholder"), std::string("OpenAI API, Discord API, etc.")}, 
                object::pair{std::string("required"), false}, 
                object::pair{std::string("helpText"), std::string("List any external APIs or services this plugin will use")}
            }, object{
                object::pair{std::string("id"), std::string("test_scenarios")}, 
                object::pair{std::string("label"), std::string("Test Scenarios (optional)")}, 
                object::pair{std::string("type"), std::string("textarea")}, 
                object::pair{std::string("placeholder"), std::string("User asks for weather, plugin responds with...")}, 
                object::pair{std::string("required"), false}, 
                object::pair{std::string("helpText"), std::string("Describe some test scenarios to validate the plugin works correctly")}
            } }}
        };
    case 2:
        return object{
            object::pair{std::string("title"), std::string("ElizaOS Agent Project")}, 
            object::pair{std::string("description"), std::string("Let's design your agent's personality and capabilities")}, 
            object::pair{std::string("fields"), array<object>{ object{
                object::pair{std::string("id"), std::string("agent_name")}, 
                object::pair{std::string("label"), std::string("Agent Name")}, 
                object::pair{std::string("type"), std::string("text")}, 
                object::pair{std::string("placeholder"), std::string("My Assistant")}, 
                object::pair{std::string("required"), true}, 
                object::pair{std::string("helpText"), std::string("What's your agent's name?")}
            }, object{
                object::pair{std::string("id"), std::string("personality")}, 
                object::pair{std::string("label"), std::string("Personality & Bio")}, 
                object::pair{std::string("type"), std::string("textarea")}, 
                object::pair{std::string("placeholder"), std::string("A helpful, friendly assistant who...")}, 
                object::pair{std::string("required"), true}, 
                object::pair{std::string("helpText"), std::string("Describe your agent's personality and background")}
            }, object{
                object::pair{std::string("id"), std::string("capabilities")}, 
                object::pair{std::string("label"), std::string("Capabilities")}, 
                object::pair{std::string("type"), std::string("textarea")}, 
                object::pair{std::string("placeholder"), std::string("Can help with coding, answer questions...")}, 
                object::pair{std::string("required"), true}, 
                object::pair{std::string("helpText"), std::string("What should your agent be able to do?")}
            }, object{
                object::pair{std::string("id"), std::string("plugins")}, 
                object::pair{std::string("label"), std::string("Plugins to Include")}, 
                object::pair{std::string("type"), std::string("textarea")}, 
                object::pair{std::string("placeholder"), std::string("twitter, discord, knowledge...")}, 
                object::pair{std::string("required"), false}, 
                object::pair{std::string("helpText"), std::string("Which ElizaOS plugins should this agent use?")}
            } }}
        };
    case 3:
        return object{
            object::pair{std::string("title"), std::string("Workflow Project")}, 
            object::pair{std::string("description"), std::string("Define your workflow automation")}, 
            object::pair{std::string("fields"), array<object>{ object{
                object::pair{std::string("id"), std::string("workflow_name")}, 
                object::pair{std::string("label"), std::string("Workflow Name")}, 
                object::pair{std::string("type"), std::string("text")}, 
                object::pair{std::string("placeholder"), std::string("data-processing-workflow")}, 
                object::pair{std::string("required"), true}
            }, object{
                object::pair{std::string("id"), std::string("description")}, 
                object::pair{std::string("label"), std::string("Description")}, 
                object::pair{std::string("type"), std::string("textarea")}, 
                object::pair{std::string("placeholder"), std::string("This workflow processes data and...")}, 
                object::pair{std::string("required"), true}
            }, object{
                object::pair{std::string("id"), std::string("steps")}, 
                object::pair{std::string("label"), std::string("Workflow Steps")}, 
                object::pair{std::string("type"), std::string("textarea")}, 
                object::pair{std::string("placeholder"), std::string("1. Fetch data\
2. Process\
3. Store results")}, 
                object::pair{std::string("required"), true}, 
                object::pair{std::string("helpText"), std::string("List the steps in your workflow")}
            }, object{
                object::pair{std::string("id"), std::string("triggers")}, 
                object::pair{std::string("label"), std::string("Triggers")}, 
                object::pair{std::string("type"), std::string("textarea")}, 
                object::pair{std::string("placeholder"), std::string("On schedule, webhook, manual...")}, 
                object::pair{std::string("required"), false}
            } }}
        };
    default:
        return object{
            object::pair{std::string("title"), std::string("New Project")}, 
            object::pair{std::string("description"), std::string("Tell me about your project")}, 
            object::pair{std::string("fields"), array<object>{ object{
                object::pair{std::string("id"), std::string("project_name")}, 
                object::pair{std::string("label"), std::string("Project Name")}, 
                object::pair{std::string("type"), std::string("text")}, 
                object::pair{std::string("required"), true}
            }, object{
                object::pair{std::string("id"), std::string("description")}, 
                object::pair{std::string("label"), std::string("Description")}, 
                object::pair{std::string("type"), std::string("textarea")}, 
                object::pair{std::string("required"), true}
            }, object{
                object::pair{std::string("id"), std::string("requirements")}, 
                object::pair{std::string("label"), std::string("Requirements")}, 
                object::pair{std::string("type"), std::string("textarea")}, 
                object::pair{std::string("required"), true}
            } }}
        };
    }
};


std::shared_ptr<Action> createProjectAction = object{
    object::pair{std::string("name"), std::string("CREATE_PROJECT")}, 
    object::pair{std::string("similes"), array<string>{ std::string("GENERATE_CODE"), std::string("BUILD_PROJECT"), std::string("CREATE_PLUGIN"), std::string("CREATE_AGENT"), std::string("BUILD_PLUGIN"), std::string("BUILD_AGENT"), std::string("MAKE_PROJECT"), std::string("NEW_PROJECT"), std::string("DEVELOP_PLUGIN"), std::string("CODE_PROJECT") }}, 
    object::pair{std::string("description"), std::string("Create a new ElizaOS project (plugin, agent, workflow, etc.) with AI-generated code")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        shared text = OR((message->content->text->toLowerCase()), (string_empty));
        auto createKeywords = array<string>{ std::string("create"), std::string("build"), std::string("make"), std::string("generate"), std::string("develop"), std::string("code") };
        auto projectKeywords = array<string>{ std::string("project"), std::string("plugin"), std::string("agent"), std::string("bot"), std::string("workflow"), std::string("app"), std::string("application") };
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
        auto wantPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("i want to (?:create|build|make)"))), (new RegExp(std::string("i need (?:a|an) (?:new )?(?:plugin|agent|bot)"))), (new RegExp(std::string("can you (?:create|build|make|help me)"))), (new RegExp(std::string("help me (?:create|build|make)"))) };
        auto hasWantPattern = wantPatterns->some([=](auto pattern) mutable
        {
            return pattern->test(text);
        }
        );
        return OR(((AND((hasCreateKeyword), (hasProjectKeyword)))), (hasWantPattern));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto _options = undefined, auto callback = undefined, auto _responses = undefined) mutable
    {
        elizaLogger->info(std::string("CREATE_PROJECT action triggered"));
        auto formsService = as<any>(runtime->getService(std::string("forms")));
        if (!formsService) {
            elizaLogger->error(std::string("Forms service not available"));
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I can't create projects right now - the forms service isn't available. Please try again later.")}, 
                    object::pair{std::string("error"), true}
                }); });
            }
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), std::string("Forms service not available")}
            };
        }
        auto projectType = extractProjectType(OR((message->content->text), (string_empty)));
        auto suggestedName = extractProjectName(OR((message->content->text), (string_empty)));
        elizaLogger->info(std::string("Detected project type: ") + projectType + std::string(", suggested name: ") + suggestedName + string_empty);
        auto formTemplate = getFormTemplate(projectType);
        auto projectId = uuidv4();
        auto formId = as<std::shared_ptr<UUID>>(uuidv4());
        auto formData = utils::assign(object{
            object::pair{std::string("id"), formId}, 
            object::pair{std::string("onCancel"), object{
                object::pair{std::string("action"), std::string("CANCEL_FORM")}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("formId"), std::string("formId")}
                }}
            }}, 
            object::pair{std::string("onSubmit"), object{
                object::pair{std::string("action"), std::string("GENERATE_CODE")}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("projectId"), std::string("projectId")}, 
                    object::pair{std::string("projectType"), std::string("projectType")}
                }}
            }}
        }, formTemplate);
        std::async([=]() { formsService["createForm"](formData); });
        auto projectMetadata = object{
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(projectId)}, 
            object::pair{std::string("name"), OR((suggestedName), (std::string("Unnamed Project")))}, 
            object::pair{std::string("type"), projectType}, 
            object::pair{std::string("description"), OR((message->content->text), (string_empty))}, 
            object::pair{std::string("createdAt"), Date->now()}, 
            object::pair{std::string("updatedAt"), Date->now()}, 
            object::pair{std::string("status"), std::string("planning")}
        };
        std::async([=]() { runtime->createMemory(object{
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
            object::pair{std::string("entityId"), runtime->agentId}, 
            object::pair{std::string("roomId"), message->roomId}, 
            object::pair{std::string("agentId"), runtime->agentId}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("Project metadata: ") + JSON->stringify(projectMetadata) + string_empty}, 
                object::pair{std::string("type"), std::string("project_metadata")}, 
                object::pair{std::string("metadata"), projectMetadata}
            }}, 
            object::pair{std::string("createdAt"), Date->now()}
        }, std::string("project_metadata")); });
        auto responseText = (projectType == std::string("agent")) ? any(std::string("Great! Let's create your ElizaOS agent. I'll help you design their personality and capabilities.")) : any((projectType == std::string("plugin")) ? std::string("Excellent! Let's build your ElizaOS plugin. I'll help you define its features and functionality.") : std::string("Perfect! Let's create your project. Please fill out the details below."));
        if (callback) {
            std::async([=]() { callback(object{
                object::pair{std::string("text"), responseText}, 
                object::pair{std::string("form"), formData}
            }); });
        }
        return object{
            object::pair{std::string("success"), true}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("projectId"), std::string("projectId")}, 
                object::pair{std::string("projectType"), std::string("projectType")}, 
                object::pair{std::string("formId"), formData->id}
            }}
        };
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I want to create a new ElizaOS plugin called 'weather-bot' that fetches weather data")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Excellent! Let's build your ElizaOS plugin. I'll help you define its features and functionality.")}, 
            object::pair{std::string("action"), std::string("CREATE_PROJECT")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Can you help me build an agent for customer support?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Great! Let's create your ElizaOS agent. I'll help you design their personality and capabilities.")}, 
            object::pair{std::string("action"), std::string("CREATE_PROJECT")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
