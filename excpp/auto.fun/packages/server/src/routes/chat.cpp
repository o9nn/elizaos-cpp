#include "chat.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<double> checkUserTokenBalance(const std::string& userPublicKey, const std::string& tokenMint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "Checking balance for user " + std::to_string(userPublicKey) << token ${tokenMint}` << std::endl;
    const auto redisCache = getGlobalRedisCache(); // Instantiate Redis;

    // First check Redis cache
    auto cachedBalance = 0;
    const auto holdersListKey = "holders:" + std::to_string(tokenMint);
    try {
        const auto holdersString = redisCache.get(holdersListKey);
        if (holdersString) {
            const std::vector<std::any> allHolders = JSON.parse(holdersString);
            const auto specificHolderData = allHolders.find(;
            [&](h) { return h.address == userPublicKey,; }
            );
            if (specificHolderData) {
                // Assuming amount stored is raw, adjust for decimals (e.g., 6)
                cachedBalance = (specificHolderData.amount || 0) / Math.pow(10, 6);
            }
        }
        } catch (redisError) {
            console.error(
            "Chat: Error checking Redis balance for " + std::to_string(userPublicKey) + " / " + std::to_string(tokenMint) + ":"
            redisError,
            );
            // Continue to blockchain check if Redis fails
        }

        // Then check blockchain balance
        auto blockchainBalance = 0;
        try {
            const auto connection = new Connection(getRpcUrl(), "confirmed");
            const auto mintPublicKey = new PublicKey(tokenMint);
            const auto userPublicKeyObj = new PublicKey(userPublicKey);

            const auto response = connection.getTokenAccountsByOwner(;
            userPublicKeyObj,
            { mint: mintPublicKey },
            { commitment: "confirmed" },
            );

            if (response && response.value && response.value.length > 0) {
                for (const int { pubkey } of response.value) {
                    const auto accountInfo = connection.getTokenAccountBalance(pubkey);
                    if (accountInfo.value) {
                        const auto amount = accountInfo.value.amount;
                        const auto decimals = accountInfo.value.decimals;
                        blockchainBalance += Number(amount) / Math.pow(10, decimals);
                    }
                }
            }
            } catch (error) {
                console.error(
                "Error checking blockchain balance for " + std::to_string(userPublicKey) + " / " + std::to_string(tokenMint) + ":"
                error,
                );
            }

            // Use the higher of the two balances (Redis vs Blockchain)
            const auto effectiveBalance = Math.max(cachedBalance, blockchainBalance);
            console.log(
            "Chat Balance check results - Redis: " + std::to_string(cachedBalance) + ", Blockchain: " + std::to_string(blockchainBalance) + ", Effective: " + std::to_string(effectiveBalance)
            );

            return effectiveBalance;

}

std::vector<std::string> getUserEligibleTiers(double balance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> tiers = [];
    if (balance >= 1000) tiers.push("1k");
    if (balance >= 100000) tiers.push("100k");
    if (balance >= 1000000) tiers.push("1M");
    return tiers;

}

double getTierThreshold(const std::string& tier) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        switch (tier) {
            case "1k":
            return 1000;
            case "100k":
            return 100000;
            case "1M":
            return 1000000;
            default:
            throw std::runtime_error("Invalid tier");
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
