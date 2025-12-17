#include "sellService.hpp"
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

double calculateAmountOutBuy(double reserveToken, double amount, double _solDecimals, double reserveLamport, double platformBuyFee) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    console.log('calculateAmountOutBuy inputs:', {
        reserveToken,
        amount,
        _solDecimals,
        reserveLamport,
        platformBuyFee,
        });

        const auto feeBasisPoints = new BN(convertToBasisPoints(platformBuyFee));
        std::cout << 'feeBasisPoints:' << feeBasisPoints.toString() << std::endl;

        const auto amountBN = new BN(amount);
        std::cout << 'amountBN:' << amountBN.toString() << std::endl;

        const auto adjustedAmount = amountBN.multipliedBy(new BN(10000)).minus(feeBasisPoints).dividedBy(new BN(10000));
        std::cout << 'adjustedAmount:' << adjustedAmount.toString() << std::endl;

        const auto reserveTokenBN = new BN(reserveToken.toString());
        std::cout << 'reserveTokenBN:' << reserveTokenBN.toString() << std::endl;

        const auto numerator = reserveTokenBN.multipliedBy(adjustedAmount);
        std::cout << 'numerator:' << numerator.toString() << std::endl;

        const auto denominator = new BN(reserveLamport.toString()).plus(adjustedAmount);
        std::cout << 'denominator:' << denominator.toString() << std::endl;

        const auto out = numerator.dividedBy(denominator).toNumber();
        std::cout << 'final output:' << out << std::endl;
        return out;

}

double calculateAmountOutSell(double reserveLamport, double amount, double _tokenDecimals, double platformSellFee, double reserveToken) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Input validation
        if (reserveLamport < 0) throw new Error('reserveLamport must be non-negative');
        if (amount < 0) throw new Error('amount must be non-negative');
        if (platformSellFee < 0) throw new Error('platformSellFee must be non-negative');
        if (reserveToken < 0) throw new Error('reserveToken must be non-negative');

        const auto feeBasisPoints = convertToBasisPoints(platformSellFee);
        const auto amountBN = new BN(amount);

        // Apply fee: adjusted_amount = amount * (10000 - fee_basis_points) / 10000
        const auto adjustedAmount = amountBN.multipliedBy(new BN(10000).minus(new BN(feeBasisPoints))).dividedBy(new BN(10000));

        // For selling tokens: amount_out = reserve_lamport * adjusted_amount / (reserve_token + adjusted_amount)
        const auto numerator = new BN(reserveLamport.toString()).multipliedBy(adjustedAmount);
        const auto denominator = new BN(reserveToken.toString()).plus(adjustedAmount);

        if (denominator.isZero()) throw new Error('Division by zero');

        return numerator.dividedBy(denominator).toNumber();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
