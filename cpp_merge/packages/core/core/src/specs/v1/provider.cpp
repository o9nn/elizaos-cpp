#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/core/src/specs/v1/provider.h"

Provider fromV2Provider(std::shared_ptr<ProviderV2> providerV2)
{
    return object{
        object::pair{std::string("name"), providerV2->name}, 
        object::pair{std::string("description"), providerV2->description}, 
        object::pair{std::string("dynamic"), providerV2->dynamic}, 
        object::pair{std::string("position"), providerV2->position}, 
        object::pair{std::string("private"), providerV2->private}, 
        object::pair{std::string("get"), [=](auto runtime, auto message, auto state = undefined) mutable
        {
            auto stateV2 = (state) ? any(toV2State(state)) : any(undefined);
            try
            {
                auto result = std::async([=]() { providerV2->get(as<any>(runtime), as<any>(message), as<any>(stateV2)); });
                return OR((result->text), (string_empty));
            }
            catch (const any& error)
            {
                console->error(std::string("Error in v2 provider ") + providerV2->name + std::string(":"), error);
                throw any(error);
            }
        }
        }
    };
};


std::shared_ptr<ProviderV2> toV2Provider(Provider provider)
{
    return object{
        object::pair{std::string("name"), OR((provider->name), (std::string("unnamed-provider")))}, 
        object::pair{std::string("description"), provider->description}, 
        object::pair{std::string("dynamic"), provider->dynamic}, 
        object::pair{std::string("position"), provider->position}, 
        object::pair{std::string("private"), provider->private}, 
        object::pair{std::string("get"), [=](auto runtime, auto message, auto state) mutable
        {
            try
            {
                auto result = std::async([=]() { provider->get(runtime, message, state); });
                if (AND((type_of(result) == std::string("object")), (result != nullptr))) {
                    return utils::assign(object{
                        , 
                        object::pair{std::string("values"), OR((result["values"]), (object{}))}, 
                        object::pair{std::string("data"), OR((result["data"]), (object{}))}, 
                        object::pair{std::string("text"), OR((result["text"]), (string_empty))}
                    }, result);
                }
                return object{
                    object::pair{std::string("values"), object{}}, 
                    object::pair{std::string("data"), object{}}, 
                    object::pair{std::string("text"), String(OR((result), (string_empty)))}
                };
            }
            catch (const any& error)
            {
                console->error(std::string("Error in v1 provider ") + (OR((provider->name), (std::string("unnamed")))) + std::string(":"), error);
                throw any(error);
            }
        }
        }
    };
};


