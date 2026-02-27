#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/types.h"

any Service::instance = nullptr;

ServiceType Service::get_serviceType()
{
    throw any(std::make_shared<Error>(std::string("Service must implement static serviceType getter")));
    return ServiceType();
}

ServiceType Service::get_serviceType()
{
    return (as<Service>(this->constructor))::get_serviceType();
}


void Main(void)
{
    string_empty + string + std::string("-") + string + std::string("-") + string + std::string("-") + string + std::string("-") + string + string_empty;
}

MAIN
