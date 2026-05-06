#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_RAYDIUM_SRC_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_RAYDIUM_SRC_UTILS_H
#include "core.h"
#include "@coral-xyz/anchor.h"
#include "@solana/spl-token.h"
#include "@solana/web3.js.h"

extern double fixedPoint;
extern string vaultConfigSeed;
extern string positionSeed;
extern string claimerInfoSeed;
extern string nftFaucetSeed;
extern any token0;
extern any claimer_address_0;
extern string devnetEndpoint;
template <typename T>
std::shared_ptr<Promise<T>> retryOperation(std::function<std::shared_ptr<Promise<T>>()> operation, double maxRetries, double delay);

extern std::function<std::shared_ptr<Promise<any>>(any, any, any, any)> sendSolTo;
extern std::function<std::shared_ptr<Promise<void>>(any, any, any, any, any)> sendTokenTo;
extern std::function<std::shared_ptr<Promise<any>>(any, any, any, any)> sendNftTo;
extern std::function<boolean(any)> isDevnet;
template <typename T>
std::shared_ptr<Promise<T>> retryOperation(std::function<std::shared_ptr<Promise<T>>()> operation, double maxRetries, double delay)
{
    for (auto attempt = 0; attempt < maxRetries; attempt++)
    {
        try
        {
            return std::async([=]() { operation(); });
        }
        catch (const any& error)
        {
            if (attempt == maxRetries - 1) throw any(error);
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, delay);
            }
            ); });
        }
    }
    throw any(std::make_shared<Error>(std::string("Unreachable")));
};


#endif
