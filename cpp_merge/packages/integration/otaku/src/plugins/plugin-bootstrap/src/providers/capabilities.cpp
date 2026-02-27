#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-bootstrap/src/providers/capabilities.h"

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
            auto& __array994_1532 = services;
            for (auto __indx994_1532 = 0_N; __indx994_1532 < __array994_1532->get_length(); __indx994_1532++)
            {
                auto& [serviceType, serviceArray] = const_(__array994_1532)[__indx994_1532];
                {
                    if (AND((serviceArray), (serviceArray->length > 0))) {
                        auto service = const_(serviceArray)[0];
                        if (service->capabilityDescription) {
                            capabilities->push(string_empty + serviceType + std:(" - ") + service->capabilityDescription->replace(std:("{{agentName}}"), runtime->character->name) + string_empty);
                        }
                    }
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
            logger->error(object{
                object::pair{std:("error"), std:("error")}
            }, std:("Error in capabilities provider:"));
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
