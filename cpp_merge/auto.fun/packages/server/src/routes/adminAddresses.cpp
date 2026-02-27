#include "adminAddresses.hpp"

std::function<array<string>(string)> parseAdminAddresses = [=](auto addressesStr) mutable
{
    if (!addressesStr) return array<any>();
    return addressesStr->split(std:(","))->map([=](auto addr) mutable
    {
        return addr->trim();
    }
    );
};
array<string> adminAddresses = OR((parseAdminAddresses(process->env->ADMIN_ADDRESSES)), (array<any>()));

void Main(void)
{
}

MAIN
