#include "getContractAddress.hpp"

std::shared_ptr<Address> getContractAddress()
{
    return as<std::shared_ptr<Address>>(getOtcAddress());
};


