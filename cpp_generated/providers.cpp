#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-bootstrap/providers/providers.h"

std::shared_ptr<Provider> providersProvider = object{
    object::pair{std::string("name"), std::string("PROVIDERS")}, 
    object::pair{std::string("description"), std::string("List of all data providers the agent can use to get additional information")}, 
    object::pair{std::string("get"), [=](auto runtime, auto _message) mutable
    {
        auto dynamicProviders = runtime->providers->filter([=](auto provider) mutable
        {
            return provider["dynamic"] == true;
        }
        );
        auto providerDescriptions = dynamicProviders->map([=](auto provider) mutable
        {
            return std::string("- **") + provider["name"] + std::string("**: ") + (OR((provider["description"]), (std::string("No description available")))) + string_empty;
        }
        );
        auto headerText = std::string("# Providers\
\
These providers are available for the agent to select and use:");
        if (providerDescriptions->length == 0) {
            return object{
                object::pair{std::string("text"), addHeader(headerText, std::string("No dynamic providers are currently available."))}
            };
        }
        auto providersText = providerDescriptions->join(std::string("\
"));
        auto text = addHeader(headerText, providersText);
        auto data = object{
            object::pair{std::string("dynamicProviders"), dynamicProviders->map([=](auto provider) mutable
            {
                return (object{
                    object::pair{std::string("name"), provider["name"]}, 
                    object::pair{std::string("description"), OR((provider["description"]), (string_empty))}
                });
            }
            )}
        };
        return object{
            object::pair{std::string("text"), std::string("text")}, 
            object::pair{std::string("data"), std::string("data")}
        };
    }
    }
};

void Main(void)
{
}

MAIN
