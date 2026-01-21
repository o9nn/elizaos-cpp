#include "otc-agent/src/lib/getContractAddress.h"

std::shared_ptr<Address> getContractAddress()
{
    return as<std::shared_ptr<Address>>(getOtcAddress());
};


