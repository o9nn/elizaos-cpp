#include "useOTC.hpp"
#include <cstdlib>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

Address getOtcAddress() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (cachedOtcAddress != undefined) {
        return cachedOtcAddress;
    }

    const auto network = getCurrentNetwork();
    const auto deployments = getContracts(network);

    // Get address from deployment config (includes env override handling)
    const auto configAddress = deployments.evm.contracts.otc;
    if (configAddress) {
        if (std::getenv("NODE_ENV") == "development" && !addressLogged) {
            std::cout << "[useOTC] OTC address:" << configAddress << "network:" << network << std::endl;
            addressLogged = true;
        }
        cachedOtcAddress = configAddress;
        return cachedOtcAddress;
    }

    if (!addressLogged) {
        std::cout << "[useOTC] No OTC address found in config" << std::endl;
        addressLogged = true;
    }
    return std::nullopt;

}

void useOTC() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    otcAddress: Address | std::nullopt;
    availableTokens: bigint;
    myOfferIds: bigint[];
    myOffers: (Offer & { id: bigint })[];
    openOfferIds: bigint[];
    openOffers: Offer[];
    agent: Address | std::nullopt;
    isAgent;
    isApprover;
    usdcAddress: Address | std::nullopt;
    ethBalanceWei?: bigint;
    usdcBalance?: bigint;
    minUsdAmount?: bigint;
    maxTokenPerOrder?: bigint;
    quoteExpirySeconds?: bigint;
    defaultUnlockDelaySeconds?: bigint;
    emergencyRefundsEnabled?;
    isLoading;
    error: unknown;
    claim: [&](offerId: bigint) { return Promise<unknown>; };
    isClaiming;
    createOffer: [&](params: {
        tokenAmountWei: bigint;
        discountBps;
        paymentCurrency: 0 | 1;
        lockupSeconds?: bigint;
        }) { return Promise<unknown>; };
        approveOffer: [&](offerId: bigint) { return Promise<unknown>; };
        cancelOffer: [&](offerId: bigint) { return Promise<unknown>; };
        fulfillOffer: [&](offerId: bigint, valueWei?: bigint) { return Promise<unknown>; };
        approveUsdc: [&](amount: bigint) { return Promise<unknown>; };
        emergencyRefund: [&](offerId: bigint) { return Promise<unknown>; };
        withdrawConsignment: [&](consignmentId: bigint) { return Promise<unknown>; };
        createConsignmentOnChain: [&](
        params: ConsignmentParams,
        onTxSubmitted?: (txHash: std:) { return void,
        ) => Promise<ConsignmentCreationResult>; };
        approveToken: [&](tokenAddress: Address, amount: bigint) { return Promise<unknown>; };
        getTokenAddress: [&](tokenId: std:) { return Promise<Address>; };
        getRequiredGasDeposit: [&]() { return Promise<bigint>; };
        getRequiredPayment: [&](
        offerId: bigint,
        currency: "ETH" | "USDC",
        ) { return Promise<bigint>; };

}

Promise<`0x$ writeContractAsync(std::optional<ReadContractConfig> config) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    std:;
}

} // namespace elizaos
