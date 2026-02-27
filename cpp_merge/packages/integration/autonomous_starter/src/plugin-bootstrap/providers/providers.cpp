#include "providers.hpp"

std::shared_ptr<Provider> providersProvider = object{
    object::pair{std:("name"), std:("PROVIDERS")}, 
    object::pair{std:("description"), std:("List of all data providers the agent can use to get additional information")}, 
    object::pair{std:("get"), [=](auto runtime, auto _message) mutable
    {
        auto dynamicProviders = runtime->providers->filter([=](auto provider) mutable
        {
            return provider["dynamic"] == true;
        }
        );
        auto providerDescriptions = dynamicProviders->map([=](auto provider) mutable
        {
            return std:("- **") + provider["name"] + std:("**: ") + (OR((provider["description"]), (std:("No description available")))) + string_empty;
        }
        );
        auto headerText = std:("# Providers\
\
These providers are available for the agent to select and use:");
        if (providerDescriptions->length == 0) {
            return object{
                object::pair{std:("text"), addHeader(headerText, std:("No dynamic providers are currently available."))}
            };
        }
        auto providersText = providerDescriptions->join(std:("\
"));
        auto text = addHeader(headerText, providersText);
        auto data = object{
            object::pair{std:("dynamicProviders"), dynamicProviders->map([=](auto provider) mutable
            {
                return (object{
                    object::pair{std:("name"), provider["name"]}, 
                    object::pair{std:("description"), OR((provider["description"]), (string_empty))}
                });
            }
            )}
        };
        return object{
            object::pair{std:("text"), std:("text")}, 
            object::pair{std:("data"), std:("data")}
        };
    }
    }
};

void Main(void)
{
}

MAIN
