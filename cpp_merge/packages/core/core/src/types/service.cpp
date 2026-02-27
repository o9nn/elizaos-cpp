#include "service.hpp"
#include <string>

Service::Service(std::shared_ptr<IAgentRuntime> runtime) {
    if (runtime) {
        this->runtime = runtime;
    }
}

string Service::serviceType;

std::shared_ptr<Promise<std::shared_ptr<Service>>> Service::start(std::shared_ptr<IAgentRuntime> _runtime)
{
    throw any(std::make_shared<Error>(std::string("Not implemented")));
    return std::shared_ptr<Promise<std::shared_ptr<Service>>>();
}

std::shared_ptr<Promise<any>> Service::stop(std::shared_ptr<IAgentRuntime> _runtime)
{
    throw any(std::make_shared<Error>(std::string("Not implemented")));
    return std::shared_ptr<Promise<any>>();
}

std::shared_ptr<ServiceError> createServiceError(any error, string code)
{
    if (is<Error>(error)) {
        return object{
            object::pair{std::string("code"), std::string("code")}, 
            object::pair{std::string("message"), error->message}, 
            object::pair{std::string("cause"), error}
        };
    }
    return object{
        object::pair{std::string("code"), std::string("code")}, 
        object::pair{std::string("message"), String(error)}
    };
};


object ServiceType = as<std::shared_ptr<const>>(object{
    object::pair{std::string("TRANSCRIPTION"), std::string("transcription")}, 
    object::pair{std::string("VIDEO"), std::string("video")}, 
    object::pair{std::string("BROWSER"), std::string("browser")}, 
    object::pair{std::string("PDF"), std::string("pdf")}, 
    object::pair{std::string("REMOTE_FILES"), std::string("aws_s3")}, 
    object::pair{std::string("WEB_SEARCH"), std::string("web_search")}, 
    object::pair{std::string("EMAIL"), std::string("email")}, 
    object::pair{std::string("TEE"), std::string("tee")}, 
    object::pair{std::string("TASK"), std::string("task")}, 
    object::pair{std::string("WALLET"), std::string("wallet")}, 
    object::pair{std::string("LP_POOL"), std::string("lp_pool")}, 
    object::pair{std::string("TOKEN_DATA"), std::string("token_data")}, 
    object::pair{std::string("DATABASE_MIGRATION"), std::string("database_migration")}, 
    object::pair{std::string("PLUGIN_MANAGER"), std::string("PLUGIN_MANAGER")}, 
    object::pair{std::string("PLUGIN_CONFIGURATION"), std::string("PLUGIN_CONFIGURATION")}, 
    object::pair{std::string("PLUGIN_USER_INTERACTION"), std::string("PLUGIN_USER_INTERACTION")}
}), satisfies, ServiceTypeRegistry;

void Main(void)
{
}

MAIN
