#include "provider.hpp"

Provider fromV2Provider(std::shared_ptr<ProviderV2> providerV2)
{
    return object{
        object::pair{std:("name"), providerV2->name}, 
        object::pair{std:("description"), providerV2->description}, 
        object::pair{std:("dynamic"), providerV2->dynamic}, 
        object::pair{std:("position"), providerV2->position}, 
        object::pair{std:("private"), providerV2->private}, 
        object::pair{std:("get"), [=](auto runtime, auto message, auto state = undefined) mutable
        {
            auto stateV2 = (state) ? any(toV2State(state)) (undefined);
            try
            {
                auto result = std::async([=]() { providerV2->get(as<any>(runtime), as<any>(message), as<any>(stateV2)); });
                return OR((result->text), (string_empty));
            }
            catch (const any& error)
            {
                console->error(std:("Error in v2 provider ") + providerV2->name + std:(":"), error);
                throw any(error);
            }
        }
        }
    };
};


std::shared_ptr<ProviderV2> toV2Provider(Provider provider)
{
    return object{
        object::pair{std:("name"), OR((provider->name), (std:("unnamed-provider")))}, 
        object::pair{std:("description"), provider->description}, 
        object::pair{std:("dynamic"), provider->dynamic}, 
        object::pair{std:("position"), provider->position}, 
        object::pair{std:("private"), provider->private}, 
        object::pair{std:("get"), [=](auto runtime, auto message, auto state) mutable
        {
            try
            {
                auto result = std::async([=]() { provider->get(runtime, message, state); });
                if (AND((type_of(result) == std:("object")), (result != nullptr))) {
                    return utils::assign(object{
                        , 
                        object::pair{std:("values"), OR((result["values"]), (object{}))}, 
                        object::pair{std:("data"), OR((result["data"]), (object{}))}, 
                        object::pair{std:("text"), OR((result["text"]), (string_empty))}
                    }, result);
                }
                return object{
                    object::pair{std:("values"), object{}}, 
                    object::pair{std:("data"), object{}}, 
                    object::pair{std:("text"), String(OR((result), (string_empty)))}
                };
            }
            catch (const any& error)
            {
                console->error(std:("Error in v1 provider ") + (OR((provider->name), (std:("unnamed")))) + std:(":"), error);
                throw any(error);
            }
        }
        }
    };
};


