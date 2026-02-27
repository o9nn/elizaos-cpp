#include "plugin-creation-actions.h"

std::shared_ptr<Promise<std::shared_ptr<PluginSpecification>>> generatePluginSpecification(string description, std::shared_ptr<IAgentRuntime> runtime)
{
    shared lowerDesc = description->toLowerCase();
    auto name = std:("@elizaos/plugin-");
    auto pluginType = std:("custom");
    if (lowerDesc->includes(std:("weather"))) {
        pluginType = std:("weather");
        name += std:("weather");
    } else if (OR((lowerDesc->includes(std:("database"))), (lowerDesc->includes(std:("sql"))))) {
        pluginType = std:("database");
        name += std:("database");
    } else if (OR((lowerDesc->includes(std:("api"))), (lowerDesc->includes(std:("rest"))))) {
        pluginType = std:("api");
        name += std:("api");
    } else if (OR((lowerDesc->includes(std:("todo"))), (lowerDesc->includes(std:("task"))))) {
        pluginType = std:("todo");
        name += std:("todo");
    } else if (OR((lowerDesc->includes(std:("email"))), (lowerDesc->includes(std:("mail"))))) {
        pluginType = std:("email");
        name += std:("email");
    } else if (OR((lowerDesc->includes(std:("chat"))), (lowerDesc->includes(std:("message"))))) {
        pluginType = std:("chat");
        name += std:("chat");
    } else {
        auto words = description->split((new RegExp(std:("\s"))))->filter([=](auto w) mutable
        {
            return w->get_length() > 4;
        }
        );
        name += OR((const_(words)[0]->toLowerCase()), (std:("custom")));
    }
    auto specification = object{
        object::pair{std:("name"), std:("name")}, 
        object::pair{std:("description"), description->slice(0, 200)}, 
        object::pair{std:("version"), std:("1.0.0")}, 
        object::pair{std:("actions"), array<any>()}, 
        object::pair{std:("providers"), array<any>()}, 
        object::pair{std:("services"), array<any>()}, 
        object::pair{std:("evaluators"), array<any>()}
    };
    auto actionKeywords = object{
        object::pair{std:("create"), array<string>{ std:("create"), std:("add"), std:("new"), std:("generate"), std:("make") }}, 
        object::pair{std:("read"), array<string>{ std:("get"), std:("fetch"), std:("retrieve"), std:("list"), std:("show"), std:("display") }}, 
        object::pair{std:("update"), array<string>{ std:("update"), std:("modify"), std:("change"), std:("edit"), std:("set") }}, 
        object::pair{std:("delete"), array<string>{ std:("delete"), std:("remove"), std:("clear"), std:("destroy") }}, 
        object::pair{std:("execute"), array<string>{ std:("execute"), std:("run"), std:("perform"), std:("do"), std:("process") }}
    };
    for (auto& [actionType, keywords] : Object->entries(actionKeywords))
    {
        if (keywords->some([=](auto kw) mutable
        {
            return lowerDesc->includes(kw);
        }
        )) {
            specification->actions->push(object{
                object::pair{std:("name"), string_empty + actionType + string_empty + (pluginType->charAt(0)->toUpperCase() + pluginType->slice(1)) + string_empty}, 
                object::pair{std:("description"), string_empty + (actionType->charAt(0)->toUpperCase() + actionType->slice(1)) + std:(" operation for ") + pluginType + string_empty}, 
                object::pair{std:("parameters"), object{}}
            });
        }
    }
    if (OR((OR((OR((lowerDesc->includes(std:("provide"))), (lowerDesc->includes(std:("information"))))), (lowerDesc->includes(std:("data"))))), (lowerDesc->includes(std:("context"))))) {
        specification->providers->push(object{
            object::pair{std:("name"), string_empty + pluginType + std:("Provider")}, 
            object::pair{std:("description"), std:("Provides ") + pluginType + std:(" data and context")}, 
            object::pair{std:("dataStructure"), object{}}
        });
    }
    if (OR((OR((OR((lowerDesc->includes(std:("service"))), (lowerDesc->includes(std:("background"))))), (lowerDesc->includes(std:("monitor"))))), (lowerDesc->includes(std:("watch"))))) {
        specification->services->push(object{
            object::pair{std:("name"), string_empty + pluginType + std:("Service")}, 
            object::pair{std:("description"), std:("Background service for ") + pluginType + std:(" operations")}, 
            object::pair{std:("methods"), array<string>{ std:("start"), std:("stop"), std:("status") }}
        });
    }
    if (OR((OR((OR((lowerDesc->includes(std:("evaluate"))), (lowerDesc->includes(std:("analyze"))))), (lowerDesc->includes(std:("check"))))), (lowerDesc->includes(std:("validate"))))) {
        specification->evaluators->push(object{
            object::pair{std:("name"), string_empty + pluginType + std:("Evaluator")}, 
            object::pair{std:("description"), std:("Evaluates and analyzes ") + pluginType + std:(" data")}, 
            object::pair{std:("triggers"), array<any>()}
        });
    }
    if (AND((AND((AND((!specification->actions->get_length()), (!specification->providers->get_length()))), (!specification->services->get_length()))), (!specification->evaluators->get_length()))) {
        specification->actions = array<object>{ object{
            object::pair{std:("name"), std:("handle") + (pluginType->charAt(0)->toUpperCase() + pluginType->slice(1)) + string_empty}, 
            object::pair{std:("description"), std:("Main handler for ") + pluginType + std:(" operations")}
        } };
    }
    return specification;
};


any PluginSpecificationSchema = z->object(object{
    object::pair{std:("name"), z->string()->regex((new RegExp(std:("^@?[a-zA-Z0-9-_]+\/[a-zA-Z0-9-_]+"))), std:("Invalid plugin name format"))}, 
    object::pair{std:("description"), z->string()->min(10, std:("Description must be at least 10 characters"))}, 
    object::pair{std:("version"), z->string()->regex((new RegExp(std:("^\d+\.\d+\.\d+"))), std:("Version must be in semver format"))->optional()->default(std:("1.0.0"))}, 
    object::pair{std:("actions"), z->array(z->object(object{
        object::pair{std:("name"), z->string()->regex((new RegExp(std:("^[a-zA-Z][a-zA-Z0-9]*"))), std:("Action name must be alphanumeric"))}, 
        object::pair{std:("description"), z->string()}, 
        object::pair{std:("parameters"), z->record(z->any())->optional()}
    }))->optional()}, 
    object::pair{std:("providers"), z->array(z->object(object{
        object::pair{std:("name"), z->string()->regex((new RegExp(std:("^[a-zA-Z][a-zA-Z0-9]*"))), std:("Provider name must be alphanumeric"))}, 
        object::pair{std:("description"), z->string()}, 
        object::pair{std:("dataStructure"), z->record(z->any())->optional()}
    }))->optional()}, 
    object::pair{std:("services"), z->array(z->object(object{
        object::pair{std:("name"), z->string()->regex((new RegExp(std:("^[a-zA-Z][a-zA-Z0-9]*"))), std:("Service name must be alphanumeric"))}, 
        object::pair{std:("description"), z->string()}, 
        object::pair{std:("methods"), z->array(z->string())->optional()}
    }))->optional()}, 
    object::pair{std:("evaluators"), z->array(z->object(object{
        object::pair{std:("name"), z->string()->regex((new RegExp(std:("^[a-zA-Z][a-zA-Z0-9]*"))), std:("Evaluator name must be alphanumeric"))}, 
        object::pair{std:("description"), z->string()}, 
        object::pair{std:("triggers"), z->array(z->string())->optional()}
    }))->optional()}, 
    object::pair{std:("dependencies"), z->record(z->string())->optional()}, 
    object::pair{std:("environmentVariables"), z->array(z->object(object{
        object::pair{std:("name"), z->string()}, 
        object::pair{std:("description"), z->string()}, 
        object::pair{std:("required"), z->boolean()}, 
        object::pair{std:("sensitive"), z->boolean()}
    }))->optional()}
});
std::shared_ptr<Action> createPluginAction = object{
    object::pair{std:("name"), std:("createPlugin")}, 
    object::pair{std:("description"), std:("Create a new plugin from a specification using AI assistance")}, 
    object::pair{std:("similes"), array<string>{ std:("generate plugin"), std:("build plugin"), std:("make plugin"), std:("develop plugin"), std:("create extension"), std:("build extension") }}, 
    object::pair{std:("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Create a plugin for managing user preferences")}
        }}
    }, object{
        object::pair{std:("name"), std:("agent")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll create a user preferences management plugin for you. Let me start by generating the necessary components...")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Build a plugin that adds weather information capabilities")}
        }}
    }, object{
        object::pair{std:("name"), std:("agent")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll create a weather information plugin with actions for fetching current weather, forecasts, and weather alerts.")}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
        if (!service) {
            return false;
        }
        auto jobs = service->getAllJobs();
        auto activeJob = jobs->find([=](auto job) mutable
        {
            return OR((job->status == std:("running")), (job->status == std:("pending")));
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
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
            if (!service) {
                return std:("Plugin creation service not available. Please ensure the plugin is properly installed.");
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
                    return std:("Invalid plugin specification:\
") + error["errors"]["map"]([=](auto e) mutable
                    {
                        return std:("- ") + e["path"]->join(std:(".")) + std:(": ") + e["message"] + string_empty;
                    }
                    )["join"](std:("\
")) + string_empty;
                }
                return std:("Failed to parse specification: ") + error["message"] + string_empty;
            }
            auto apiKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            if (!apiKey) {
                return std:("ANTHROPIC_API_KEY is not configured. Please set it to enable AI-powered plugin generation.");
            }
            auto jobId = std::async([=]() { service->createPlugin(specification, apiKey); });
            return std:("Plugin creation job started successfully!\
\
Job ID: ") + jobId + std:("\
Plugin: ") + specification->name + std:("\
\
Use 'checkPluginCreationStatus' to monitor progress.");
        }
        catch (const any& error)
        {
            return std:("Failed to create plugin: ") + error["message"] + string_empty;
        }
    }
    }
};
std::shared_ptr<Action> checkPluginCreationStatusAction = object{
    object::pair{std:("name"), std:("checkPluginCreationStatus")}, 
    object::pair{std:("description"), std:("Check the status of a plugin creation job")}, 
    object::pair{std:("similes"), array<string>{ std:("plugin status"), std:("check plugin progress"), std:("plugin creation status"), std:("get plugin status") }}, 
    object::pair{std:("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What's the status of my plugin creation?")}
        }}
    }, object{
        object::pair{std:("name"), std:("agent")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Let me check the status of your plugin creation job...")}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
        if (!service) {
            return false;
        }
        auto jobs = service->getAllJobs();
        return jobs->get_length() > 0;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
            if (!service) {
                return std:("Plugin creation service not available.");
            }
            auto jobs = service->getAllJobs();
            if (jobs->get_length() == 0) {
                return std:("No plugin creation jobs found.");
            }
            auto jobIdMatch = message->content->text->match((new RegExp(std:("[a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12}"))));
            any targetJob;
            if (jobIdMatch) {
                targetJob = service->getJobStatus(const_(jobIdMatch)[0]);
                if (!targetJob) {
                    return std:("Job with ID ") + const_(jobIdMatch)[0] + std:(" not found.");
                }
            } else {
                targetJob = const_(jobs->filter([=](auto job) mutable
                {
                    return OR((job->status == std:("running")), (job->status == std:("pending")));
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
                return std:("No plugin creation jobs found.");
            }
            shared response = std:("📦 Plugin Creation Status\
");
            response += std:("━━━━━━━━━━━━━━━━━━━━━━━━━━━━\
\
");
            response += std:("🆔 Job ID: ") + targetJob["id"] + std:("\
");
            response += std:("📌 Plugin: ") + targetJob["specification"]["name"] + std:("\
");
            response += std:("📊 Status: ") + targetJob["status"]["toUpperCase"]() + std:("\
");
            response += std:("🔄 Phase: ") + targetJob["currentPhase"] + std:("\
");
            response += std:("📈 Progress: ") + Math->round(targetJob["progress"]) + std:("%\
");
            response += std:("⏱️ Started: ") + targetJob["startedAt"]["toLocaleString"]() + std:("\
");
            if (targetJob["completedAt"]) {
                response += std:("✅ Completed: ") + targetJob["completedAt"]["toLocaleString"]() + std:("\
");
                auto duration = targetJob["completedAt"]["getTime"]() - targetJob["startedAt"]["getTime"]();
                response += std:("⏳ Duration: ") + Math->round(duration / 1000) + std:("s\
");
            }
            if (targetJob["logs"]["length"] > 0) {
                response += std:("\
📝 Recent Activity:\
");
                targetJob["logs"]["slice"](-5)["forEach"]([=](auto log) mutable
                {
                    response += std:("  ") + log + std:("\
");
                }
                );
            }
            if (targetJob["status"] == std:("completed")) {
                response += std:("\
✅ Plugin created successfully!\
");
                response += std:("📂 Location: ") + targetJob["outputPath"] + std:("\
");
            } else if (targetJob["status"] == std:("failed")) {
                response += std:("\
❌ Plugin creation failed\
");
                if (targetJob["error"]) {
                    response += std:("Error: ") + targetJob["error"] + std:("\
");
                }
            }
            return response;
        }
        catch (const any& error)
        {
            return std:("Failed to check status: ") + error["message"] + string_empty;
        }
    }
    }
};
std::shared_ptr<Action> cancelPluginCreationAction = object{
    object::pair{std:("name"), std:("cancelPluginCreation")}, 
    object::pair{std:("description"), std:("Cancel the current plugin creation job")}, 
    object::pair{std:("similes"), array<string>{ std:("stop plugin creation"), std:("abort plugin creation"), std:("cancel plugin") }}, 
    object::pair{std:("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Cancel the plugin creation")}
        }}
    }, object{
        object::pair{std:("name"), std:("agent")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll cancel the current plugin creation job.")}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
        if (!service) {
            return false;
        }
        auto jobs = service->getAllJobs();
        auto activeJob = jobs->find([=](auto job) mutable
        {
            return OR((job->status == std:("running")), (job->status == std:("pending")));
        }
        );
        return !!activeJob;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
            if (!service) {
                return std:("Plugin creation service not available.");
            }
            auto jobs = service->getAllJobs();
            auto activeJob = jobs->find([=](auto job) mutable
            {
                return OR((job->status == std:("running")), (job->status == std:("pending")));
            }
            );
            if (!activeJob) {
                return std:("No active plugin creation job to cancel.");
            }
            service->cancelJob(activeJob->id);
            return std:("Plugin creation job has been cancelled.\
\
Job ID: ") + activeJob->id + std:("\
Plugin: ") + activeJob->specification->name + string_empty;
        }
        catch (const any& error)
        {
            return std:("Failed to cancel job: ") + error["message"] + string_empty;
        }
    }
    }
};
std::shared_ptr<Action> createPluginFromDescriptionAction = object{
    object::pair{std:("name"), std:("createPluginFromDescription")}, 
    object::pair{std:("description"), std:("Create a plugin from a natural language description")}, 
    object::pair{std:("similes"), array<string>{ std:("describe plugin"), std:("plugin from description"), std:("explain plugin"), std:("I need a plugin that") }}, 
    object::pair{std:("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I need a plugin that helps manage todo lists with add, remove, and list functionality")}
        }}
    }, object{
        object::pair{std:("name"), std:("agent")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll create a todo list management plugin based on your description. This will include actions for adding, removing, and listing todos.")}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
        if (!service) {
            return false;
        }
        auto jobs = service->getAllJobs();
        auto activeJob = jobs->find([=](auto job) mutable
        {
            return OR((job->status == std:("running")), (job->status == std:("pending")));
        }
        );
        if (activeJob) {
            return false;
        }
        return AND((message->content->text), (message->content->text->length > 20));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std:("plugin_creation")));
            if (!service) {
                return std:("Plugin creation service not available.");
            }
            auto apiKey = runtime->getSetting(std:("ANTHROPIC_API_KEY"));
            if (!apiKey) {
                return std:("ANTHROPIC_API_KEY is not configured. Please set it to enable AI-powered plugin generation.");
            }
            auto specification = std::async([=]() { generatePluginSpecification(message->content->text, runtime); });
            try
            {
                PluginSpecificationSchema->parse(specification);
            }
            catch (const any& error)
            {
                if (is<z->ZodError>(error)) {
                    return std:("Failed to generate valid specification:\
") + error["errors"]["map"]([=](auto e) mutable
                    {
                        return std:("- ") + e["path"]->join(std:(".")) + std:(": ") + e["message"] + string_empty;
                    }
                    )["join"](std:("\
")) + string_empty;
                }
            }
            auto jobId = std::async([=]() { service->createPlugin(specification, apiKey); });
            return (std:("I'm creating a plugin based on your description!\
\
") + std:("📦 Plugin: ") + specification->name + std:("\
") + std:("📝 Description: ") + specification->description + std:("\
") + std:("🆔 Job ID: ") + jobId + std:("\
\
") + std:("Components to be created:\
") + string_empty + (specification->actions->get_length()) ? any(std:("- ") + specification->actions->get_length() + std:(" actions\
")) (string_empty) + string_empty + string_empty + (specification->providers->get_length()) ? any(std:("- ") + specification->providers->get_length() + std:(" providers\
")) (string_empty) + string_empty + string_empty + (specification->services->get_length()) ? any(std:("- ") + specification->services->get_length() + std:(" services\
")) (string_empty) + string_empty + string_empty + (specification->evaluators->get_length()) ? any(std:("- ") + specification->evaluators->get_length() + std:(" evaluators\
")) (string_empty) + std:("\
") + std:("Use 'checkPluginCreationStatus' to monitor progress."));
        }
        catch (const any& error)
        {
            return std:("Failed to create plugin: ") + error["message"] + string_empty;
        }
    }
    }
};

void Main(void)
{
}

MAIN
