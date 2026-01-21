#include "viem-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> readERC20Info(PublicClient client, Address tokenAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    symbol: string; name: string; decimals: number
}

std::future<bigint> readERC20Balance(PublicClient client, Address tokenAddress, Address account) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return safeReadContract<bigint>(client, {;
        address: tokenAddress,
        abi: ERC20_ABI,
        functionName: "balanceOf",
        args: [account],
        });

}

} // namespace elizaos
