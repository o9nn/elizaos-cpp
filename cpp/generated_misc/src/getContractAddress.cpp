#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/lib/getContractAddress.h"

std::shared_ptr<Address> getContractAddress()
{
    return as<std::shared_ptr<Address>>(getOtcAddress());
};


