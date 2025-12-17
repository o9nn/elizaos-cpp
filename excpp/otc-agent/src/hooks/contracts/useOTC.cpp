#include "useOTC.hpp"
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
        if (process.env.NODE_ENV == "development" && !addressLogged) {
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
    isAgent: boolean;
    isApprover: boolean;
    usdcAddress: Address | std::nullopt;
    ethBalanceWei?: bigint;
    usdcBalance?: bigint;
    minUsdAmount?: bigint;
    maxTokenPerOrder?: bigint;
    quoteExpirySeconds?: bigint;
    defaultUnlockDelaySeconds?: bigint;
    emergencyRefundsEnabled?: boolean;
    isLoading: boolean;
    error: unknown;
    claim: (offerId: bigint) => Promise<unknown>;
    isClaiming: boolean;
    createOffer: (params: {
        tokenAmountWei: bigint;
        discountBps: number;
        paymentCurrency: 0 | 1;
        lockupSeconds?: bigint;
        }) => Promise<unknown>;
        approveOffer: (offerId: bigint) => Promise<unknown>;
        cancelOffer: (offerId: bigint) => Promise<unknown>;
        fulfillOffer: (offerId: bigint, valueWei?: bigint) => Promise<unknown>;
        approveUsdc: (amount: bigint) => Promise<unknown>;
        emergencyRefund: (offerId: bigint) => Promise<unknown>;
        withdrawConsignment: (consignmentId: bigint) => Promise<unknown>;
        createConsignmentOnChain: (
        params: ConsignmentParams,
        onTxSubmitted?: (txHash: string) => void,
        ) => Promise<ConsignmentCreationResult>;
        approveToken: (tokenAddress: Address, amount: bigint) => Promise<unknown>;
        getTokenAddress: (tokenId: string) => Promise<Address>;
        getRequiredGasDeposit: () => Promise<bigint>;
        getRequiredPayment: (
        offerId: bigint,
        currency: "ETH" | "USDC",
        ) => Promise<bigint>;

}

Promise<`0x$ writeContractAsync(std::optional<ReadContractConfig> config) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    string;
}

} // namespace elizaos
