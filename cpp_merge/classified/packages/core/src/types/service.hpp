#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_SERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_SERVICE_H
#include "core.h"
#include "./primitives.h"
#include "./runtime.h"

typedef any ServiceTypeName;

template <typename K>
using ServiceTypeValue = any;

template <typename T>
using IsValidServiceType = T;

template <typename T>
using TypedServiceClass = object;

template <typename T>
using ServiceInstance = T;

template <typename T>
using ServiceRegistry = Map<T, std::shared_ptr<Service>>;

class ServiceTypeRegistry;
class ServiceClassMap;
class Service;
template <typename ConfigType, typename ResultType>
class TypedService;
class ServiceError;

class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
public:
    using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
    string TRANSCRIPTION;

    string VIDEO;

    string BROWSER;

    string PDF;

    string REMOTE_FILES;

    string WEB_SEARCH;

    string EMAIL;

    string TEE;

    string TASK;

    string WALLET;

    string LP_POOL;

    string TOKEN_DATA;

    string MESSAGE;

    string POST;

    string PLUGIN_MANAGER;

    string PLUGIN_CONFIGURATION;

    string PLUGIN_USER_INTERACTION;

    string REGISTRY;

    string LLM_PROVIDER_MANAGER;

    string UNKNOWN;
};

class ServiceClassMap : public object, public std::enable_shared_from_this<ServiceClassMap> {
public:
    using std::enable_shared_from_this<ServiceClassMap>::shared_from_this;
};

extern object ServiceType, satisfies, ServiceTypeRegistry;
class Service : public object, public std::enable_shared_from_this<Service> {
public:
    using std::enable_shared_from_this<Service>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    static ServiceTypeName serviceType;

    static string serviceName;

    ServiceTypeName serviceType;

    string serviceName;

    Service(std::shared_ptr<IAgentRuntime> runtime = undefined);
    virtual std::shared_ptr<Promise<void>> stop() = 0;
    string capabilityDescription;

    Metadata config;

    static std::shared_ptr<Promise<std::shared_ptr<Service>>> start(std::shared_ptr<IAgentRuntime> _runtime);
    static std::shared_ptr<Promise<any>> stop(std::shared_ptr<IAgentRuntime> _runtime);
};

template <typename ConfigType, typename ResultType>
class TypedService : public Service, public std::enable_shared_from_this<TypedService<ConfigType, ResultType>> {
public:
    using std::enable_shared_from_this<TypedService<ConfigType, ResultType>>::shared_from_this;
    ConfigType config;

    virtual std::shared_ptr<Promise<ResultType>> process(any input) = 0;
};

template <typename T>
any getTypedService(std::shared_ptr<IAgentRuntime> runtime, ServiceTypeName serviceType);

class ServiceError : public object, public std::enable_shared_from_this<ServiceError> {
public:
    using std::enable_shared_from_this<ServiceError>::shared_from_this;
    string code;

    string message;

    any details;

    std::shared_ptr<Error> cause;
};

std::shared_ptr<ServiceError> createServiceError(any error, string code = std:("UNKNOWN_ERROR"));

template <typename T>
any getTypedService(std::shared_ptr<IAgentRuntime> runtime, ServiceTypeName serviceType)
{
    return runtime->getService<T>(serviceType);
};


#endif
