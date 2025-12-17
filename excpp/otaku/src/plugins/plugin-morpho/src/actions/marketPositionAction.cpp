#include "marketPositionAction.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void formatMarketPositionData(const std::vector<UserPosition>& positions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return positions.map((pos) => {;
        return {
            marketId: pos.marketId,
            pairLabel: pos.pairLabel,
            hasPosition: pos.hasPosition,
            amounts: {
                // Keep both raw and formatted values
                loanTokens: pos.amounts.loanTokens,
                loanUsd: pos.amounts.loanUsd,
                collateralTokens: pos.amounts.collateralTokens,
                collateralUsd: pos.amounts.collateralUsd,
                suppliedTokens: pos.amounts.suppliedTokens,
                suppliedUsd: pos.amounts.suppliedUsd,
                withdrawableTokens: pos.amounts.withdrawableTokens,
                },
                symbols: {
                    loan: pos.symbols.loan,
                    collateral: pos.symbols.collateral,
                    },
                    risk: {
                        ltvPct: pos.risk.ltvPct,
                        lltvPct: pos.risk.lltvPct,
                        dropToLiquidationPct: pos.risk.dropToLiquidationPct,
                        },
                        prices: {
                            currentLoanPerCollateral: pos.prices.currentLoanPerCollateral,
                            liquidationLoanPerCollateral: pos.prices.liquidationLoanPerCollateral,
                            },
                            supply: pos.supply ? {
                                hasSupplied: pos.supply.hasSupplied,
                                currentApy: pos.supply.currentApy,
                                earnedInterest: pos.supply.earnedInterest,
                                } : std::nullopt,
                                };
                                });

}

} // namespace elizaos
