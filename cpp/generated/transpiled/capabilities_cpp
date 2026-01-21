#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-bootstrap/providers/capabilities.h"

std::shared_ptr<Provider> capabilitiesProvider = object{
    object::pair{std::string("name"), std::string("CAPABILITIES")}, 
    object::pair{std::string("get"), [=](auto runtime, auto _message) mutable
    {
        try
        {
            auto services = runtime->getAllServices();
            if (OR((!services), (services->size == 0))) {
                return object{
                    object::pair{std::string("text"), std::string("No services are currently registered.")}
                };
            }
            auto capabilities = array<string>();
            for (auto& [serviceType, service] : services)
            {
                if (service->capabilityDescription) {
                    capabilities->push(string_empty + serviceType + std::string(" - ") + service->capabilityDescription->replace(std::string("{{agentName}}"), runtime->character->name) + string_empty);
                }
            }
            if (capabilities->get_length() == 0) {
                return object{
                    object::pair{std::string("text"), std::string("No capability descriptions found in the registered services.")}
                };
            }
            auto formattedCapabilities = capabilities->join(std::string("\
"));
            return object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("capabilities"), std::string("capabilities")}
                }}, 
                object::pair{std::string("text"), std::string("# ") + runtime->character->name + std::string("'s Capabilities\
\
") + formattedCapabilities + string_empty}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in capabilities provider:"), error);
            return object{
                object::pair{std::string("text"), std::string("Error retrieving capabilities from services.")}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
