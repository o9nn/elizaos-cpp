#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-dynamic/actions/plugin-creation-actions.h"

std::shared_ptr<Promise<std::shared_ptr<PluginSpecification>>> generatePluginSpecification(string description, std::shared_ptr<IAgentRuntime> runtime)
{
    shared lowerDesc = description->toLowerCase();
    auto name = std::string("@elizaos/plugin-");
    auto pluginType = std::string("custom");
    if (lowerDesc->includes(std::string("weather"))) {
        pluginType = std::string("weather");
        name += std::string("weather");
    } else if (OR((lowerDesc->includes(std::string("database"))), (lowerDesc->includes(std::string("sql"))))) {
        pluginType = std::string("database");
        name += std::string("database");
    } else if (OR((lowerDesc->includes(std::string("api"))), (lowerDesc->includes(std::string("rest"))))) {
        pluginType = std::string("api");
        name += std::string("api");
    } else if (OR((lowerDesc->includes(std::string("todo"))), (lowerDesc->includes(std::string("task"))))) {
        pluginType = std::string("todo");
        name += std::string("todo");
    } else if (OR((lowerDesc->includes(std::string("email"))), (lowerDesc->includes(std::string("mail"))))) {
        pluginType = std::string("email");
        name += std::string("email");
    } else if (OR((lowerDesc->includes(std::string("chat"))), (lowerDesc->includes(std::string("message"))))) {
        pluginType = std::string("chat");
        name += std::string("chat");
    } else {
        auto words = description->split((new RegExp(std::string("\s"))))->filter([=](auto w) mutable
        {
            return w->get_length() > 4;
        }
        );
        name += OR((const_(words)[0]->toLowerCase()), (std::string("custom")));
    }
    auto specification = object{
        object::pair{std::string("name"), std::string("name")}, 
        object::pair{std::string("description"), description->slice(0, 200)}, 
        object::pair{std::string("version"), std::string("1.0.0")}, 
        object::pair{std::string("actions"), array<any>()}, 
        object::pair{std::string("providers"), array<any>()}, 
        object::pair{std::string("services"), array<any>()}, 
        object::pair{std::string("evaluators"), array<any>()}
    };
    auto actionKeywords = object{
        object::pair{std::string("create"), array<string>{ std::string("create"), std::string("add"), std::string("new"), std::string("generate"), std::string("make") }}, 
        object::pair{std::string("read"), array<string>{ std::string("get"), std::string("fetch"), std::string("retrieve"), std::string("list"), std::string("show"), std::string("display") }}, 
        object::pair{std::string("update"), array<string>{ std::string("update"), std::string("modify"), std::string("change"), std::string("edit"), std::string("set") }}, 
        object::pair{std::string("delete"), array<string>{ std::string("delete"), std::string("remove"), std::string("clear"), std::string("destroy") }}, 
        object::pair{std::string("execute"), array<string>{ std::string("execute"), std::string("run"), std::string("perform"), std::string("do"), std::string("process") }}
    };
    for (auto& [actionType, keywords] : Object->entries(actionKeywords))
    {
        if (keywords->some([=](auto kw) mutable
        {
            return lowerDesc->includes(kw);
        }
        )) {
            specification->actions->push(object{
                object::pair{std::string("name"), string_empty + actionType + string_empty + (pluginType->charAt(0)->toUpperCase() + pluginType->slice(1)) + string_empty}, 
                object::pair{std::string("description"), string_empty + (actionType->charAt(0)->toUpperCase() + actionType->slice(1)) + std::string(" operation for ") + pluginType + string_empty}, 
                object::pair{std::string("parameters"), object{}}
            });
        }
    }
    if (OR((OR((OR((lowerDesc->includes(std::string("provide"))), (lowerDesc->includes(std::string("information"))))), (lowerDesc->includes(std::string("data"))))), (lowerDesc->includes(std::string("context"))))) {
        specification->providers->push(object{
            object::pair{std::string("name"), string_empty + pluginType + std::string("Provider")}, 
            object::pair{std::string("description"), std::string("Provides ") + pluginType + std::string(" data and context")}, 
            object::pair{std::string("dataStructure"), object{}}
        });
    }
    if (OR((OR((OR((lowerDesc->includes(std::string("service"))), (lowerDesc->includes(std::string("background"))))), (lowerDesc->includes(std::string("monitor"))))), (lowerDesc->includes(std::string("watch"))))) {
        specification->services->push(object{
            object::pair{std::string("name"), string_empty + pluginType + std::string("Service")}, 
            object::pair{std::string("description"), std::string("Background service for ") + pluginType + std::string(" operations")}, 
            object::pair{std::string("methods"), array<string>{ std::string("start"), std::string("stop"), std::string("status") }}
        });
    }
    if (OR((OR((OR((lowerDesc->includes(std::string("evaluate"))), (lowerDesc->includes(std::string("analyze"))))), (lowerDesc->includes(std::string("check"))))), (lowerDesc->includes(std::string("validate"))))) {
        specification->evaluators->push(object{
            object::pair{std::string("name"), string_empty + pluginType + std::string("Evaluator")}, 
            object::pair{std::string("description"), std::string("Evaluates and analyzes ") + pluginType + std::string(" data")}, 
            object::pair{std::string("triggers"), array<any>()}
        });
    }
    if (AND((AND((AND((!specification->actions->get_length()), (!specification->providers->get_length()))), (!specification->services->get_length()))), (!specification->evaluators->get_length()))) {
        specification->actions = array<object>{ object{
            object::pair{std::string("name"), std::string("handle") + (pluginType->charAt(0)->toUpperCase() + pluginType->slice(1)) + string_empty}, 
            object::pair{std::string("description"), std::string("Main handler for ") + pluginType + std::string(" operations")}
        } };
    }
    return specification;
};


any PluginSpecificationSchema = z->object(object{
    object::pair{std::string("name"), z->string()->regex((new RegExp(std::string("^@?[a-zA-Z0-9-_]+\/[a-zA-Z0-9-_]+"))), std::string("Invalid plugin name format"))}, 
    object::pair{std::string("description"), z->string()->min(10, std::string("Description must be at least 10 characters"))}, 
    object::pair{std::string("version"), z->string()->regex((new RegExp(std::string("^\d+\.\d+\.\d+"))), std::string("Version must be in semver format"))->optional()->default(std::string("1.0.0"))}, 
    object::pair{std::string("actions"), z->array(z->object(object{
        object::pair{std::string("name"), z->string()->regex((new RegExp(std::string("^[a-zA-Z][a-zA-Z0-9]*"))), std::string("Action name must be alphanumeric"))}, 
        object::pair{std::string("description"), z->string()}, 
        object::pair{std::string("parameters"), z->record(z->any())->optional()}
    }))->optional()}, 
    object::pair{std::string("providers"), z->array(z->object(object{
        object::pair{std::string("name"), z->string()->regex((new RegExp(std::string("^[a-zA-Z][a-zA-Z0-9]*"))), std::string("Provider name must be alphanumeric"))}, 
        object::pair{std::string("description"), z->string()}, 
        object::pair{std::string("dataStructure"), z->record(z->any())->optional()}
    }))->optional()}, 
    object::pair{std::string("services"), z->array(z->object(object{
        object::pair{std::string("name"), z->string()->regex((new RegExp(std::string("^[a-zA-Z][a-zA-Z0-9]*"))), std::string("Service name must be alphanumeric"))}, 
        object::pair{std::string("description"), z->string()}, 
        object::pair{std::string("methods"), z->array(z->string())->optional()}
    }))->optional()}, 
    object::pair{std::string("evaluators"), z->array(z->object(object{
        object::pair{std::string("name"), z->string()->regex((new RegExp(std::string("^[a-zA-Z][a-zA-Z0-9]*"))), std::string("Evaluator name must be alphanumeric"))}, 
        object::pair{std::string("description"), z->string()}, 
        object::pair{std::string("triggers"), z->array(z->string())->optional()}
    }))->optional()}, 
    object::pair{std::string("dependencies"), z->record(z->string())->optional()}, 
    object::pair{std::string("environmentVariables"), z->array(z->object(object{
        object::pair{std::string("name"), z->string()}, 
        object::pair{std::string("description"), z->string()}, 
        object::pair{std::string("required"), z->boolean()}, 
        object::pair{std::string("sensitive"), z->boolean()}
    }))->optional()}
});
std::shared_ptr<Action> createPluginAction = object{
    object::pair{std::string("name"), std::string("createPlugin")}, 
    object::pair{std::string("description"), std::string("Create a new plugin from a specification using AI assistance")}, 
    object::pair{std::string("similes"), array<string>{ std::string("generate plugin"), std::string("build plugin"), std::string("make plugin"), std::string("develop plugin"), std::string("create extension"), std::string("build extension") }}, 
    object::pair{std::string("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Create a plugin for managing user preferences")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("agent")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll create a user preferences management plugin for you. Let me start by generating the necessary components...")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Build a plugin that adds weather information capabilities")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("agent")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll create a weather information plugin with actions for fetching current weather, forecasts, and weather alerts.")}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
        if (!service) {
            return false;
        }
        auto jobs = service->getAllJobs();
        auto activeJob = jobs->find([=](auto job) mutable
        {
            return OR((job->status == std::string("running")), (job->status == std::string("pending")));
        }
        );
        if (activeJob) {
            return false;
        }
        if (!isValidJsonSpecification(message->content->text)) {
            return false;
        }
        return validatePrompt(message);
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
            if (!service) {
                return std::string("Plugin creation service not available. Please ensure the plugin is properly installed.");
            }
            std::shared_ptr<PluginSpecification> specification;
            try
            {
                auto parsed = JSON->parse(message->content->text);
                specification = as<std::shared_ptr<PluginSpecification>>(PluginSpecificationSchema->parse(parsed));
            }
            catch (const any& error)
            {
                if (is<z->ZodError>(error)) {
                    return std::string("Invalid plugin specification:\
") + error["errors"]["map"]([=](auto e) mutable
                    {
                        return std::string("- ") + e["path"]->join(std::string(".")) + std::string(": ") + e["message"] + string_empty;
                    }
                    )["join"](std::string("\
")) + string_empty;
                }
                return std::string("Failed to parse specification: ") + error["message"] + string_empty;
            }
            auto apiKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            if (!apiKey) {
                return std::string("ANTHROPIC_API_KEY is not configured. Please set it to enable AI-powered plugin generation.");
            }
            auto jobId = std::async([=]() { service->createPlugin(specification, apiKey); });
            return std::string("Plugin creation job started successfully!\
\
Job ID: ") + jobId + std::string("\
Plugin: ") + specification->name + std::string("\
\
Use 'checkPluginCreationStatus' to monitor progress.");
        }
        catch (const any& error)
        {
            return std::string("Failed to create plugin: ") + error["message"] + string_empty;
        }
    }
    }
};
std::shared_ptr<Action> checkPluginCreationStatusAction = object{
    object::pair{std::string("name"), std::string("checkPluginCreationStatus")}, 
    object::pair{std::string("description"), std::string("Check the status of a plugin creation job")}, 
    object::pair{std::string("similes"), array<string>{ std::string("plugin status"), std::string("check plugin progress"), std::string("plugin creation status"), std::string("get plugin status") }}, 
    object::pair{std::string("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What's the status of my plugin creation?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("agent")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Let me check the status of your plugin creation job...")}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
        if (!service) {
            return false;
        }
        auto jobs = service->getAllJobs();
        return jobs->get_length() > 0;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
            if (!service) {
                return std::string("Plugin creation service not available.");
            }
            auto jobs = service->getAllJobs();
            if (jobs->get_length() == 0) {
                return std::string("No plugin creation jobs found.");
            }
            auto jobIdMatch = message->content->text->match((new RegExp(std::string("[a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12}"))));
            any targetJob;
            if (jobIdMatch) {
                targetJob = service->getJobStatus(const_(jobIdMatch)[0]);
                if (!targetJob) {
                    return std::string("Job with ID ") + const_(jobIdMatch)[0] + std::string(" not found.");
                }
            } else {
                targetJob = const_(jobs->filter([=](auto job) mutable
                {
                    return OR((job->status == std::string("running")), (job->status == std::string("pending")));
                }
                )->sort([=](auto a, auto b) mutable
                {
                    return b->startedAt->getTime() - a->startedAt->getTime();
                }
                ))[0];
                if (!targetJob) {
                    targetJob = const_(jobs->sort([=](auto a, auto b) mutable
                    {
                        return b->startedAt->getTime() - a->startedAt->getTime();
                    }
                    ))[0];
                }
            }
            if (!targetJob) {
                return std::string("No plugin creation jobs found.");
            }
            shared response = std::string("📦 Plugin Creation Status\
");
            response += std::string("━━━━━━━━━━━━━━━━━━━━━━━━━━━━\
\
");
            response += std::string("🆔 Job ID: ") + targetJob["id"] + std::string("\
");
            response += std::string("📌 Plugin: ") + targetJob["specification"]["name"] + std::string("\
");
            response += std::string("📊 Status: ") + targetJob["status"]["toUpperCase"]() + std::string("\
");
            response += std::string("🔄 Phase: ") + targetJob["currentPhase"] + std::string("\
");
            response += std::string("📈 Progress: ") + Math->round(targetJob["progress"]) + std::string("%\
");
            response += std::string("⏱️ Started: ") + targetJob["startedAt"]["toLocaleString"]() + std::string("\
");
            if (targetJob["completedAt"]) {
                response += std::string("✅ Completed: ") + targetJob["completedAt"]["toLocaleString"]() + std::string("\
");
                auto duration = targetJob["completedAt"]["getTime"]() - targetJob["startedAt"]["getTime"]();
                response += std::string("⏳ Duration: ") + Math->round(duration / 1000) + std::string("s\
");
            }
            if (targetJob["logs"]["length"] > 0) {
                response += std::string("\
📝 Recent Activity:\
");
                targetJob["logs"]["slice"](-5)["forEach"]([=](auto log) mutable
                {
                    response += std::string("  ") + log + std::string("\
");
                }
                );
            }
            if (targetJob["status"] == std::string("completed")) {
                response += std::string("\
✅ Plugin created successfully!\
");
                response += std::string("📂 Location: ") + targetJob["outputPath"] + std::string("\
");
            } else if (targetJob["status"] == std::string("failed")) {
                response += std::string("\
❌ Plugin creation failed\
");
                if (targetJob["error"]) {
                    response += std::string("Error: ") + targetJob["error"] + std::string("\
");
                }
            }
            return response;
        }
        catch (const any& error)
        {
            return std::string("Failed to check status: ") + error["message"] + string_empty;
        }
    }
    }
};
std::shared_ptr<Action> cancelPluginCreationAction = object{
    object::pair{std::string("name"), std::string("cancelPluginCreation")}, 
    object::pair{std::string("description"), std::string("Cancel the current plugin creation job")}, 
    object::pair{std::string("similes"), array<string>{ std::string("stop plugin creation"), std::string("abort plugin creation"), std::string("cancel plugin") }}, 
    object::pair{std::string("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Cancel the plugin creation")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("agent")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll cancel the current plugin creation job.")}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
        if (!service) {
            return false;
        }
        auto jobs = service->getAllJobs();
        auto activeJob = jobs->find([=](auto job) mutable
        {
            return OR((job->status == std::string("running")), (job->status == std::string("pending")));
        }
        );
        return !!activeJob;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
            if (!service) {
                return std::string("Plugin creation service not available.");
            }
            auto jobs = service->getAllJobs();
            auto activeJob = jobs->find([=](auto job) mutable
            {
                return OR((job->status == std::string("running")), (job->status == std::string("pending")));
            }
            );
            if (!activeJob) {
                return std::string("No active plugin creation job to cancel.");
            }
            service->cancelJob(activeJob->id);
            return std::string("Plugin creation job has been cancelled.\
\
Job ID: ") + activeJob->id + std::string("\
Plugin: ") + activeJob->specification->name + string_empty;
        }
        catch (const any& error)
        {
            return std::string("Failed to cancel job: ") + error["message"] + string_empty;
        }
    }
    }
};
std::shared_ptr<Action> createPluginFromDescriptionAction = object{
    object::pair{std::string("name"), std::string("createPluginFromDescription")}, 
    object::pair{std::string("description"), std::string("Create a plugin from a natural language description")}, 
    object::pair{std::string("similes"), array<string>{ std::string("describe plugin"), std::string("plugin from description"), std::string("explain plugin"), std::string("I need a plugin that") }}, 
    object::pair{std::string("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I need a plugin that helps manage todo lists with add, remove, and list functionality")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("agent")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll create a todo list management plugin based on your description. This will include actions for adding, removing, and listing todos.")}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
        if (!service) {
            return false;
        }
        auto jobs = service->getAllJobs();
        auto activeJob = jobs->find([=](auto job) mutable
        {
            return OR((job->status == std::string("running")), (job->status == std::string("pending")));
        }
        );
        if (activeJob) {
            return false;
        }
        return AND((message->content->text), (message->content->text->length > 20));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
            if (!service) {
                return std::string("Plugin creation service not available.");
            }
            auto apiKey = runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
            if (!apiKey) {
                return std::string("ANTHROPIC_API_KEY is not configured. Please set it to enable AI-powered plugin generation.");
            }
            auto specification = std::async([=]() { generatePluginSpecification(message->content->text, runtime); });
            try
            {
                PluginSpecificationSchema->parse(specification);
            }
            catch (const any& error)
            {
                if (is<z->ZodError>(error)) {
                    return std::string("Failed to generate valid specification:\
") + error["errors"]["map"]([=](auto e) mutable
                    {
                        return std::string("- ") + e["path"]->join(std::string(".")) + std::string(": ") + e["message"] + string_empty;
                    }
                    )["join"](std::string("\
")) + string_empty;
                }
            }
            auto jobId = std::async([=]() { service->createPlugin(specification, apiKey); });
            return (std::string("I'm creating a plugin based on your description!\
\
") + std::string("📦 Plugin: ") + specification->name + std::string("\
") + std::string("📝 Description: ") + specification->description + std::string("\
") + std::string("🆔 Job ID: ") + jobId + std::string("\
\
") + std::string("Components to be created:\
") + string_empty + (specification->actions->get_length()) ? any(std::string("- ") + specification->actions->get_length() + std::string(" actions\
")) : any(string_empty) + string_empty + string_empty + (specification->providers->get_length()) ? any(std::string("- ") + specification->providers->get_length() + std::string(" providers\
")) : any(string_empty) + string_empty + string_empty + (specification->services->get_length()) ? any(std::string("- ") + specification->services->get_length() + std::string(" services\
")) : any(string_empty) + string_empty + string_empty + (specification->evaluators->get_length()) ? any(std::string("- ") + specification->evaluators->get_length() + std::string(" evaluators\
")) : any(string_empty) + std::string("\
") + std::string("Use 'checkPluginCreationStatus' to monitor progress."));
        }
        catch (const any& error)
        {
            return std::string("Failed to create plugin: ") + error["message"] + string_empty;
        }
    }
    }
};

void Main(void)
{
}

MAIN
