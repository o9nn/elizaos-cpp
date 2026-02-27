#include "capabilities.hpp"
#include <string>

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
            auto& __array994_1532 = services;
            for (auto __indx994_1532 = 0_N; __indx994_1532 < __array994_1532->get_length(); __indx994_1532++)
            {
                auto& [serviceType, serviceArray] = const_(__array994_1532)[__indx994_1532];
                {
                    if (AND((serviceArray), (serviceArray->length > 0))) {
                        auto service = const_(serviceArray)[0];
                        if (service->capabilityDescription) {
                            capabilities->push(string_empty + serviceType + std::string(" - ") + service->capabilityDescription->replace(std::string("{{agentName}}"), runtime->character->name) + string_empty);
                        }
                    }
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
            logger->error(object{
                object::pair{std::string("error"), std::string("error")}
            }, std::string("Error in capabilities provider:"));
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
