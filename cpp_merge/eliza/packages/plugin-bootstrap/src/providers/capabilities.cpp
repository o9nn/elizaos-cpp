#include "capabilities.hpp"

std::shared_ptr<Provider> capabilitiesProvider = object{
    object::pair{std:("name"), std:("CAPABILITIES")}, 
    object::pair{std:("get"), [=](auto runtime, auto _message) mutable
    {
        try
        {
            auto services = runtime->getAllServices();
            if (OR((!services), (services->size == 0))) {
                return object{
                    object::pair{std:("text"), std:("No services are currently registered.")}
                };
            }
            auto capabilities = array<string>();
            for (auto& [serviceType, service] : services)
            {
                if (service->capabilityDescription) {
                    capabilities->push(string_empty + serviceType + std:(" - ") + service->capabilityDescription->replace(std:("{{agentName}}"), runtime->character->name) + string_empty);
                }
            }
            if (capabilities->get_length() == 0) {
                return object{
                    object::pair{std:("text"), std:("No capability descriptions found in the registered services.")}
                };
            }
            auto formattedCapabilities = capabilities->join(std:("\
"));
            return object{
                object::pair{std:("data"), object{
                    object::pair{std:("capabilities"), std:("capabilities")}
                }}, 
                object::pair{std:("text"), std:("# ") + runtime->character->name + std:("'s Capabilities\
\
") + formattedCapabilities + string_empty}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error in capabilities provider:"), error);
            return object{
                object::pair{std:("text"), std:("Error retrieving capabilities from services.")}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
