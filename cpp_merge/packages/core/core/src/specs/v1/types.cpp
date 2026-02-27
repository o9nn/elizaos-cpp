#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/core/src/specs/v1/types.h"

any Service::instance = nullptr;

ServiceType Service::get_serviceType()
{
    throw any(std::make_shared<Error>(std:("Service must implement static serviceType getter")));
    return ServiceType();
}

ServiceType Service::get_serviceType()
{
    return (as<Service>(this->constructor))::get_serviceType();
}


void Main(void)
{
    string_empty + string + std:("-") + string + std:("-") + string + std:("-") + string + std:("-") + string + string_empty;
}

MAIN
