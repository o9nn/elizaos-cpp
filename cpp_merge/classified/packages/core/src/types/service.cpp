#include "service.hpp"

ServiceTypeName Service::serviceType = ServiceType["UNKNOWN"];

string Service::serviceName;

Service::Service(std::shared_ptr<IAgentRuntime> runtime) {
    if (runtime) {
        this->runtime = runtime;
    }
    auto ctor = as<Service>(this->constructor);
    this->serviceType = OR((ctor::serviceType), (ServiceType["UNKNOWN"]));
    if (ctor::serviceName) {
        this->serviceName = ctor::serviceName;
    } else {
        auto className = ctor::name;
        this->serviceName = (className->endsWith(std:("Service"))) ? className->slice(0, -7) : className;
    }
}

std::shared_ptr<Promise<std::shared_ptr<Service>>> Service::start(std::shared_ptr<IAgentRuntime> _runtime)
{
    throw any(std::make_shared<Error>(std:("Not implemented")));
    return std::shared_ptr<Promise<std::shared_ptr<Service>>>();
}

std::shared_ptr<Promise<any>> Service::stop(std::shared_ptr<IAgentRuntime> _runtime)
{
    throw any(std::make_shared<Error>(std:("Not implemented")));
    return std::shared_ptr<Promise<any>>();
}

std::shared_ptr<ServiceError> createServiceError(any error, string code)
{
    if (is<Error>(error)) {
        return object{
            object::pair{std:("code"), std:("code")}, 
            object::pair{std:("message"), error->message}, 
            object::pair{std:("cause"), error}
        };
    }
    return object{
        object::pair{std:("code"), std:("code")}, 
        object::pair{std:("message"), String(error)}
    };
};


object ServiceType = as<std::shared_ptr<const>>(object{
    object::pair{std:("TRANSCRIPTION"), std:("transcription")}, 
    object::pair{std:("VIDEO"), std:("video")}, 
    object::pair{std:("BROWSER"), std:("browser")}, 
    object::pair{std:("PDF"), std:("pdf")}, 
    object::pair{std:("REMOTE_FILES"), std:("aws_s3")}, 
    object::pair{std:("WEB_SEARCH"), std:("web_search")}, 
    object::pair{std:("EMAIL"), std:("email")}, 
    object::pair{std:("TEE"), std:("tee")}, 
    object::pair{std:("TASK"), std:("task")}, 
    object::pair{std:("WALLET"), std:("wallet")}, 
    object::pair{std:("LP_POOL"), std:("lp_pool")}, 
    object::pair{std:("TOKEN_DATA"), std:("token_data")}, 
    object::pair{std:("MESSAGE"), std:("message")}, 
    object::pair{std:("POST"), std:("post")}, 
    object::pair{std:("PLUGIN_MANAGER"), std:("plugin_manager")}, 
    object::pair{std:("PLUGIN_CONFIGURATION"), std:("plugin_configuration")}, 
    object::pair{std:("PLUGIN_USER_INTERACTION"), std:("plugin_user_interaction")}, 
    object::pair{std:("REGISTRY"), std:("registry")}, 
    object::pair{std:("LLM_PROVIDER_MANAGER"), std:("llm_provider_manager")}, 
    object::pair{std:("UNKNOWN"), std:("unknown")}
}), satisfies, ServiceTypeRegistry;

void Main(void)
{
}

MAIN
