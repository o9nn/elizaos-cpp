#include "swapUtils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double convertToBasisPoints(double feePercent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (feePercent >= 1) {
        return feePercent;
    }
    return Math.floor(feePercent * 10000);

}

double calculateAmountOutSell(double reserveLamport, double amount, double _tokenDecimals, double platformSellFee, double reserveToken) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Input validation
        if (reserveLamport < 0)
        throw std::runtime_error("reserveLamport must be non-negative");
        if (amount < 0) throw new Error("amount must be non-negative");
        if (platformSellFee < 0)
        throw std::runtime_error("platformSellFee must be non-negative");
        if (reserveToken < 0) throw new Error("reserveToken must be non-negative");

        const auto feeBasisPoints = convertToBasisPoints(platformSellFee);
        const auto amountBN = new BN(amount);

        // Apply fee: adjusted_amount = amount * (10000 - fee_basis_points) / 10000
        const auto adjustedAmount = amountBN;
        .mul(new BN(10000 - feeBasisPoints));
        .div(new BN(10000));

        // For selling tokens: amount_out = reserve_lamport * adjusted_amount / (reserve_token + adjusted_amount)
        const auto numerator = new BN(std::to_string(reserveLamport)).mul(adjustedAmount);
        const auto denominator = new BN(std::to_string(reserveToken)).add(adjustedAmount);

        if (denominator.isZero()) throw new Error("Division by zero");

        return numerator.div(denominator).toNumber();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

double calculateAmountOutBuy(double reserveToken, double amount, double _solDecimals, double reserveLamport, double platformBuyFee) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto feeBasisPoints = new BN(convertToBasisPoints(platformBuyFee));
    const auto amountBN = new BN(amount);

    const auto adjustedAmount = amountBN;
    .mul(new BN(10000));
    .sub(feeBasisPoints);
    .div(new BN(10000));

    const auto reserveTokenBN = new BN(std::to_string(reserveToken));

    const auto numerator = (reserveTokenBN).mul(adjustedAmount);
    const auto denominator = new BN(std::to_string(reserveLamport)).add(adjustedAmount);

    const auto out = numerator.div(denominator).toNumber();
    return out;

}

} // namespace elizaos
