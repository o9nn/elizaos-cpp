#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-dynamic/providers/plugin-creation-providers.h"

std::shared_ptr<Provider> pluginCreationStatusProvider = object{
    object::pair{std::string("name"), std::string("plugin_creation_status")}, 
    object::pair{std::string("description"), std::string("Provides status of active plugin creation jobs")}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state) mutable
    {
        auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
        if (!service) {
            return object{
                object::pair{std::string("text"), std::string("Plugin creation service not available")}
            };
        }
        auto jobs = service->getAllJobs();
        auto activeJobs = jobs->filter([=](auto job) mutable
        {
            return OR((job->status == std::string("running")), (job->status == std::string("pending")));
        }
        );
        if (activeJobs->get_length() == 0) {
            return object{
                object::pair{std::string("text"), std::string("No active plugin creation jobs")}
            };
        }
        auto job = const_(activeJobs)[0];
        return object{
            object::pair{std::string("text"), std::string("Active plugin creation: ") + job->specification->name + std::string(" - Status: ") + job->status + std::string(", Phase: ") + job->currentPhase + std::string(", Progress: ") + Math->round(job->progress) + std::string("%")}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("jobId"), job->id}, 
                object::pair{std::string("pluginName"), job->specification->name}, 
                object::pair{std::string("status"), job->status}, 
                object::pair{std::string("phase"), job->currentPhase}, 
                object::pair{std::string("progress"), job->progress}
            }}
        };
    }
    }
};
std::shared_ptr<Provider> pluginCreationCapabilitiesProvider = object{
    object::pair{std::string("name"), std::string("plugin_creation_capabilities")}, 
    object::pair{std::string("description"), std::string("Provides information about plugin creation capabilities")}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state) mutable
    {
        auto service = as<std::shared_ptr<PluginCreationService>>(runtime->services->get(std::string("plugin_creation")));
        auto hasService = !!service;
        auto hasApiKey = !!runtime->getSetting(std::string("ANTHROPIC_API_KEY"));
        if (!hasService) {
            return object{
                object::pair{std::string("text"), std::string("Plugin creation service is not available")}
            };
        }
        if (!hasApiKey) {
            return object{
                object::pair{std::string("text"), std::string("Plugin creation is available but requires ANTHROPIC_API_KEY for AI-powered code generation")}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("serviceAvailable"), true}, 
                    object::pair{std::string("aiEnabled"), false}
                }}
            };
        }
        return object{
            object::pair{std::string("text"), std::string("Plugin creation service is fully operational with AI-powered code generation")}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("serviceAvailable"), true}, 
                object::pair{std::string("aiEnabled"), true}, 
                object::pair{std::string("supportedComponents"), array<string>{ std::string("actions"), std::string("providers"), std::string("services"), std::string("evaluators") }}, 
                object::pair{std::string("maxIterations"), 5}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
